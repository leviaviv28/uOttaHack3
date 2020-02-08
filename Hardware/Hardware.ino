#include "WiFi.h"
#include <HTTPClient.h>
#include <PubSubClient.h>

#define MAX_RETRY 3

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
  char t_route[21];
  strcat(t_route,"update_tracking");
  String t = String(tracking);
  for (int i=1; i<6;i++){
    t_route[15+i]=t.charAt(i);
  }
  //t_route[20] = '\0';
  const char* t_SSID = WiFi.SSID().c_str();
	Serial.println(client.publish(t_route, t_SSID));
	Serial.println(t_route);
}

void setup() {
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
 }


void loop() { 
  if(WiFi.status() == WL_CONNECTED) {   //Check WiFi connection status
    Serial.println(client.connect("esp32test", mqttUser, mqttPassword));
    sendUpdate();
  } else {
      //ESP.restart();
    scanWiFi();
  }
  delay(5000);
}
