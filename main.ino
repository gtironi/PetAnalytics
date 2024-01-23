#include "IMU.h"

IMUClass imu1(12);  // CS_PIN for IMU 1
IMUClass imu2(13);  // CS_PIN for IMU 2
IMUClass imu3(27);  // CS_PIN for IMU 3

void setup() {
  Serial.begin(115200);
  SPI.begin();

  imu1.begin();
  imu2.begin();
  imu3.begin();
}

void loop() {
  imu1.readRawData();
  imu2.readRawData();
  imu3.readRawData();
}
