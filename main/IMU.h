#include "ICM_20948.h" 

class IMU_20948 {

  public:
    ICM_20948_SPI myICM;
    int IMUPin;
    const char* IMUName;
    IMU_20948(int pinIMU, const char* nameIMU);
    void readData();
    float getFrequency(); // TERMINAR

  private:
    void activate_sensors(bool *success);
    void setOdrRate(bool *success);
};