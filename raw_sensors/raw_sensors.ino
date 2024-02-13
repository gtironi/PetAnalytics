#include "ICM_20948.h"

#define USE_SPI
#define SERIAL_PORT Serial
#define SPI_PORT SPI
#define CS_PIN 12

ICM_20948_SPI myICM;

void setup(){
    // configurando a comunicação serial
    SERIAL_PORT.begin(115200);
    delay(100);

    // aguarda a limpeza da porta serial
    while (SERIAL_PORT.available())
        SERIAL_PORT.read();

    // aguarda o comando do usuário para continuar (acredito ser desnecessário)
    SERIAL_PORT.println(F("Press any key to continue..."));
    while (!SERIAL_PORT.available())
    ;

    // inicializando o SPI
    SPI_PORT.begin();

    // caso seja necessário, comentar a linha seguinte vai destivar a ajuda da biblioteca para debugar o código
    myICM.enableDebugging(); 

    // loop de inicialização do IMU
    bool initialized = false;
    while (!initialized){
        // inicializa a comunicação com o sensor via SPI
        myICM.begin(CS_PIN, SPI_PORT);

        // imprime o status/resultado da tentativa de comunicação
        SERIAL_PORT.print(F("Initialization of the sensor returned: "));
        SERIAL_PORT.println(myICM.statusString());

        // se der errado, vai tentar novamente, caso contrário, a comunicação estará pronta
        if (myICM.status != ICM_20948_Stat_Ok){
            SERIAL_PORT.println(F("Trying again..."));
            delay(500);
        } else{
            initialized = true;
        }
    }

    // mensagem de sucesso
    SERIAL_PORT.println(F("Device connected!"));

    // variável que vai indicar se as configurações foram bem sucedidas
    bool success = true;

    // vai inicializar o DMP e retornar o status/resultado
    success &= (myICM.initializeDMP() == ICM_20948_Stat_Ok);

    // ativação
    success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_GYROSCOPE) == ICM_20948_Stat_Ok);
    success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_RAW_ACCELEROMETER) == ICM_20948_Stat_Ok);
    success &= (myICM.enableDMPSensor(INV_ICM20948_SENSOR_MAGNETIC_FIELD_UNCALIBRATED) == ICM_20948_Stat_Ok);
    
    // ajustando a taxa do DMP      
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Accel, 1) == ICM_20948_Stat_Ok);        
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro, 1) == ICM_20948_Stat_Ok);         
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Gyro_Calibr, 1) == ICM_20948_Stat_Ok);  
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Cpass, 1) == ICM_20948_Stat_Ok);        
    success &= (myICM.setDMPODRrate(DMP_ODR_Reg_Cpass_Calibr, 1) == ICM_20948_Stat_Ok);       


    // habilitando o FIFO 
    success &= (myICM.enableFIFO() == ICM_20948_Stat_Ok);

    // habilitando o DMP
    success &= (myICM.enableDMP() == ICM_20948_Stat_Ok);

    // resetando o FIFO e o DMP
    success &= (myICM.resetDMP() == ICM_20948_Stat_Ok);
    success &= (myICM.resetFIFO() == ICM_20948_Stat_Ok);

    // como todas as configurações foram vinculadas à variável 'success', caso tudo tenha dado certo 
    // o código seguirá em frente e uma mensagem de sucesso será printada, caso contrário, uma mensagem de fracasso
    if (success){
        SERIAL_PORT.println(F("DMP enabled!"));
    } else{
        SERIAL_PORT.println(F("Enable DMP failed!"));
        while (1);
    }

    std::chrono::steady_clock::time_point lastInsertTime;
    std::chrono::steady_clock::time_point currentTime;
}

