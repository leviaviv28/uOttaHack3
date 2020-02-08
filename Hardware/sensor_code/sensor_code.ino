# include <Wire.h>

#define GY_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

void setup() {
  Wire.begin();
  Wire.beginTransmission(GY_ADDR);

  //Set the power managment register to 0, this wakes up the chip
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  
  //send the transmission
  Wire.endTransmission(false);

  Serial.begin(9600);
}

void loop() {
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
    temp = Wire.read() << 8 | Wire.read();

    //gyroscope value
    gx = Wire.read() << 8 | Wire.read();
    gy = Wire.read() << 8 | Wire.read();
    gz = Wire.read() << 8 | Wire.read();

    Serial.print(String(ax));
  }
}
