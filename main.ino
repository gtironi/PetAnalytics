#include "IMU.h"

IMUClass imu1(2);  // CS_PIN for IMU 1
IMUClass imu2(4);  // CS_PIN for IMU 2
IMUClass imu3(6);  // CS_PIN for IMU 3

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