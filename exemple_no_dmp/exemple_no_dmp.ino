// OLA Specifics:
const byte PIN_IMU_POWER = 27; // The Red SparkFun version of the OLA (V10) uses pin 27
const byte PIN_IMU_INT = 37;
const byte PIN_IMU_CHIP_SELECT = 12;
const byte PIN_PWR_LED = 29;
const byte PIN_STAT_LED = 19;
const byte PIN_TRIGGER = 11;
const byte PIN_SPI_SCK = 5;
const byte PIN_SPI_CIPO = 6;
const byte PIN_SPI_COPI = 7;

volatile uint16_t count = 0;

uint32_t baudrate = 115200;

#include "ICM_20948.h"  // Click here to get the library: http://librarymanager/All#SparkFun_ICM_20948_IMU

#define SERIAL_PORT Serial

#define SPI_PORT SPI    // Your desired SPI port.       Used only when "USE_SPI" is defined
#define CS_PIN PIN_IMU_CHIP_SELECT // Which pin you connect CS to. Used only when "USE_SPI" is defined. OLA uses pin 44.

ICM_20948_SPI myICM;  // If using SPI create an ICM_20948_SPI object

// Some vars to control or respond to interrupts
volatile bool isrFired = false;
volatile bool sensorSleep = false;

void setup() {
  
  powerLEDOn(); // Turn the power LED on - if the hardware supports it
  pinMode(PIN_STAT_LED, OUTPUT);
  digitalWrite(PIN_STAT_LED, HIGH);

  SERIAL_PORT.begin(baudrate); //Default for initial debug messages if necessary
  SERIAL_PORT.println("OLA Fast IMU Test");
  
  SPI_PORT.begin();
  
  pinMode(PIN_IMU_CHIP_SELECT, OUTPUT);
  digitalWrite(PIN_IMU_CHIP_SELECT, HIGH); //Be sure IMU is deselected

  enableCIPOpullUp(); // Enable CIPO pull-up on the OLA

  pinMode(PIN_IMU_INT, INPUT_PULLUP);   // Using a pullup b/c ICM-20948 Breakout board has an onboard pullup as well and we don't want them to compete
  attachInterrupt(digitalPinToInterrupt(PIN_IMU_INT), icmISR, FALLING); // Set up a falling interrupt

  //Reset ICM by power cycling it
  imuPowerOff();
  
  delay(10);
  
  imuPowerOn(); // Enable power for the OLA IMU
  
  delay(100); // Wait for the IMU to power up

  bool initialized = false;
  while( !initialized ){

  myICM.begin( CS_PIN, SPI_PORT ); 
    
  if( myICM.status != ICM_20948_Stat_Ok ){
  SERIAL_PORT.println( "Trying again..." );
  delay(500);
  }else{
   initialized = true;
  }
 }
 
  digitalWrite(PIN_STAT_LED, LOW); // Turn the STAT LED off now that everything is configured
  
  // Here we are doing a SW reset to make sure the device starts in a known state
  myICM.swReset( );
  if( myICM.status != ICM_20948_Stat_Ok){
    SERIAL_PORT.print(F("Software Reset returned: "));
    SERIAL_PORT.println(myICM.statusString());
  }
  delay(250);

  // Now wake the sensor up
  myICM.sleep( sensorSleep );
  myICM.lowPower( false );

  // The next few configuration functions accept a bit-mask of sensors for which the settings should be applied.

  // Set Gyro and Accelerometer to a particular sample mode
  // options: ICM_20948_Sample_Mode_Continuous
  //          ICM_20948_Sample_Mode_Cycled
  myICM.setSampleMode( (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), ICM_20948_Sample_Mode_Continuous);
  
  ICM_20948_smplrt_t mySmplrt;
  mySmplrt.a = 1;  // specify the accel sample rate to maximum: see Table 19 in datasheet DS-000189-ICM-20948-v1.3.pdf - as
  mySmplrt.g = 1;  // specify the gyro sample rate to maximum: see Table 17 in datasheet DS-000189-ICM-20948-v1.3.pdf  - as 
  myICM.setSampleRate( ICM_20948_Internal_Acc, mySmplrt );
  myICM.setSampleRate( ICM_20948_Internal_Gyr, mySmplrt );

  ICM_20948_fss_t myFSS;
  myFSS.a = gpm2;   // (ICM_20948_ACCEL_CONFIG_FS_SEL_e)
  myFSS.g = dps250; // (ICM_20948_GYRO_CONFIG_1_FS_SEL_e)
  myICM.setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS);

  ICM_20948_dlpcfg_t myDLPcfg; 
  myDLPcfg.a = acc_d473bw_n499bw;
  myDLPcfg.g = gyr_d361bw4_n376bw5;
  myICM.setDLPFcfg( (ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myDLPcfg );

  // Choose whether or not to use DLPF
  // Here we're also showing another way to access the status values, and that it is OK to supply individual sensor masks to these functions
  ICM_20948_Status_e accDLPEnableStat = myICM.enableDLPF( ICM_20948_Internal_Acc, true );
  ICM_20948_Status_e gyrDLPEnableStat = myICM.enableDLPF( ICM_20948_Internal_Gyr, true );

  
  myICM.cfgIntActiveLow(true);                      // Active low to be compatible with the breakout board's pullup resistor
  myICM.cfgIntOpenDrain(false);                     // Push-pull, though open-drain would also work thanks to the pull-up resistors on the breakout
  myICM.cfgIntLatch(false);                         // Don't latch the interrupt - previously this was set to true (Latch the interrupt until cleared)
  myICM.intEnableRawDataReady(true);                // enable interrupts on raw data ready

}


void loop() {
  
  //  Check for a trigger event
  if( isrFired ){                       // If our isr flag is set then clear the interrupts on the ICM
    isrFired = false;
    myICM.getAGMT(); //Update values
    count++;
  }
  
  if( count >= 1000){
    count = 0;
    SERIAL_PORT.println(); // Use Serial Monitor Timestamp to see the interval for 1000 events
  }
  
}

#if defined(ARDUINO_ARCH_MBED) // updated for v2.1.0 of the Apollo3 core
bool enableCIPOpullUp()
{
  //Add 1K5 pull-up on CIPO
  am_hal_gpio_pincfg_t cipoPinCfg = g_AM_BSP_GPIO_IOM0_MISO;
  cipoPinCfg.ePullup = AM_HAL_GPIO_PIN_PULLUP_1_5K;
  pin_config(PinName(PIN_SPI_CIPO), cipoPinCfg);
  return (true);
}
#else
bool enableCIPOpullUp()
{
  //Add CIPO pull-up
  ap3_err_t retval = AP3_OK;
  am_hal_gpio_pincfg_t cipoPinCfg = AP3_GPIO_DEFAULT_PINCFG;
  cipoPinCfg.uFuncSel = AM_HAL_PIN_6_M0MISO;
  cipoPinCfg.ePullup = AM_HAL_GPIO_PIN_PULLUP_1_5K;
  cipoPinCfg.eDriveStrength = AM_HAL_GPIO_PIN_DRIVESTRENGTH_12MA;
  cipoPinCfg.eGPOutcfg = AM_HAL_GPIO_PIN_OUTCFG_PUSHPULL;
  cipoPinCfg.uIOMnum = AP3_SPI_IOM;
  padMode(MISO, cipoPinCfg, &retval);
  return (retval == AP3_OK);
}
#endif

//Trigger Pin ISR
void icmISR( void ){
  isrFired = true; 
}