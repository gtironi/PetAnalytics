#include "ICM_20948.h" 

#define SPI_PORT SPI
#define SERIAL_PORT Serial
#define CS_PIN_1 12     
#define CS_PIN_2 13
#define CS_PIN_3 27

ICM_20948_SPI myICM_1; // Cria um objeto ICM_20948_SPI
ICM_20948_SPI myICM_2;
ICM_20948_SPI myICM_3;

void setup(){
  // configurando a comunicação serial
  SERIAL_PORT.begin(115200);
  delay(100);

  // caso seja necessário, comentar a linha seguinte vai destivar a ajuda da biblioteca para debugar o código
  //myICM_1.enableDebugging();
  //myICM_2.enableDebugging();
  //myICM_3.enableDebugging();

  // inicializando o SPI;
  SPI_PORT.begin();

  // loop de inicialização do IMU
  bool initialized = false;
  while (!initialized)
  {

    // inicializa a comunicação com o sensor via SPI
    myICM_1.begin(CS_PIN_1, SPI_PORT);
    myICM_2.begin(CS_PIN_2, SPI_PORT);
    myICM_3.begin(CS_PIN_3, SPI_PORT);

    // imprime o status/resultado da tentativa de comunicação
    SERIAL_PORT.print(F("Initialization of the sensor returned: "));
    SERIAL_PORT.println(myICM_1.statusString());
    SERIAL_PORT.println(myICM_2.statusString());
    SERIAL_PORT.println(myICM_3.statusString());

    // se der errado, vai tentar novamente, caso contrário, a comunicação estará pronta
    if (myICM_1.status != ICM_20948_Stat_Ok && myICM_2.status != ICM_20948_Stat_Ok && myICM_3.status != ICM_20948_Stat_Ok)
    {
      SERIAL_PORT.println(F("Trying again..."));
      delay(500);
    }
    else
    {
      initialized = true;
    }
  }

  // mensagem de sucesso
  SERIAL_PORT.println(F("Device connected!"));

  // variável que vai indicar se as configurações foram bem sucedidas
  bool success = true;

  // vai inicializar o DMP e retornar o status/resultado
  success &= (myICM_1.initializeDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_2.initializeDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_3.initializeDMP() == ICM_20948_Stat_Ok);

  // ativando os sensores
  activate_sensors(myICM_1, &success);
  activate_sensors(myICM_2, &success);
  activate_sensors(myICM_3, &success);

  // ajustando a taxa do ODR para os IMU
  setOdrRate(myICM_1, &success);
  setOdrRate(myICM_2, &success);
  setOdrRate(myICM_3, &success);

  // habilitando o FIFO 
  success &= (myICM_1.enableFIFO() == ICM_20948_Stat_Ok);
  success &= (myICM_2.enableFIFO() == ICM_20948_Stat_Ok);
  success &= (myICM_3.enableFIFO() == ICM_20948_Stat_Ok);

  // habilitando o DMP
  success &= (myICM_1.enableDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_2.enableDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_3.enableDMP() == ICM_20948_Stat_Ok);

  // resetando o DMP
  success &= (myICM_1.resetDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_2.resetDMP() == ICM_20948_Stat_Ok);
  success &= (myICM_3.resetDMP() == ICM_20948_Stat_Ok);

  // resetando a FIFO
  success &= (myICM_1.resetFIFO() == ICM_20948_Stat_Ok);
  success &= (myICM_2.resetFIFO() == ICM_20948_Stat_Ok);
  success &= (myICM_3.resetFIFO() == ICM_20948_Stat_Ok);

  // Verifica o sucesso das configurações acima
  if (success){
    SERIAL_PORT.println(F("DMP enabled!"));}
  else{
    SERIAL_PORT.println(F("Enable DMP failed!"));
    SERIAL_PORT.println(F("Please check that you have uncommented line 29 (#define ICM_20948_USE_DMP) in ICM_20948_C.h..."));
    while (1);}
}

void loop(){
  readData(myICM_1, "1");
  readData(myICM_2, "2");
  readData(myICM_3, "3");
}

void setOdrRate(ICM_20948_SPI &myICM, bool *success){
  *success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Accel, 0) == ICM_20948_Stat_Ok);
  *success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro, 0) == ICM_20948_Stat_Ok);
  *success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Cpass, 0) == ICM_20948_Stat_Ok);
}

