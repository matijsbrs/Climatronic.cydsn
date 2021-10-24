/* ========================================
 *
 * Copyright Matijs Behrens, 2021
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Matijs Behrens.
 *
 * The workers file contains 'Workers'
 * A Worker is a function which is periodically called. 
 * This clears up the main route. And improves the readability of the main program.

* ========================================
*/
#include "MyTypes.h"
#include "Storage.h"
#include "Workers.h"

uint16_t ADC_Assistent_MeasureTemp() {
	uint16_t tempLvl = 0;
	uint32_t temp = 0;
    
    ADC_StopConvert();
	ADC_EnableInjection();
	ADC_StartConvert();
    // actively wait for the INJ channel to be ready.
	while(!ADC_IsEndConversion(ADC_WAIT_FOR_RESULT_INJ)){
        CyDelayUs(1);
    }
    
    // compensate for the 5000 mV reference voltage
    temp = ADC_GetResult16(4)*5000;
    temp /= 1024;
	
    tempLvl = (uint16_t) (DieTemp_CountsTo_Celsius(temp));
    
	ADC_StopConvert();
    
	return tempLvl;
}


// void ADC_Worker() 
// read the ADC and take an average over a given amount of samples
// the value is then multiplied by a given slope value (defined in uVolts)
void ADC_Worker() {

    uint16 i = 0u;
    uint32 MeasureCycles = RegisterInterface->Datastore.Analog_Samples;
    int32 ADC[4] = {0x00};
    
    uint32 AnalogIn = 0u;
    uint32 Slope = 0u;
    
    for(i =0 ; i < MeasureCycles ; i++ ) {
            ADC_StartConvert();
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            while (!ADC_IsEndConversion(ADC_RETURN_STATUS));
			ADC_StopConvert();
            
            ADC[0] += ADC_GetResult16(0u);
            ADC[1] += ADC_GetResult16(1u);
            ADC[2] += ADC_GetResult16(2u);
            ADC[3] += ADC_GetResult16(3u);
    }
    // cycle through all 4 inputes
    for ( i = 0 ; i < 4 ; i++ ) {
        // ADC Value is in Bits which need to be divided by the number of measurement cycles
        AnalogIn = ADC[i]/MeasureCycles;
        // load the Slope value in uVolt
        Slope = RegisterInterface->Datastore.Analog_Slope[i];
        // calculate the answer in uVolt.
        AnalogIn *= Slope;
        // convert back to mVolt
        AnalogIn /= 1000;
        // store in Analog output register
        RegisterInterface->Datastore.Analog_Input[i] = (uint16) AnalogIn;
    }   
}

// not yet implemented.
void Coil_Worker()  {
   
}

void DHT22_Worker() {
    // cycle through the (4) DHT22's
    uint8 id;
    for ( id = 0 ; id < 4 ; id++ ) {
        if (DHT22_Download(&DHT22[id]) ) {
            RegisterInterface->Datastore.Humidity[id]       = DHT22[id].Humidity + RegisterInterface->Datastore.HumidityComp[id];
            RegisterInterface->Datastore.Temperature[id]    = DHT22[id].Temperature + RegisterInterface->Datastore.TemperatureComp[id];
        } else {
            RegisterInterface->Datastore.Humidity[id]       = 0x0000;
            RegisterInterface->Datastore.Temperature[id]    = 0x0000;
        }
    }
    // Update Warning flags. Either set or reset flag.
    if ( DHT22[0].IsValid ) RegisterInterface->System.Warning &= (0xFFFF ^ _WARN_DHT22_0_INVALID ); else RegisterInterface->System.Warning |= _WARN_DHT22_0_INVALID;
    if ( DHT22[1].IsValid ) RegisterInterface->System.Warning &= (0xFFFF ^ _WARN_DHT22_1_INVALID ); else RegisterInterface->System.Warning |= _WARN_DHT22_1_INVALID;
    if ( DHT22[2].IsValid ) RegisterInterface->System.Warning &= (0xFFFF ^ _WARN_DHT22_2_INVALID ); else RegisterInterface->System.Warning |= _WARN_DHT22_2_INVALID;
    if ( DHT22[3].IsValid ) RegisterInterface->System.Warning &= (0xFFFF ^ _WARN_DHT22_3_INVALID ); else RegisterInterface->System.Warning |= _WARN_DHT22_3_INVALID;

}
![The San Juan Mountains are beautiful!](/assets/images/san-juan-mountains.jpg "San Juan Mountains")

