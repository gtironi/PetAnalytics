#include "IMU.h"

#define SPI_FREQ 5000000
#define CS_PIN 2

#define accFFS gpm2 // [2, 4, 8 or 16]
#define gyrFFS dps250 // [250, 500, 1000 or 2000]

// Continuous or Cycled
#define accSampleMode ICM_20948_Sample_Mode_Continuous 
#define gyrSampleMode ICM_20948_Sample_Mode_Continuous

// ODR = Base Sample Rate/1+SMPLRT_DIV -> SMPLRT_DIV = 10, ODR = 100 hz e Base Sample Rate = 1100 hz** para o acelerômetro é 1100 hz e para o giroscópio é 1125hz 
#define accSampleRate 10
#define gyrSampleRate 10

IMUClass::IMUClass(uint8_t csPin) : csPin(csPin), imu() {}

void IMUClass::begin() {
  imu.enableDebugging();
  
  bool initialized = false;
  while (!initialized){
  imu.begin(csPin, SPI, SPI_FREQ);
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

  set(csPin);
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

void IMUClass::set(uint8_t csPin){
/*
  This function performs essential setup tasks for the ICM-20948 sensor, 
  including a software reset, deactivation of sleep and low-power modes, 
  configuration of sample modes, setting full-scale ranges, configuring 
  Digital Low-Pass Filters, and initializing the magnetometer. The specific 
  configurations for sample modes, full-scale ranges, DLPF, and magnetometer 
  are expected to be provided externally.
  */

  imu.swReset();
  checkingStatus("Software Reset");

  // Deactivate sleep mode and low-power mode to prepare the sensor for operation
  imu.sleep(false);
  imu.lowPower(false);

  // Sample Mode
  imu.setSampleMode(ICM_20948_Internal_Acc, accSampleMode);
  imu.setSampleMode(ICM_20948_Internal_Gyr, gyrSampleMode);
  checkingStatus("setSampleMode");

  // Sample Rate (vai ser usada pois não habilitamos DMP e o FIFO)
  ICM_20948_smplrt_t smplrt;
  smplrt.a = accSampleRate;
  smplrt.g = gyrSampleRate;
  imu.setSampleRate((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), smplrt);
  checkingStatus("setSampleRate");

  // Full Scale Settings
  ICM_20948_fss_t myFSS;
  myFSS.a = accFFS;
  myFSS.g = gyrFFS;
  imu.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);
  checkingStatus("setFullScale");

  // Magnetometer
  imu.startupMagnetometer();
  checkingStatus("startupMagnetometer");
}

void IMUClass::checkstatus(char functionName){
    /*
  This function checks the status of an operation performed by a function in the ICM-20948 library.
  If the operation indicates an error, it prints a debug message containing the function name and 
  the corresponding status message.
  */

  if (imu.status != ICM_20948_Stat_Ok)
  {
    Serial.print(F(functionName));
    Serial.print(F(" returned: "));
    Serial.println(imu.statusString());
  }
}