void loop(){
    // declaração de uma variável que vai armazenar dados do DMP
    icm_20948_DMP_data_t data;

    // vai ler dados da DMP da fifo e armazenar na variável 'data'
    myICM.readDMPdataFromFIFO(&data);

    // verifica se existem dados válidos disponíveis
    if ((myICM.status == ICM_20948_Stat_Ok) || (myICM.status == ICM_20948_Stat_FIFOMoreDataAvail)){

    if ((data.header & DMP_header_bitmap_Accel) > 0) // Check for Accel
    {
      float acc_x = (float)data.Raw_Accel.Data.X; // Extract the raw accelerometer data
      float acc_y = (float)data.Raw_Accel.Data.Y;
      float acc_z = (float)data.Raw_Accel.Data.Z;

      SERIAL_PORT.print(F("Accel: X:"));
      SERIAL_PORT.print(acc_x);
      SERIAL_PORT.print(F(" Y:"));
      SERIAL_PORT.print(acc_y);
      SERIAL_PORT.print(F(" Z:"));
      SERIAL_PORT.println(acc_z);
    }

    if ((data.header & DMP_header_bitmap_Gyro) > 0) // Check for Gyro
    {
      float x = (float)data.Raw_Gyro.Data.X; // Extract the raw gyro data
      float y = (float)data.Raw_Gyro.Data.Y;
      float z = (float)data.Raw_Gyro.Data.Z;

      SERIAL_PORT.print(F("Gyro: X:"));
      SERIAL_PORT.print(x);
      SERIAL_PORT.print(F(" Y:"));
      SERIAL_PORT.print(y);
      SERIAL_PORT.print(F(" Z:"));
      SERIAL_PORT.println(z);
    }

    if ((data.header & DMP_header_bitmap_Compass) > 0) // Check for Compass
    {
      float x = (float)data.Compass.Data.X; // Extract the compass data
      float y = (float)data.Compass.Data.Y;
      float z = (float)data.Compass.Data.Z;

      SERIAL_PORT.print(F("Compass: X:"));
      SERIAL_PORT.print(x);
      SERIAL_PORT.print(F(" Y:"));
      SERIAL_PORT.print(y);
      SERIAL_PORT.print(F(" Z:"));
      SERIAL_PORT.println(z);
    }
    }

    // verifica se existem mais dados disponíveis na FIFO, caso contrário ele aguarda 1 ms e o loop é executado novamente
    if (myICM.status != ICM_20948_Stat_FIFOMoreDataAvail){
        delay(1);
    }
}

