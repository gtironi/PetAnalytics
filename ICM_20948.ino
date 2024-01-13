#include "ICM_20948.h"

#define SPI_PORT SPI
#define SPI_FREQ 5000000
#define CS_PIN 2

#define accFFS gpm2 // [2, 4, 8 or 16]
#define gyrFFS dps250 // [250, 500, 1000 or 2000]

// Continuous or Cycled
#define accSampleMode ICM_20948_Sample_Mode_Continuous 
#define gyrSampleMode ICM_20948_Sample_Mode_Continuous

#define accDLPF acc_d473bw_n499bw 
#define gyrDLPF gyr_d361bw4_n376bw5 

ICM_20948_SPI myICM;

void setup() {
  Serial.begin(115200);
  initializationICM(myICM);
  setICM(myICM);
}

void loop() {
  getDataICM(myICM);
}

void initializationICM(ICM_20948_SPI &myICM){
  /*
    Initializes the ICM-20948 sensor, checks the initialization status,
    and prints relevant information on the serial port. It waits until
    the sensor is successfully initialized before proceeding with the program execution.
  */

  SPI_PORT.begin();

  bool initialized = false;
  while (!initialized){
    myICM.begin(CS_PIN, SPI_PORT, SPI_FREQ);

    Serial.print(F("Sensor initialization returned: "));
    Serial.println(myICM.statusString());

    if (myICM.status != ICM_20948_Stat_Ok){
      Serial.println(F("Trying again..."));
      delay(500);

    } else {
      initialized = true;
    }
  }
}

void setICM(ICM_20948_SPI &myICM){
  /*
    This function performs essential setup tasks for the ICM-20948 sensor, 
    including a software reset, deactivation of sleep and low-power modes, 
    configuration of sample modes, setting full-scale ranges, configuring 
    Digital Low-Pass Filters, and initializing the magnetometer. The specific 
    configurations for sample modes, full-scale ranges, DLPF, and magnetometer 
    are expected to be provided externally.
  */

  myICM.swReset();

  // Deactivate sleep mode and low-power mode to prepare the sensor for operation
  myICM.sleep(false);
  myICM.lowPower(false);

  // Sample Mode
  myICM.setSampleMode(ICM_20948_Internal_Acc, accSampleMode);
  myICM.setSampleMode(ICM_20948_Internal_Gyr, gyrSampleMode);

  // Full Scale Settings
  ICM_20948_fss_t myFSS;
  myFSS.a = accFFS;
  myFSS.g = gyrFFS;
  myICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);

  // Digital Low-Pass Filter
  ICM_20948_dlpcfg_t myDLPcfg;
  myDLPcfg.a = accDLPF;
  myDLPcfg.g = gyrDLPF;
  myICM.setDLPFcfg((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myDLPcfg);

  // Magnetometer
  myICM.startupMagnetometer();
}

bool getDataICM(ICM_20948_SPI &myICM) {
  /*
    Checks for available data from the ICM-20948 sensor. If data is ready,
    it calls the `getAGMT` function to retrieve accelerometer, gyroscope,
    magnetometer, and temperature measurements. Then, it prints these data
    on the serial port using the `printScaledAGMT` function.
    
    Returns true if data is obtained successfully, otherwise returns false.
  */

  if (myICM.dataReady()) {
    myICM.getAGMT();
    printScaledAGMT(&myICM);
    return true;

  } else {
    Serial.println(F("Waiting for data..."));
    return false;
  }
}

void printPaddedInt16b(int16_t val)
{
  if (val > 0)
  {
    SERIAL_PORT.print(" ");
    if (val < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (val < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  else
  {
    SERIAL_PORT.print("-");
    if (abs(val) < 10000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 1000)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 100)
    {
      SERIAL_PORT.print("0");
    }
    if (abs(val) < 10)
    {
      SERIAL_PORT.print("0");
    }
  }
  SERIAL_PORT.print(abs(val));
}

void printRawAGMT(ICM_20948_AGMT_t agmt)
{
  SERIAL_PORT.print("RAW. Acc [ ");
  printPaddedInt16b(agmt.acc.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.acc.axes.z);
  SERIAL_PORT.print(" ], Gyr [ ");
  printPaddedInt16b(agmt.gyr.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.gyr.axes.z);
  SERIAL_PORT.print(" ], Mag [ ");
  printPaddedInt16b(agmt.mag.axes.x);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.y);
  SERIAL_PORT.print(", ");
  printPaddedInt16b(agmt.mag.axes.z);
  SERIAL_PORT.print(" ], Tmp [ ");
  printPaddedInt16b(agmt.tmp.val);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
}

void printFormattedFloat(float val, uint8_t leading, uint8_t decimals)
{
  float aval = abs(val);
  if (val < 0)
  {
    SERIAL_PORT.print("-");
  }
  else
  {
    SERIAL_PORT.print(" ");
  }
  for (uint8_t indi = 0; indi < leading; indi++)
  {
    uint32_t tenpow = 0;
    if (indi < (leading - 1))
    {
      tenpow = 1;
    }
    for (uint8_t c = 0; c < (leading - 1 - indi); c++)
    {
      tenpow *= 10;
    }
    if (aval < tenpow)
    {
      SERIAL_PORT.print("0");
    }
    else
    {
      break;
    }
  }
  if (val < 0)
  {
    SERIAL_PORT.print(-val, decimals);
  }
  else
  {
    SERIAL_PORT.print(val, decimals);
  }
}

void printScaledAGMT(ICM_20948_SPI *sensor) 
{
  SERIAL_PORT.print("Scaled. Acc (mg) [ ");
  printFormattedFloat(sensor->accX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->accZ(), 5, 2);
  SERIAL_PORT.print(" ], Gyr (DPS) [ ");
  printFormattedFloat(sensor->gyrX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->gyrZ(), 5, 2);
  SERIAL_PORT.print(" ], Mag (uT) [ ");
  printFormattedFloat(sensor->magX(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magY(), 5, 2);
  SERIAL_PORT.print(", ");
  printFormattedFloat(sensor->magZ(), 5, 2);
  SERIAL_PORT.print(" ], Tmp (C) [ ");
  printFormattedFloat(sensor->temp(), 5, 2);
  SERIAL_PORT.print(" ]");
  SERIAL_PORT.println();
}