/* ========================================
 *
 * Copyright Sam Walsh, 2014
 * All Rights Reserved.
 *
 * main.c put your code in here after the processMessage();
 *
 * ========================================
*/
#include <project.h>
#include <interrupts.h>
#include <Modbus.h>
#include <stdlib.h>
#include <string.h>
#include "WDT.h"
#include "startup.h"
#include "toolkit.h"
#include "MyTypes.h"
#include "Storage.h"
#include "Workers.h"

// DHT22 storage structs
struct DHT22_Param_s DHT22[4];  

// main storage pointer
interface_t * RegisterInterface;


void PerSecond_Handler() {
    // This function does the time counting, important! 
    PerSecondInterrupt_ClearPending();
    RegisterInterface->Datastore.SystemUptimeTimer++;
    RTC_Update();   
    RegisterInterface->Datastore.RTC_UnixTime = RTC_GetUnixTime();
}

int main()
{   
    // clear memory
    memset(&received[0], 0, sizeof(received));
    
    // Setup the system.
    Setup();    

    // Enable interrupts                                                                                                                                                                    
    CyGlobalIntEnable;
    
    // Main program
    uint32 updated = 0;
    while(forever)
    {       
        // Parttime Workers
        if ( ( updated != RegisterInterface->Datastore.SystemUptimeTimer) ) {
            
            // run once per second.
            updated = RegisterInterface->Datastore.SystemUptimeTimer;   
            // workers
            DHT22_Worker();
            RegisterInterface->System.DieTemperature = ADC_Assistent_MeasureTemp();
            
            uint8 state = Automated_Manager();
      
            // Automatic valve program active
            ERR_LED_Write(state&0x01);
            // Automatic Thermostatic program active
            OK_LED_Write((state&0x02)>>1);
        }
        
        // Fulltime workers
        Input_Worker();
        Output_Worker();
        ADC_Worker();
        Coil_Worker();
        Memory_Worker();
        
        
        // Pad the watchdog.
        if ( RegisterInterface->System.UseWatchDog )
            CySysWatchdogFeed(CY_SYS_WDT_COUNTER1);
        
    }
}