// o exemplo considera a função padrão da biblioteca insuficiente e por isso vai sobrescrevê-la
ICM_20948_Status_e ICM_20948::initializeDMP(void){
    
    // variáveis que serão usadas ao longo do código para indicar sucesso/fracasso de alguma configuração
    ICM_20948_Status_e  result = ICM_20948_Stat_Ok;
    ICM_20948_Status_e  worstResult = ICM_20948_Stat_Ok;

    // --------------------------------------------------------------------------------------------------------------------------------------------------- //

    // NÃO ENTENDI MUITO BEM O QUE FOI FEITO AQUI, TENHO QUE VOLTAR PARA TENTAR ENTENDER E COMENTAR
    // MAS É IMPORTANTE LEMBRAR QUE ESSAS CONFIGURAÇÕES TRATAM DO PROTOCOLO I2C E PORTANTO PODE NÃO SER TÃO RELEVANTE PARA O PROJETO
    // MAS PARA EVITAR COMPLICAÇÕES, AINDA VOU DEIXÁ-LAS NO CÓDIGO
    result = i2cControllerConfigurePeripheral(0, MAG_AK09916_I2C_ADDR, AK09916_REG_RSV2, 10, true, true, false, true, true); if (result > worstResult) worstResult = result;
    result = i2cControllerConfigurePeripheral(1, MAG_AK09916_I2C_ADDR, AK09916_REG_CNTL2, 1, false, true, false, false, false, AK09916_mode_single); if (result > worstResult) worstResult = result;

    result = setBank(3); if (result > worstResult) worstResult = result;
    uint8_t mstODRconfig = 0x04;
    result = write(AGB3_REG_I2C_MST_ODR_CONFIG, &mstODRconfig, 1); if (result > worstResult) worstResult = result;

    // --------------------------------------------------------------------------------------------------------------------------------------------------- //

    // vai selecionar a melhor fonte de clock disponível - PPL (Phase-Locked Loop) caso esteja possível, ou o ocilador interno
    result = setClockSource(ICM_20948_Clock_Auto); if (result > worstResult) worstResult = result;

    // está dizendo ao sensor para ativar os sensores de aceleração e giroscópio
    result = setBank(0); if (result > worstResult) worstResult = result;
    uint8_t pwrMgmt2 = 0x40;
    result = write(AGB0_REG_PWR_MGMT_2, &pwrMgmt2, 1); if (result > worstResult) worstResult = result; 

    // colocando o mestre I2C (I2C_Master) em um modo de baixo consumo de energia cíclico
    result = setSampleMode(ICM_20948_Internal_Mst, ICM_20948_Sample_Mode_Cycled); if (result > worstResult) worstResult = result;

    // vai desabilitar o FIFO e o DMP por padrão, isso pode ser alterado de forma externa como foi feito no código acima
    result = enableFIFO(false); if (result > worstResult) worstResult = result;
    result = enableDMP(false); if (result > worstResult) worstResult = result;

    // vai configurar o FSR do acelerômetro e do giroscópio com valores padrões
    // esses valores também podem ser alterados de forma externa posteriormente
    ICM_20948_fss_t myFSS;
    myFSS.a = gpm4; // 4g
    myFSS.g = dps2000; // 2000 graus por segundo
    result = setFullScale((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), myFSS); if (result > worstResult) worstResult = result;

    // como o almir disse que o DLPF era desnecessário, não vou ativar o DLPF, ainda mais que ela é adicionada no código original por precaução
    // basta mudar o parâmetro para 'true' para alterar a configuração
    result = enableDLPF(ICM_20948_Internal_Gyr, false); if (result > worstResult) worstResult = result;

    /*
    VOU COMENTAR ESSA LINHA, VISTO QUE TROCANDO UMA IDEIA COM O CHAT GPT ELE ME DISSE QUE ISSO PODE 
    IMPEDIR QUE O ACELERÔMETRO E O GIROSCÓPIO DE REGISTRAR SEUS DADOS NA FIFO, O PROBLEMA É QUE NO
    CÓDIGO PRINCIPAL EXTRAÍMOS OS DADOS DA FIFO, CASO DÊ ALGUM ERRO, PODEMOS DESCOMENTAR ESSE TRECHO

    result = setBank(0); if (result > worstResult) worstResult = result;
    uint8_t zero = 0;
    result = write(AGB0_REG_FIFO_EN_1, &zero, 1); if (result > worstResult) worstResult = result;

    result = write(AGB0_REG_FIFO_EN_2, &zero, 1); if (result > worstResult) worstResult = result;
    */

    // como estamos lendo os dados com uma frequência regular, isso pode economizar recursos, visto que essa interrupção indica quando dados novos estão prontos
    result = intEnableRawDataReady(false); if (result > worstResult) worstResult = result;

    // vai limpar a fifo assim que o DMP for inicializado
    result = resetFIFO(); if (result > worstResult) worstResult = result;

    // estamos configuração as taxas de amostragem padrões do acelerômetro e do giroscópio para 112 hz ao mudar o parâmetro para 8, e não 4 como tá no exemplo 
    ICM_20948_smplrt_t mySmplrt;
    mySmplrt.g = 4;
    mySmplrt.a = 4;
    result = setSampleRate((ICM_20948_Internal_Acc | ICM_20948_Internal_Gyr), mySmplrt); if (result > worstResult) worstResult = result;
    
    // configurando o endereço inicial do DMP
    result = setDMPstartAddress(); if (result > worstResult) worstResult = result;

    // carregando o firmware no DMP (firmware é o software que será executado no DMP e é responsável pelo processamento dos dados dos sensores e pela execução de algoritmos específicos)
    result = loadDMPFirmware(); if (result > worstResult) worstResult = result;

    // configura o endereço do DMP garantindo que o carregamento do firmware ocorra no local correto da memória
    result = setDMPstartAddress(); if (result > worstResult) worstResult = result;

    // NÃO SEI AO CERTO O QUE ESTÁ ACONTECENDO AQUI E POR ISSO NÃO VOU ARRISCAR DE MEXER
    result = setBank(0); if (result > worstResult) worstResult = result;
    uint8_t fix = 0x48;
    result = write(AGB0_REG_HW_FIX_DISABLE, &fix, 1); if (result > worstResult) worstResult = result;

    // configurando o registro "Single FIFO Priority Select"
    result = setBank(0); if (result > worstResult) worstResult = result;
    uint8_t fifoPrio = 0xE4;
    result = write(AGB0_REG_SINGLE_FIFO_PRIORITY_SEL, &fifoPrio, 1); if (result > worstResult) worstResult = result;

    // a escala do acelerômetro no Digital Motion Processor (DMP) é configurada para 4g
    // ajustando a escala para garantir a precisão e faixa de medição desejadas
    const unsigned char accScale[4] = {0x04, 0x00, 0x00, 0x00};
    result = writeDMPmems(ACC_SCALE, 4, &accScale[0]); if (result > worstResult) worstResult = result;
    const unsigned char accScale2[4] = {0x00, 0x04, 0x00, 0x00};
    result = writeDMPmems(ACC_SCALE2, 4, &accScale2[0]); if (result > worstResult) worstResult = result;

    // configura a matriz de montagem e escala do magnetômetro no DMP, ajustando os coeficientes para 
    // alinhar corretamente os eixos do magnetômetro com os do acelerômetro/gyro
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

    // configura a matriz de montagem B2S no DMP, ajustando os coeficientes para alinhar corretamente os eixos do sensor com o corpo
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

    // configurar o fator de escala do giroscópio no Digital Motion Processor (DMP)
    result = setGyroSF(4, 3); if (result > worstResult) worstResult = result; // 4 = 225Hz (see above), 3 = 2000dps (see above)
    
    // configurando a FS do giroscópio 
    const unsigned char gyroFullScale[4] = {0x10, 0x00, 0x00, 0x00}; // 2000dps : 2^28
    result = writeDMPmems(GYRO_FULLSCALE, 4, &gyroFullScale[0]); if (result > worstResult) worstResult = result;

    // configura o ganho do acelerômetro (PERGUNTAR PARA O ALMIR DEPOIS QUAL DAS DUAS LINHAS EU DEVO USAR)
    const unsigned char accelOnlyGain[4] = {0x00, 0xE8, 0xBA, 0x2E}; // 225Hz
    //const unsigned char accelOnlyGain[4] = {0x01, 0xD1, 0x74, 0x5D}; // 112Hz
    result = writeDMPmems(ACCEL_ONLY_GAIN, 4, &accelOnlyGain[0]); if (result > worstResult) worstResult = result;

    // configura a variância alpha do acelerômetro (PERGUNTAR PARA O ALMIR DEPOIS QUAL DAS DUAS LINHAS EU DEVO USAR)
    const unsigned char accelAlphaVar[4] = {0x3D, 0x27, 0xD2, 0x7D}; // 225Hz
    //const unsigned char accelAlphaVar[4] = {0x3A, 0x49, 0x24, 0x92}; // 112Hz
    result = writeDMPmems(ACCEL_ALPHA_VAR, 4, &accelAlphaVar[0]); if (result > worstResult) worstResult = result;

    // (PERGUNTAR PARA O ALMIR DEPOIS QUAL DAS DUAS LINHAS EU DEVO USAR)
    const unsigned char accelAVar[4] = {0x02, 0xD8, 0x2D, 0x83}; // 225Hz
    //const unsigned char accelAVar[4] = {0x05, 0xB6, 0xDB, 0x6E}; // 112Hz
    result = writeDMPmems(ACCEL_A_VAR, 4, &accelAVar[0]); if (result > worstResult) worstResult = result;

    // vai configurar a taxa de calibração do acelerômetro
    const unsigned char accelCalRate[4] = {0x00, 0x00};
    result = writeDMPmems(ACCEL_CAL_RATE, 2, &accelCalRate[0]); if (result > worstResult) worstResult = result;

    // configura o tempo do buffer do magnetômetro no DMP
    const unsigned char compassRate[2] = {0x00, 0x45}; // 69Hz
    result = writeDMPmems(CPASS_TIME_BUFFER, 2, &compassRate[0]); if (result > worstResult) worstResult = result;

    // Enable DMP interrupt
    // This would be the most efficient way of getting the DMP data, instead of polling the FIFO
    //result = intEnableDMP(true); if (result > worstResult) worstResult = result;

    return worstResult;
}