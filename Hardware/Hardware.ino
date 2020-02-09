#include "WiFi.h"
#include <HTTPClient.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <math.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define MAX_RETRY 3

//accelerometer stuff
#define GY_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
bool upside = true;

//DHT stuff
#define DHTTYPE DHT11
#define DHTPIN 2
DHT_Unified dht(DHTPIN, DHTTYPE);
double humidity = 0.0;
double temp = 0.0;

//last time led flashed
#define LED 4
long prev = 0;

//warning LED
#define WARN_LED 18
int warn = 0;
//bit-0 = upside down, bit-1 = high temp, bit-2 = low temp, bit-3 = high humidity

//LED states
#define NOT_CONNECTED 25
#define CONNECTED 2000

long prev_scan = 0;

// Setting variables for timer ISRs
volatile int interruptCounter;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 5000;
int totalInterruptCounter;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
long tracking = 12345;
// WiFi Configuration Variables
const char* wifiPrefix = "InnovaChamps";
const char* serverAddress = "http://34.66.100.203:80/update_location";
const char* password =  "nohackplz";
// MQTT Configuration Variables
const char* mqttServer = "mr2hd0llj3vwjx.messaging.solace.cloud";
const int mqttPort = 1883;
const char* mqttUser = "solace-cloud-client";
const char* mqttPassword = "vvngpe3cn2ss72a7425hgf1l7f";
WiFiClient espClient;
PubSubClient client(espClient);

void scanWiFi() {
	Serial.println("scan start");
	int n = WiFi.scanNetworks();
	Serial.println("scan done");
	if (n == 0) {
		Serial.println("no networks found");
    flash_led(NOT_CONNECTED);
	} else {
		Serial.print(n);
		Serial.println(" networks found");
		for (int i = 0; i < n; i++) {
			// Print SSID and RSSI for each network found
			// Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
      Serial.println(WiFi.RSSI(i));
      Serial.println(WiFi.RSSI());
			// Check if WiFi is valid InnovaPost address, and that signal is stronger than current connection
      char __SSID[sizeof(WiFi.SSID(i))];
      WiFi.SSID(i).toCharArray(__SSID, sizeof(__SSID));
      Serial.println(__SSID);
			if (WiFi.status() != WL_CONNECTED) {
			  WiFi.begin(__SSID, password);
        Serial.println(__SSID);
        delay(3000);
        startMillis = millis();  //initial start time
				while (WiFi.status() != WL_CONNECTED &&  millis() - startMillis <= period) {//test whether the period has elapsed //Check for the connection
				  Serial.println("Connecting to " + WiFi.SSID(i));
         }
				Serial.println("Connected to " + WiFi.SSID(i));
			}
		}
	}
}

void sendUpdate() {
  char t_route[22];
  String st = String("update_tracking/");
  for (int i=0; i<16;i++){
    t_route[i]=st.charAt(i);
  }
  String t = String(tracking);
  for (int i=0; i<5;i++){
    t_route[16+i]=t.charAt(i);
  }
  t_route[21] = '\0';
  const char* t_SSID = WiFi.SSID().c_str();
	Serial.println(client.publish(t_route, t_SSID));
	Serial.println(t_route);
  sendEnviron();
}

void sendEnviron() {
  char t_route[25];
  String st = String("update_environment/");
  for (int i=0; i<19;i++){
    t_route[i]=st.charAt(i);
  }
  String t = String(tracking);
  for (int i=0; i<5;i++){
    t_route[19+i]=t.charAt(i);
  }

  t_route[24] = '\0';
  
  char environ_info[14];
 
  //add 'yaw' (if upside-right) to string
  if (upside) {
    environ_info[0] = '1';
  } else {
    environ_info[0] = '0';
  }
  environ_info[1] = ',';

  //add temperature to string
  String temp_str = String(temp);
  if (temp_str.length() < 5) {
    temp_str = "0" + temp_str;
  }
  for (int i=0; i<5;i++){
    environ_info[2+i]=temp_str.charAt(i);
  }

  environ_info[7] = ',';

  //add humidity to string
  String humidity_str = String(humidity);
  if (humidity_str.length() < 5) {
    humidity_str = "0" + humidity_str;
  }
  for (int i=0; i<5;i++){
    environ_info[8+i]=humidity_str.charAt(i);
  }
  
  environ_info[13] = '\0';
  
  Serial.println(client.publish(t_route, environ_info));
  Serial.println(t_route);
}

