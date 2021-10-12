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

#include "MyTypes.h"

#ifndef _WORKERS_H
    #define _WORKERS_H
    
    #define _RELAIS_SET_TIME(x)     (x<<8)
    #define _RELAIS_GET_TIME(x)     ((x>>8)& 0xFF)
    #define _RELAIS_ON              0x0000
    #define _RELAIS_OFF             0x0001
    #define _RELAIS_TIMED_ON_MSK    0x0004
    #define _RELAIS_TIMED_OFF_MSK   0x0002
    #define _RELAIS_ON_AFTER(x)     _RELAIS_SET_TIME(x) | _RELAIS_TIMED_ON_MSK | _RELAIS_OFF
    #define _RELAIS_OFF_AFTER(x)    _RELAIS_SET_TIME(x) | _RELAIS_TIMED_OFF_MSK | _RELAIS_ON
    
    
    
    void Memory_Worker();
    void DHT22_Worker();
    void Coil_Worker();
    void ADC_Worker();
    uint16_t ADC_Assistent_MeasureTemp();
    uint8 Automated_Manager();
    
    void Output_Worker();
    void Output_Timer_Worker();
    void Input_Worker();
    
#endif


/* [] END OF FILE */