/**
    @fn void Memory_Worker()
    @details Function handels write and reset (request)
*/
void Memory_Worker() {
    if ( RegisterInterface->System.StorageController == 0x01) {
        uint32 status = Storage_Write();
        RegisterInterface->System.StorageController = 0x00;
        if ( status != CY_EM_EEPROM_SUCCESS ) {
            RegisterInterface->System.StorageController = 0x80 | status;
            
        } else {
            RegisterInterface->System.StorageController = 0x00;
        }
        
    } else if ( RegisterInterface->System.StorageController == 0xFFFF ) { 
        // Not Valid assume full reset. 
        // Can also be written through the modbus interface.
        Storage_Clear();
        Storage_Write();
        CySoftwareReset();
    }
}


uint16 AirTempMax_Hyst = 0;
uint16 AirTempMin_Hyst = 0;
uint16 Humidity_Hyst = 0;
uint16 Thermostat_Hyst = 0;
/**
    @fn uint8 Automated_Thermostat_Assistant()
    @return state of the state machine.
    @details Control the water valves, and heat request valve.
*/
uint8 Automated_Thermostat_Assistant() {
    // Thermostatic cycle
    
    enum HeaterControl  { CANCELED, OFF, REQUEST, REQUESTED } Heating = RegisterInterface->System.HeatingState;
    
    // DHT22_0: Circulatie uit woning (aanzuiging ELAN10)
    // DHT22_1: WTW Aanzuig uit woning
    // DHT22_2: Buitenlucht
    // DHT22_3: Naar woning 
    
    uint8 Thermostat = (RegisterInterface->Datastore.Digital_In_0 ^ 0x01); // invert from pull down to logic on / 1
    
    // state machine
    switch (Heating) {
        case REQUEST:
            RegisterInterface->Datastore.Digital_Out_0 = _RELAIS_OFF_AFTER(25); // activate 24VAC for 22 sec.
            RegisterInterface->Datastore.Digital_Out_3 = _RELAIS_ON;   // Open heating valve
            RegisterInterface->Datastore.Digital_Out_5 = _RELAIS_ON;   // start heat request to CV relais
            Heating = REQUESTED;
        case REQUESTED:
            if ( (RegisterInterface->System.AutomaticOverride & 0x0200 ) == 0x0200) {
                if ( SW_Aux_Read() ) {
                    RegisterInterface->System.AutomaticOverride |= 0x0002;
                    if ( DHT22[0].IsValid ) {
                        uint16 Temperature = (RegisterInterface->Datastore.Temperature[0] - Thermostat_Hyst);
                        // if temperature > 250 ignore to prevent uint8 overflow issues. 
                        if ( Temperature < 250 ) {
                            if ( (Temperature) > (RegisterInterface->System.ThermostatSetPoint & 0xFF) ) {
                                Heating = CANCELED;
                                Thermostat_Hyst = 0;   
                                RegisterInterface->System.AutomaticOverride &= (0xFFFF ^ 0x8000);
                            }
                        }
                    }
                } else {
                    // Set the Thermostat hysteresis to 0 as the thermostatic program is disabled
                    Thermostat_Hyst = 0;   
                    RegisterInterface->System.AutomaticOverride &= (0xFFFF ^ 0x8002); // reset flag 0x8000 and 0x0002
                    // Is the external thermostat off? than Cancel heating.
                    if ( !Thermostat ) 
                        Heating = CANCELED;
                }
            } 
            break;  
        case CANCELED:
            RegisterInterface->Datastore.Digital_Out_0 = _RELAIS_OFF_AFTER(25); // activate 24VAC for 22 sec.
            RegisterInterface->Datastore.Digital_Out_5 = _RELAIS_OFF;       // Stop heat request to CV relais
            RegisterInterface->Datastore.Digital_Out_3 = _RELAIS_OFF;       // Close heating valve
            Heating = OFF;
            break;
        case OFF:
            if ( (RegisterInterface->System.AutomaticOverride & 0x0200 ) == 0x0200) {
                if ( SW_Aux_Read() ) {
                    RegisterInterface->System.AutomaticOverride |= 0x0002;
                    if ( DHT22[0].IsValid ) {
                        uint16 Temperature = (RegisterInterface->Datastore.Temperature[0] - Thermostat_Hyst);
                        // if temperature > 250 ignore to prevent uint8 overflow issues. 
                        if ( Temperature < 250 ) {
                            if ( (Temperature) <= (RegisterInterface->System.ThermostatSetPoint & 0xFF) ) {
                                Thermostat_Hyst = (RegisterInterface->System.ThermostatSetPoint >> 8);
                                Heating = REQUEST;
                                RegisterInterface->System.AutomaticOverride |= 0x8000;
                            }
                        }
                    }
                } else {
                    // Set the Thermostat hysteresis to 0 as the thermostat is disabled
                    Thermostat_Hyst = 0;   
                    RegisterInterface->System.AutomaticOverride &= (0xFFFF ^ 0x8002); // reset flag 0x8000 and 0x0002
                    // Is the external thermostat off? than Cancel heating.
                    if ( Thermostat ) 
                        Heating = REQUEST;
                }
            }
            
            break;
        default:
            Heating = CANCELED;
            break;
    }
    return Heating;
}


