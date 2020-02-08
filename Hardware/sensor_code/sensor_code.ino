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

    //Reading DHT
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (!isnan(event.temperature)) {
      Serial.print(", temp: ");
      Serial.print(event.temperature);
    }

    dht.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity)) {
      Serial.print(", humditiy: ");
      Serial.println(event.relative_humidity);
    } else {
      Serial.println("");
    }

  }
}