void flash_led(int delay_ms) {
   if (millis() - prev > delay_ms) {
     digitalWrite(LED, HIGH);
     delay(10);
     digitalWrite(LED, LOW);      
     prev = millis();
   }
}

void setup() {
  //setting pinmodes
  pinMode(DHTPIN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(WARN_LED, OUTPUT);
  
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);

	client.setServer(mqttServer, mqttPort);

	Serial.println("Setup done");
	delay(4000);   //Delay needed before calling the WiFi.begin
	// Connect to nearest InnovaPost WiFi 
	scanWiFi();
  Serial.println("scanned");

  //setup accelerometer
  Wire.begin();
  Wire.beginTransmission(GY_ADDR);

  //Set the power managment register to 0, this wakes up the chip
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  
  //send the transmission
  Wire.endTransmission(false);

  //setup DHT
  dht.begin();
 }


void loop() {
  //reset warnings
  warn = 0;
  
  //Reading Accelerometer
  Wire.beginTransmission(GY_ADDR);
  
  //start at the first register for the accelerometer
  Wire.write(ACCEL_XOUT_H);
  
  //send the address
  Wire.endTransmission(false);

  //request the values
  int w = Wire.requestFrom(GY_ADDR, 14);

  int ax, ay, az;//, temp, gx, gy, gz;
  if (Wire.available()) {
    //accelerometer values
    ax = Wire.read() << 8 | Wire.read();
    ay = Wire.read() << 8 | Wire.read();
    az = Wire.read() << 8 | Wire.read();

    //temperature
//    temp = Wire.read() << 8 | Wire.read();

    //gyroscope value
//    gx = Wire.read() << 8 | Wire.read();
//    gy = Wire.read() << 8 | Wire.read();
//    gz = Wire.read() << 8 | Wire.read();

    double roll, pitch, yaw;
    double x = (int16_t)ax;
    double y = (int16_t)ay;
    double z = (int16_t)az;
    //need to cast to int16_t because ESP sends it weird

    roll = atan(y/sqrt((x * x)+(z + z))) * (180.0/3.14);
    pitch = atan(x/sqrt((y * y) + (z * z))) * (180.0/3.14);
    yaw = atan(z/sqrt((x * x) + (y * y))) * (180.0/3.14);

    Serial.print("roll: ");
    Serial.print(String(roll));
    Serial.print(", pitch: ");
    Serial.print(String(pitch));
    Serial.print(", yaw: ");
    Serial.print(String(yaw));
//    Serial.print(", temp: ");
//    Serial.println(String(temp/340 + 36.53));
//    Serial.print(", gx: ");
//    Serial.print(String(gx));
//    Serial.print(", gy: ");
//    Serial.print(String(gy));
//    Serial.print(", gz: ");
//    Serial.println(String(gz));

    //checking whether it's upside down
    if (yaw < 0) {
      warn += 1; //set bit-0 = 1
      upside = false;
    } else {
      upside = true;
    }

    //Reading DHT
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
      double temp = event.temperature;
      Serial.print(", temp: ");
      Serial.print(temp);
      if (temp > 50) {
        warn += 2;
      } else if (temp < -15) {
        warn -= 4;
      }
    }

    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
      Serial.print(", humditiy: ");
      humidity = event.relative_humidity;
      Serial.println(humidity);
      if (humidity > 60 || humidity < 15) {
        warn += 8; //set bit-3 = 1
      }
    } else {
      Serial.println("");
    }

    Serial.println(warn);
    if (warn & 0001 || warn & 0100 || warn & 0010 || warn & 1000) { //mask to check each bit
      digitalWrite(WARN_LED, HIGH);
    } else {
      digitalWrite(WARN_LED, LOW);
    }
  }
   
  if (millis() - prev_scan > 5000) {
    if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
      Serial.println(client.connect("esp32test", mqttUser, mqttPassword));
      sendUpdate();
      flash_led(CONNECTED);
    } else {
        //ESP.restart();
      scanWiFi();
      flash_led(NOT_CONNECTED);
    }
    //delay(5000);
    prev_scan = millis();
  }
}
