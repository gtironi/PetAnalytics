#include "IMU.h"
#include <Arduino.h>
#include <SPI.h>

void setup(){
  Serial.begin(115200);
  SPI.begin();
}

void loop(){
  IMU_20948 IMU1(12, "Primeiro IMU"), IMU2(13, "Segundo IMU"), IMU3(27, "Terceiro IMU");
  while(1){
    IMU1.readData();
    IMU2.readData();
    IMU3.readData();
  }
}