/**
    @fn uint8 Automatied_Climate_Assistant()
    @details Controls the Airvalves based on Temperature.
    @return state of the ClimateControl statemachine
*/
uint8 Automated_Climate_Assistant() {
     uint16 Airtemp = 0;
    // Enumerators
    enum ClimateControl { RETURN_NORMAL, NORMAL, TO_HOT, TO_COLD, HIGH_TEMPERATURE_MODE, LOW_TEMPERATURE_MODE  } Climate = RegisterInterface->System.CirculationState;
    
    // DHT22_0: Circulatie uit woning (aanzuiging ELAN10)
    // DHT22_1: WTW Aanzuig uit woning
    // DHT22_2: Buitenlucht
    // DHT22_3: Naar woning 
    
        if ( SW_Mode_Read() && ((RegisterInterface->System.AutomaticOverride & 0x0100 ) == 0x0100) ) {
        RegisterInterface->System.AutomaticOverride |= 0x0001;
    
        // Safemode operation. if the sensor doesn't work always revert to normal operation.
        if ( (!DHT22[3].IsValid) && (Climate != NORMAL) ) {
               Climate = RETURN_NORMAL;
        }
        
        switch (Climate) {
            case RETURN_NORMAL:
                RegisterInterface->Datastore.Digital_Out_0 = _RELAIS_OFF_AFTER(48) ; // activate 24VAC for 48 sec.
                RegisterInterface->Datastore.Digital_Out_1 = _RELAIS_OFF; // Turn open air valve
                RegisterInterface->Datastore.Digital_Out_2 = _RELAIS_OFF; // Turn open air valve
                Climate = NORMAL;
                break;
            case NORMAL:
                if (DHT22[3].IsValid)  {
                    // Test for high temperatures
                    Airtemp = (RegisterInterface->Datastore.Temperature[3] / 10) + AirTempMax_Hyst;
                    if ( Airtemp >= (RegisterInterface->System.AirTempMax & 0xFF) ) {
                        AirTempMax_Hyst = (RegisterInterface->System.AirTempMax>>8)&0xFF;
                        Climate = TO_HOT; 
                    } 
                    // Test for low temperatures
                    Airtemp = (RegisterInterface->Datastore.Temperature[3] / 10) - AirTempMin_Hyst;
                    if ( Airtemp <= (RegisterInterface->System.AirTempMin & 0xFF) ) {
                        AirTempMax_Hyst = (RegisterInterface->System.AirTempMin>>8)&0xFF;
                        Climate = TO_COLD;
                    } 
                }
               break;
            case TO_COLD:
                // prepare for low temp mode.
                Climate = LOW_TEMPERATURE_MODE;
                RegisterInterface->Datastore.Digital_Out_0 = _RELAIS_OFF_AFTER(48) ; // activate 24VAC for 48 sec.
                RegisterInterface->Datastore.Digital_Out_1 = _RELAIS_ON; // Close air valve
                RegisterInterface->Datastore.Digital_Out_2 = _RELAIS_ON; // Close air valve
                break;
                
            case TO_HOT:
                // prepare for High temp mode.
                Climate = HIGH_TEMPERATURE_MODE;
                RegisterInterface->Datastore.Digital_Out_0 = _RELAIS_OFF_AFTER(48) ; // activate 24VAC for 48 sec.
                RegisterInterface->Datastore.Digital_Out_1 = _RELAIS_ON; // Close air valve
                RegisterInterface->Datastore.Digital_Out_2 = _RELAIS_ON; // Close air valve
                break;
            
            case HIGH_TEMPERATURE_MODE:
                // Check if Mode is still correct otherwise return to NORMAL
                if ( DHT22[3].IsValid ) {
                    Airtemp = (RegisterInterface->Datastore.Temperature[3] / 10) + AirTempMax_Hyst;
                    if ( Airtemp < (RegisterInterface->System.AirTempMax & 0xFF) ) {
                        AirTempMax_Hyst = 0;
                        Climate = RETURN_NORMAL;
                    } 
                }
                break;
                    
            case LOW_TEMPERATURE_MODE:
                // Check if mode is still correct otherwise change to RETURN_NORMAL
                if ( DHT22[3].IsValid ) {
                    Airtemp = (RegisterInterface->Datastore.Temperature[3] / 10) - AirTempMin_Hyst;
                    if ( Airtemp > (RegisterInterface->System.AirTempMin & 0xFF) ) {
                        AirTempMax_Hyst = 0;
                        Climate = RETURN_NORMAL;
                    }     
                }
                break;
            default:
                // unknown state, return to NORMAL
                Climate = RETURN_NORMAL;
                break;
        }
    } else {
        // disable The climate control program
        RegisterInterface->System.AutomaticOverride &= (0xFFFF ^ 0x8001);   
    }

    return Climate;
    
}

