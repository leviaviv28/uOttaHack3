#include <Wire.h>
#include <math.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

//accelerometer stuff
#define GY_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

//DHT stuff
#define DHTTYPE DHT11
#define DHTPIN 2
DHT_Unified dht(DHTPIN, DHTTYPE);

//last time led flashed
#define LED 4
long prev = 0;

//warning LED
#define WARN_LED 7
int warn = 0;
//bit-0 = upside down, bit-1 = high temp, bit-2 = low temp, bit-3 = high humidity

//LED states
#define NOT_CONNECTED 25
#define CONNECTED 2000

void flash_led(int delay_ms) {
   if (millis() - prev > delay_ms) {
     digitalWrite(LED, HIGH);
     delay(10);
     digitalWrite(LED, LOW);      
     prev = millis();
   }
}

void setup() {
  Serial.begin(9600);

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

  int ax, ay, az, temp, gx, gy, gz;
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
    double x = ax;
    double y = ay;
    double z = az;

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
      double humidity = event.relative_humidity;
      Serial.println(humidity);
      if (humidity > 60 || humidity < 15) {
        warn += 8; //set bit-3 = 1
      }
    } else {
      Serial.println("");
    }

    flash_led(CONNECTED);

    Serial.println(warn);
    if (warn & 0001 || warn & 0100 || warn & 0010 || warn & 1000) { //mask to check each bit
      digitalWrite(WARN_LED, HIGH);
    } else {
      digitalWrite(WARN_LED, LOW);
    }
  }
}
