/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include "MyTypes.h"
#include "Storage.h"
#include "WDT.h"

extern void PerSecond_Handler();

void Setup() {
    Storage_Init();
    RegisterInterface = (interface_t *)  Storage_Read();
    
    // set the uptime to zero. This value is also stored and should be reset. 
    RegisterInterface->Datastore.SystemUptimeTimer = 0;
    // reset Warning and Error registers.
    RegisterInterface->System.Warning = 0x0000;
    RegisterInterface->System.Error = 0x0000;
    
    
    // start the RTC.
    RTC_Start();
    RTC_SetUnixTime(RegisterInterface->Datastore.RTC_UnixTime);
    RTC_SetPeriod(1,1);
    
    // Config state check
    if ( RegisterInterface->System.ConfigState == 0 ) {
         // clear memory
        memset(&holdingReg[0], 0, sizeof(holdingReg));
        
        RegisterInterface->System.ConfigState = 0x7F;
        RegisterInterface->System.StorageController = 0x01; // Set a request for the Storage controller 
        RegisterInterface->System.Address = 30;
        RegisterInterface->System.ActAsDriver = 1;      // Setup the UART so the TX is in pull up mode.
        RegisterInterface->System.Version = 0x0200 + 6; // High byte: Major realease, low byte: minor release.
        RegisterInterface->System.UseWatchDog = 0x00; //        
        coils[0] = 0x01;
        
        RegisterInterface->Datastore.Analog_Samples = 8;
        RegisterInterface->Datastore.Analog_Slope[0] = 1221; // 1221 uV is default Full Range input 0..5000 mVolt.
        RegisterInterface->Datastore.Analog_Slope[1] = 1221; // 1221 uV is default Full Range input 0..5000 mVolt.
        RegisterInterface->Datastore.Analog_Slope[2] = 1221; // 1221 uV is default Full Range input 0..5000 mVolt.
        RegisterInterface->Datastore.Analog_Slope[3] = 1221; // 1221 uV is default Full Range input 0..5000 mVolt.
        
        // defaults
        RegisterInterface->Datastore.Digital_Out_0 = 1;
        RegisterInterface->Datastore.Digital_Out_1 = 1;
        RegisterInterface->Datastore.Digital_Out_2 = 1;
        RegisterInterface->Datastore.Digital_Out_3 = 1;
        RegisterInterface->Datastore.Digital_Out_4 = 1;
        RegisterInterface->Datastore.Digital_Out_5 = 1;
        RegisterInterface->Datastore.Digital_Out_6 = 1;
        RegisterInterface->Datastore.Digital_Out_7 = 1;
        
        RegisterInterface->Datastore.HumidityComp[0] = 12;
        RegisterInterface->Datastore.HumidityComp[1] = 27;
        RegisterInterface->Datastore.HumidityComp[2] = -138;
        RegisterInterface->Datastore.HumidityComp[3] = -50;
        
        RegisterInterface->Datastore.TemperatureComp[0] = 1;
        RegisterInterface->Datastore.TemperatureComp[1] = 0;
        RegisterInterface->Datastore.TemperatureComp[2] = -4;
        RegisterInterface->Datastore.TemperatureComp[3] = 0;
        
        
        // automatic mode
        RegisterInterface->System.AirTempMax            = (2<<8)|(30);      // 2 C hysteresis and 30 C threshold.
        RegisterInterface->System.AirTempMin            = (2<<8)|(15);      // 2 C hysteresis and 15 C threshold.
        RegisterInterface->System.HumidityDevation      = (3<<8)|(6);       // 3 % hysteresis and 6% difference threshold.
        RegisterInterface->System.AutomaticOverride     = 0x0700;           // Allow All Overrides 0x0100 + 0x0200 + 0x0400 (Climate, Temp and Humidity.)
        RegisterInterface->System.ThermostatSetPoint    = (5<<8) | (175);   // in 10th of C Degrees. 0.5C Hyst. 17.5C Trigger.
        RegisterInterface->System.HighAirFlowLong       = 3600 * 8;         // 8 hours of increased airflow   ( Guests mode )
        RegisterInterface->System.HighAirFlowMedium     = 3600;             // 1 hour of increased airflow    ( refresh mode )
        RegisterInterface->System.HighAirFlowShort      = 300;              // 5 minutes of increased airflow ( toilet mode )
        
    }
    
    // System preperations
    // UART 
        // per default start in tx res pull up mode.
        ModbusUART_tx_SetDriveMode(ModbusUART_tx_DM_RES_UP);
        ModbusUART_tx_Write(0xFF);
        //ModbusUART_rx_SetDriveMode(ModbusUART_rx_DM_RES_UP);
        //ModbusUART_rx_Write(0xFF);
        
        // if the controller is not in line driver mode change back.
        if ( RegisterInterface->System.ActAsDriver == 0) {
            ModbusUART_rx_SetDriveMode(ModbusUART_rx_DM_DIG_HIZ);
            ModbusUART_rx_Write(0xFF);
            ModbusUART_tx_SetDriveMode(ModbusUART_tx_DM_OD_LO);
            ModbusUART_tx_Write(0xFF);
        }
        
        Modbus_Init();
   
    // Input Iterrupt
      //  Input_IRQ_StartEx(&Input_Handler);
           
    // ADC setup    
        ADC_Start();
    // ADC end setup    
        
   
    // UID  64 Bit (uint32 *) 
        CyGetUniqueId((uint32 *) &RegisterInterface->System.UID); // test for it.. Might be the best source for the Random generator. 
    
    // Modbus Hardware setup
        ModbusUART_Start();
        Modbus_Config.SlaveAddress = &RegisterInterface->System.Address;
        Modbus_Config.Uart_GetByte = &ModbusUART_UartGetChar;
        Modbus_Config.Uart_PutByte = &ModbusUART_SpiUartWriteTxData;
        Modbus_Config.StoreWorker  = &Storage_Write;
        MessageReceived_StartEx(messageReceived_isr);  

    // Timer
        SlowClock_Start();
        PerSecondInterrupt_StartEx(&PerSecond_Handler);
    
    // DHT22
        DHT22[0].ReadPin   = &DHT22_0_Read;
        DHT22[0].WritePin  = &DHT22_0_Write;
        DHT22[1].ReadPin   = &DHT22_1_Read;
        DHT22[1].WritePin  = &DHT22_1_Write;
        DHT22[2].ReadPin   = &DHT22_2_Read;
        DHT22[2].WritePin  = &DHT22_2_Write;
        DHT22[3].ReadPin   = &DHT22_3_Read;
        DHT22[3].WritePin  = &DHT22_3_Write;
         
    // Watchdog counter
       if (RegisterInterface->System.UseWatchDog ) WDT_Start();
}


/* [] END OF FILE */