/**
    @fn uint8 Automatied_Humidity_Assistant()
    @return state of the Humidity control statemachine
    @details Control the mode of operation of the Heat recovery unit. Turn up the airflow if humidity is high.
*/
uint32 StartOverride = 0; // manual override max countdown.
uint8 Automated_Humidity_Assistant() {
    // Humidity Control
    uint8 Active = 0; // The Active flag is 1 when both sensor values are valid. 
    int16 Circulated;
    int16 Disposed;
    
    enum HumidityControl  { CANCEL, NORMAL, OVERRIDE, REQUEST_SHORT_OVERRIDE, REQUEST_MEDIUM_OVERRIDE, REQUEST_LONG_OVERRIDE, REQUEST_OVERRIDE, REQUEST_HIGHFLOW, HIGH_FLOW } Humidity = RegisterInterface->System.HumidityState;
    
     if ( (RegisterInterface->System.AutomaticOverride & 0x0400 ) == 0x0400 ) {
        RegisterInterface->System.AutomaticOverride |= 0x0004;
        // Humidity control:                
        if ( DHT22[0].IsValid && DHT22[1].IsValid ) {
            Circulated = (int16) RegisterInterface->Datastore.Humidity[0];
            Disposed   = (int16) RegisterInterface->Datastore.Humidity[1] + Humidity_Hyst;
            Active = 1; // Sensor data is valid. 
        } 
    }
    
    switch (Humidity) {
        case NORMAL:
            if ( Active ) {
                if ( Disposed > (Circulated + ((RegisterInterface->System.HumidityDevation&0xFF) *10) ) ) {
                    Humidity_Hyst = (RegisterInterface->System.HumidityDevation >> 8) * 10;
                    Humidity = REQUEST_HIGHFLOW;
                    // request increased airflow
                }
            }
        break;
        
        case OVERRIDE:
            // check timer, if ready automatically Cancel HighFlow
            if ( StartOverride <= RegisterInterface->Datastore.SystemUptimeTimer ) {
                Humidity = CANCEL;   
            }
        break;
        
        case HIGH_FLOW:
            if ( Active ) {
                if ( Disposed <= (Circulated + ((RegisterInterface->System.HumidityDevation&0xFF) *10) ) ) {
                    Humidity_Hyst = 0;
                    Humidity = CANCEL; // Cancel increased airflow
                }
            } else {
                // When the sensors are not valid. return to normal operation
                Humidity = CANCEL;
            }
        break;
        
        case REQUEST_HIGHFLOW:
            RegisterInterface->Datastore.Digital_Out_4 = _RELAIS_ON;   // Return to normal airflow
            Humidity = HIGH_FLOW;
        break;
        
        case REQUEST_SHORT_OVERRIDE:
            StartOverride = RegisterInterface->Datastore.SystemUptimeTimer + RegisterInterface->System.HighAirFlowShort;
            Humidity = REQUEST_OVERRIDE;
            break;
        case REQUEST_MEDIUM_OVERRIDE:
            StartOverride = RegisterInterface->Datastore.SystemUptimeTimer + RegisterInterface->System.HighAirFlowMedium;
            Humidity = REQUEST_OVERRIDE;
            break;
        case REQUEST_LONG_OVERRIDE: 
            StartOverride = RegisterInterface->Datastore.SystemUptimeTimer + RegisterInterface->System.HighAirFlowLong;
            Humidity = REQUEST_OVERRIDE;
        break;
        
        case REQUEST_OVERRIDE:
            RegisterInterface->Datastore.Digital_Out_4 = _RELAIS_ON;   // Return to normal airflow
            Humidity = OVERRIDE;
            break;
            
        default:
        case CANCEL:
            RegisterInterface->Datastore.Digital_Out_4 = _RELAIS_OFF;   // Return to normal airflow
            Humidity = NORMAL;
        break;
           
    }
    
    
    if ( (RegisterInterface->System.AutomaticOverride & 0x0400 ) == 0x0400 ) {
        RegisterInterface->System.AutomaticOverride |= 0x0004;
        // Humidity control:                
        if ( DHT22[0].IsValid && DHT22[1].IsValid ) {
            int16 Circulated = (int16) RegisterInterface->Datastore.Humidity[0];
            int16 Disposed   = (int16) RegisterInterface->Datastore.Humidity[1] + Humidity_Hyst;
            
            if ( Disposed > (Circulated + ((RegisterInterface->System.HumidityDevation&0xFF) *10) ) ) {
                Humidity_Hyst = (RegisterInterface->System.HumidityDevation >> 8) * 10;
            } else {
                Humidity_Hyst = 0;   
            }
        }
    }
    
    
    return Humidity;
}

