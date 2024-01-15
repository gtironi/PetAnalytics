#ifndef IMU_H
#define IMU_H

#include "Arduino.h"
#include "ICM_20948.h"
#include <SPI.h>

class IMUClass {
public:
  IMUClass(uint8_t csPin);
  void begin();
  void readRawData();

private:
  uint8_t csPin;
  ICM_20948_SPI imu;
  void printRawAGMT(ICM_20948_AGMT_t agmt);
  void printPaddedInt16b(int16_t val);
  void set(uint8_t csPin);
  void checkstatus(char functionName);
};

#endif