void activate_sensors(ICM_20948_SPI &myICM, bool *success){
  // ativação dos sensores
  *success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_GYROSCOPE) == ICM_20948_Stat_Ok);
  *success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_ACCELEROMETER) == ICM_20948_Stat_Ok);
  *success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_MAGNETIC_FIELD_UNCALIBRATED) == ICM_20948_Stat_Ok);
}

void readData(ICM_20948_SPI &myICM, const char* nameICM){
  icm_20948_DMP_data_t data;
  myICM.readDMPdataFromFIFO(&data);

  if((myICM.status != ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)){
    
    float acc_x = (float)data.Raw_Accel.Data.X;
    float acc_y = (float)data.Raw_Accel.Data.Y;
    float acc_z = (float)data.Raw_Accel.Data.Z;

    SERIAL_PORT.print(F("Accel "));
    SERIAL_PORT.print(F(nameICM));
    SERIAL_PORT.print(F(": X:"));
    SERIAL_PORT.print(acc_x);
    SERIAL_PORT.print(F(" Y:"));
    SERIAL_PORT.print(acc_y);
    SERIAL_PORT.print(F(" Z:"));
    SERIAL_PORT.println(acc_z);
    

    
    float x = (float)data.Raw_Gyro.Data.X;
    float y = (float)data.Raw_Gyro.Data.Y;
    float z = (float)data.Raw_Gyro.Data.Z;

    SERIAL_PORT.print(F("Gyro "));
    SERIAL_PORT.print(F(nameICM));
    SERIAL_PORT.print(F(": X:"));
    SERIAL_PORT.print(x);
    SERIAL_PORT.print(F(" Y:"));
    SERIAL_PORT.print(y);
    SERIAL_PORT.print(F(" Z:"));
    SERIAL_PORT.println(z);
    


    float comp_x = (float)data.Compass.Data.X;
    float comp_y = (float)data.Compass.Data.Y;
    float comp_z = (float)data.Compass.Data.Z;

    SERIAL_PORT.print(F("Compass "));
    SERIAL_PORT.print(F(nameICM));
    SERIAL_PORT.print(F(": X:"));
    SERIAL_PORT.print(comp_x);
    SERIAL_PORT.print(F(" Y:"));
    SERIAL_PORT.print(comp_y);
    SERIAL_PORT.print(F(" Z:"));
    SERIAL_PORT.println(comp_z);
  }
}


