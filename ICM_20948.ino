#include "ICM_20948.h"

#define USE_SPI
#define SPI_PORT SPI

ICM_20948_SPI myICM;

void setup() {

  Serial.begin(115200);
  while (!Serial){
  };

  initializationICM(myICM);
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

  bool initialized = false;
  while (!initialized){

    Serial.print(F("Sensor initialization returned: "));
    Serial.println(myICM.statusString());

    if (myICM.status != ICM_20948_Stat_Ok){
      Serial.println("Trying again...");
      delay(500);

    } else {
      initialized = true;
    }
  }
  SPI_PORT.begin();
}

void getDataICM(ICM_20948_SPI &myICM) {
  /*
    Checks for available data from the ICM-20948 sensor. If data is ready,
    it calls the `getAGMT` function to retrieve accelerometer, gyroscope,
    magnetometer, and temperature measurements. Then, it prints these data
    on the serial port using the `printData` function.
  */

  if (myICM.dataReady()) {

    myICM.getAGMT();
    float* arrayOfData = getDataArray(myICM);
    printData(arrayOfData);
    delay(10);

  } else {

    Serial.println("Waiting for data...");
    delay(500);

  }
}

float* getDataArray(ICM_20948_SPI &myICM) {
  /*
    This function takes an instance of the ICM_20948_SPI class and returns
    a pointer to an array of floats with the 10 measurements made by the sensor 
    in the following order: acc_x, acc_y, acc_z, gyr_x, gyr_y, gyr_z, mag_x, mag_y, 
    mag_z, and temp.
  */

  static float arrayOfData[10];

  arrayOfData[0] = myICM.accX();
  arrayOfData[1] = myICM.accY();
  arrayOfData[2] = myICM.accZ();
  arrayOfData[3] = myICM.gyrX();
  arrayOfData[4] = myICM.gyrY();
  arrayOfData[5] = myICM.gyrZ();
  arrayOfData[6] = myICM.magX();
  arrayOfData[7] = myICM.magY();
  arrayOfData[8] = myICM.magZ();
  arrayOfData[9] = myICM.temp();

  return arrayOfData;
}

void printData(float arrayOfData[10]) {
  /*
    This function, named printData, takes an array of 10 integers as input and prints 
    the data along with corresponding labels to the Serial port. The labels include 
    information about accelerometer (Acc_X, Acc_Y, Acc_Z), gyroscope (Gyr_X, Gyr_Y, Gyr_Z), 
    magnetometer (Mag_X, Mag_Y, Mag_Z), and temperature (Temp). The function iterates 
    through the array of data, printing each label followed by its corresponding data 
    value. The data is sent to the Serial port for monitoring or debugging purposes.
  */
  const char arrayOfLabel[10][20] = {"Acc_X: ", " | Acc_Y: ", " | Acc_Z: ", " | Gyr_X: ", " | Gyr_Y: ", " | Gyr_Z: ", " | Mag_X: ", " | Mag_Y: ", " | Mag_Z: ", " | Temp: "};
  
  for (int i = 0; i < 10; i++) {
    Serial.print(arrayOfLabel[i]);
    if (i == 9) {
      Serial.println(arrayOfData[i]);
    } else {
      Serial.print(arrayOfData[i]);
    }
  }
}