/**
    @fn uint8 Automated_Worker()
    @return return the active automatic modes.
    @details The worker function call all automation assitants.
    
*/
uint8 Automated_Manager() {
    
    RegisterInterface->System.HeatingState = Automated_Thermostat_Assistant();
    RegisterInterface->System.CirculationState = Automated_Climate_Assistant();
    RegisterInterface->System.HumidityState = Automated_Humidity_Assistant();
 
    return (uint8) (RegisterInterface->System.AutomaticOverride & 0xFF);
}


void Input_Worker() {

    uint16 Mask = 0xFF00;
    uint8 input = DigitalIn_Read();
    
    RegisterInterface->Datastore.Digital_In_0 = ((input & 0x01) == 0x01)?1:0;
    RegisterInterface->Datastore.Digital_In_1 = ((input & 0x02) == 0x02)?1:0;
    RegisterInterface->Datastore.Digital_In_2 = ((input & 0x04) == 0x04)?1:0;
    RegisterInterface->Datastore.Digital_In_3 = ((input & 0x08) == 0x08)?1:0;
    RegisterInterface->Datastore.Digital_In_4 = ((input & 0x10) == 0x10)?1:0;
    RegisterInterface->Datastore.Digital_In_5 = ((input & 0x20) == 0x20)?1:0;
    RegisterInterface->Datastore.Digital_In_6 = ((input & 0x40) == 0x40)?1:0;
    RegisterInterface->Datastore.Digital_In_7 = ((input & 0x80) == 0x80)?1:0;
}


/**
    @fn void Output_Timer_Worker()
    @details manage the Timer / couters of the output channels.
*/
uint32 Timer_LastUptime = 0;
void Output_Timer_Worker() {
    uint16 delta = RegisterInterface->Datastore.SystemUptimeTimer - Timer_LastUptime;
    Timer_LastUptime = RegisterInterface->Datastore.SystemUptimeTimer;
    
    // if to large or zero this run
    if ( (delta > 512) || (delta == 0 ) ) 
        return;
        
    uint16 *output;
    output = &RegisterInterface->Datastore.Digital_Out_0;
    
    
    uint8 i;
    for ( i = 0 ; i < 8 ; i++ ) {
        uint16 counter = (output[i] >> 8);
        uint8 actions = ((output[i] >> 1) & 0x03);
        uint8 state = (output[i] & 0x01);
        if ( counter > delta ) {
            counter -= delta;
            output[i] &= 0x007F;
            output[i] |= (counter << 8);
            }
        else 
            counter = 0;
        
        if (counter == 0) {
            if ( (actions & 0x03 ) == 0x01 ) {
                // set on counter = 0
                output[i] = 0x0001;
            }
            else if ( (actions & 0x03 ) == 0x02 ) {
                // reset on counter = 0
                output[i] = 0x0000;   
            } else {
                // do nothing   
            }
        }
    }
}

void Output_Worker() {
    Output_Timer_Worker();
    uint8 output;
    output      =  (uint8)  (RegisterInterface->Datastore.Digital_Out_0 & 0x0001);      
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_1 & 0x0001)<<1);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_2 & 0x0001)<<2);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_3 & 0x0001)<<3);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_4 & 0x0001)<<4);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_5 & 0x0001)<<5);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_6 & 0x0001)<<6);  
    output      |= (uint8) ((RegisterInterface->Datastore.Digital_Out_7 & 0x0001)<<7);  
    DigitalOut_Write(output);
}


/* [] END OF FILE */
