/* ========================================
 *
 * Copyright Matijs Behrens, 2021
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Matijs Behrens.
 *
 * ========================================
All the used types 

*/

#include "project.h"
#include "DHT22.h"
#include "Modbus.h"

#ifndef _MY_TYPES_H
    #define _MY_TYPES_H

    #define _WARN_DHT22_0_INVALID   0x0001
    #define _WARN_DHT22_1_INVALID   0x0002
    #define _WARN_DHT22_2_INVALID   0x0004
    #define _WARN_DHT22_3_INVALID   0x0008
    
    #define _ERR_FLASH_INIT         0x0001
    #define _ERR_FLASH_READ         0x0002
    #define _ERR_FLASH_WRITE        0x0004
    

typedef struct interface_s {
    // 0 
    struct System_s {
        // 0
        uint16 ConfigState; // 0: set to factory default,  0x7F: System ready, masked with 0x80 -> Error state
        // 1
        uint16 StorageController; // 1: Write to Flash, 2: Ready 0x80: Error.
        // 2
        uint8 Address;
        uint8 ActAsDriver; // Activate pullup resistor in TX port. 
        // 3
        uint16 Version;
        // 4
        uint16 UseWatchDog; 
        // 5+6+7+8 +9+10+11 
        uint64 UID;
        
        //14
        uint16_t DieTemperature;     // @12
        uint8_t HeatingState;        // @13
        uint8_t CirculationState;    // @13
        uint8_t reseved_6;           // @14
        uint8_t HumidityState;       // @14    
        uint16_t AirTempMax;         // @15 maximum temp. if higher close valve
        uint16_t AirTempMin;         // @16 minimum temp. if lower close valve.
        uint16_t HumidityDevation;   // @17 minimum humidity diff  
        uint16_t AutomaticOverride;  // @18 0x0001: Automatic Valve program Active 0x0100:allowed 0x0002: Automatic Temperature control program active: 0x0200:Allowed 0x8000:Auto=active.
        uint16_t ThermostatSetPoint; // @19 If Automatic thermostat is used.
        uint32_t HighAirFlowTimer;   // @20 Set the auto cancel timestamp for High airflow override
        uint16_t HighAirFlowShort;   // @21 Duration of short increased flow override
        uint16_t HighAirFlowMedium;  // @22 Duration of medium increased airflow override
        uint16_t HighAirFlowLong;    // @23 Duration of long increased airflow override
        uint16_t reserved_12;
        uint16_t reserved_13;
        uint16_t reserved_14;
        uint16_t reserved_15;
        uint16_t reserved_16;
        uint16_t reserved_17;
        uint16_t reserved_18;
        uint8_t Warning;
        uint8_t Error;
    } System;
    // 48
    struct Store_s {
        // 36 + 37
        uint32 reserved_19;
        // 38 + 39
        uint32 SystemUptimeTimer;       
        // 40
        uint16 Humidity[4];
        // 44
        uint16 Temperature[4];

        // 48
        uint16 Analog_Samples;
        // 49
        uint16 Analog_Slope[4];
        // 53
        uint16 Analog_Input[4];
        
        // 57
        uint16 Digital_In_0;
        uint16 Digital_In_1;
        uint16 Digital_In_2;
        uint16 Digital_In_3;
        uint16 Digital_In_4;
        uint16 Digital_In_5;
        uint16 Digital_In_6;
        uint16 Digital_In_7;
        
        // 65
        uint16 Digital_Out_0;
        uint16 Digital_Out_1;
        uint16 Digital_Out_2;
        uint16 Digital_Out_3;
        uint16 Digital_Out_4;
        uint16 Digital_Out_5;
        uint16 Digital_Out_6;
        uint16 Digital_Out_7;
        
        // 73,74,75,76
        int16 HumidityComp[4];
        // 77,78,79,80
        int16 TemperatureComp[4];
        uint64 RTC_UnixTime;
    } Datastore;
    
  
} interface_t;


extern unsigned short holdingReg[100]; 
extern unsigned char coils[16];
extern unsigned char received[125];
extern unsigned char response[125]; 
extern uint8 modbusMessage;

extern interface_t * RegisterInterface;
extern struct DHT22_Param_s DHT22[4];  
extern const uint8_t eepromArray[EEPROM_PHYSICAL_SIZE] __ALIGNED(CY_FLASH_SIZEOF_ROW);

#endif 

/* [] END OF FILE */
