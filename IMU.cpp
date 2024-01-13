#include "IMU.h"

IMUClass::IMUClass(uint8_t csPin) : csPin(csPin), imu() {}

void IMUClass::begin() {
  imu.enableDebugging();
  
  bool initialized = false;
  while (!initialized){
  imu.begin(csPin, SPI);
  Serial.print("Sensor");
  Serial.print(csPin);
  Serial.print("initialization returned: ");
  Serial.println(imu.statusString());

  if (imu.status != ICM_20948_Stat_Ok)
  {
    Serial.println(F("Trying again..."));
    delay(500);
  }
  else
  {
    initialized = true;
  }}
}

void IMUClass::readRawData() {
  if (imu.dataReady()) {
    imu.getAGMT();
    Serial.print("IMU ");
    Serial.print(csPin);
    Serial.print(":");
    printRawAGMT(imu.agmt);
  }
}

void IMUClass::printRawAGMT(ICM_20948_AGMT_t agmt) {
  Serial.print("RAW. Acc [ ");
  printPaddedInt16b(agmt.acc.axes.x);
  Serial.print(", ");
  printPaddedInt16b(agmt.acc.axes.y);
  Serial.print(", ");
  printPaddedInt16b(agmt.acc.axes.z);
  Serial.print(" ], Gyr [ ");
  printPaddedInt16b(agmt.gyr.axes.x);
  Serial.print(", ");
  printPaddedInt16b(agmt.gyr.axes.y);
  Serial.print(", ");
  printPaddedInt16b(agmt.gyr.axes.z);
  Serial.print(" ], Mag [ ");
  printPaddedInt16b(agmt.mag.axes.x);
  Serial.print(", ");
  printPaddedInt16b(agmt.mag.axes.y);
  Serial.print(", ");
  printPaddedInt16b(agmt.mag.axes.z);
  Serial.print(" ]");
  Serial.println();
}

void IMUClass::printPaddedInt16b(int16_t val) {
  if (val > 0) {
    Serial.print(" ");
    if (val < 10000) {
      Serial.print("0");
    }
    if (val < 1000) {
      Serial.print("0");
    }
    if (val < 100) {
      Serial.print("0");
    }
    if (val < 10) {
      Serial.print("0");
    }
  } else {
    Serial.print("-");
    if (abs(val) < 10000) {
      Serial.print("0");
    }
    if (abs(val) < 1000) {
      Serial.print("0");
    }
    if (abs(val) < 100) {
      Serial.print("0");
    }
    if (abs(val) < 10) {
      Serial.print("0");
    }
  }
  Serial.print(abs(val));
}