ICM_20948_Status_e ICM_20948::initializeDMP(void)
{
  ICM_20948_Status_e  result = ICM_20948_Stat_Ok; 
  ICM_20948_Status_e  worstResult = ICM_20948_Stat_Ok;

  
  result = i2cControllerConfigurePeripheral(0, MAG_AK09916_I2C_ADDR, AK09916_REG_RSV2, 10, true, true, false, true, true); if (result > worstResult) worstResult = result;
  
  result = i2cControllerConfigurePeripheral(1, MAG_AK09916_I2C_ADDR, AK09916_REG_CNTL2, 1, false, true, false, false, false, AK09916_mode_single); if (result > worstResult) worstResult = result;

  uint8_t mstODRconfig = 0x04; 
  result = write(AGB3_REG_I2C_MST_ODR_CONFIG, &mstODRconfig, 1); if (result > worstResult) worstResult = result;

  result = setClockSource(ICM_20948_Clock_Auto); if (result > worstResult) worstResult = result; 

  result = setBank(0); if (result > worstResult) worstResult = result;                              
  uint8_t pwrMgmt2 = 0x40;                                                    
  result = write(AGB0_REG_PWR_MGMT_2, &pwrMgmt2, 1); if (result > worstResult) worstResult = result; 

  result = setSampleMode(ICM_20948_Internal_Mst, ICM_20948_Sample_Mode_Cycled); if (result > worstResult) worstResult = result;

  result = enableFIFO(false); if (result > worstResult) worstResult = result;

  result = enableDMP(false); if (result > worstResult) worstResult = result;

  ICM_20948_fss_t myFSS;
  myFSS.a = gpm4;
  myFSS.g = dps2000;     
  result = setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS); if (result > worstResult) worstResult = result;

  result = enableDLPF(ICM_20948_Internal_Gyr, true); if (result > worstResult) worstResult = result;

  result = setBank(0); if (result > worstResult) worstResult = result; 
  uint8_t zero = 0;
  result = write(AGB0_REG_FIFO_EN_1, &zero, 1); if (result > worstResult) worstResult = result;

  result = write(AGB0_REG_FIFO_EN_2, &zero, 1); if (result > worstResult) worstResult = result;

  result = intEnableRawDataReady(false); if (result > worstResult) worstResult = result;

  result = resetFIFO(); if (result > worstResult) worstResult = result;

  ICM_20948_smplrt_t mySmplrt;
  mySmplrt.g = 10;
  mySmplrt.a = 10;
  
  result = setSampleRate((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), mySmplrt); if (result > worstResult) worstResult = result;

  result = setDMPstartAddress(); if (result > worstResult) worstResult = result;

  result = loadDMPFirmware(); if (result > worstResult) worstResult = result;

  result = setDMPstartAddress(); if (result > worstResult) worstResult = result;

  result = setBank(0); if (result > worstResult) worstResult = result;
  uint8_t fix = 0x48;
  result = write(AGB0_REG_HW_FIX_DISABLE, &fix, 1); if (result > worstResult) worstResult = result;

  result = setBank(0); if (result > worstResult) worstResult = result;
  uint8_t fifoPrio = 0xE4;
  result = write(AGB0_REG_SINGLE_FIFO_PRIORITY_SEL, &fifoPrio, 1); if (result > worstResult) worstResult = result;

  const unsigned char accScale[4] = {0x04, 0x00, 0x00, 0x00};
  result = writeDMPmems(ACC_SCALE, 4, &accScale[0]); if (result > worstResult) worstResult = result;

  const unsigned char accScale2[4] = {0x00, 0x04, 0x00, 0x00};
  result = writeDMPmems(ACC_SCALE2, 4, &accScale2[0]); if (result > worstResult) worstResult = result;

  const unsigned char mountMultiplierZero[4] = {0x00, 0x00, 0x00, 0x00};
  const unsigned char mountMultiplierPlus[4] = {0x09, 0x99, 0x99, 0x99};
  const unsigned char mountMultiplierMinus[4] = {0xF6, 0x66, 0x66, 0x67};
  result = writeDMPmems(CPASS_MTX_00, 4, &mountMultiplierPlus[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_01, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_02, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_10, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_11, 4, &mountMultiplierMinus[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_12, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_20, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_21, 4, &mountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(CPASS_MTX_22, 4, &mountMultiplierMinus[0]); if (result > worstResult) worstResult = result;

  const unsigned char b2sMountMultiplierZero[4] = {0x00, 0x00, 0x00, 0x00};
  const unsigned char b2sMountMultiplierPlus[4] = {0x40, 0x00, 0x00, 0x00};
  result = writeDMPmems(B2S_MTX_00, 4, &b2sMountMultiplierPlus[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_01, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_02, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_10, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_11, 4, &b2sMountMultiplierPlus[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_12, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_20, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_21, 4, &b2sMountMultiplierZero[0]); if (result > worstResult) worstResult = result;
  result = writeDMPmems(B2S_MTX_22, 4, &b2sMountMultiplierPlus[0]); if (result > worstResult) worstResult = result;

  result = setGyroSF(4, 3); if (result > worstResult) worstResult = result;

  const unsigned char gyroFullScale[4] = {0x10, 0x00, 0x00, 0x00};
  result = writeDMPmems(GYRO_FULLSCALE, 4, &gyroFullScale[0]); if (result > worstResult) worstResult = result;

  const unsigned char accelOnlyGain[4] = {0x00, 0xE8, 0xBA, 0x2E};
  result = writeDMPmems(ACCEL_ONLY_GAIN, 4, &accelOnlyGain[0]); if (result > worstResult) worstResult = result;

  const unsigned char accelAlphaVar[4] = {0x3D, 0x27, 0xD2, 0x7D};
  result = writeDMPmems(ACCEL_ALPHA_VAR, 4, &accelAlphaVar[0]); if (result > worstResult) worstResult = result;

  const unsigned char accelAVar[4] = {0x02, 0xD8, 0x2D, 0x83};
  result = writeDMPmems(ACCEL_A_VAR, 4, &accelAVar[0]); if (result > worstResult) worstResult = result;

  const unsigned char accelCalRate[4] = {0x00, 0x00};
  result = writeDMPmems(ACCEL_CAL_RATE, 2, &accelCalRate[0]); if (result > worstResult) worstResult = result;

  const unsigned char compassRate[2] = {0x00, 0x45};
  result = writeDMPmems(CPASS_TIME_BUFFER, 2, &compassRate[0]); if (result > worstResult) worstResult = result;

  return worstResult;
}