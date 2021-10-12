#include "stdbool.h"
#include "project.h"
#include "DHT22.h"
uint16_t times[64];

bool DHT22_Download(struct DHT22_Param_s * param) {

    int i;
    uint8 IState;
    param->Temperature |= 0x8000;
    param->Humidity |= 0x8000;    
    IState=CyEnterCriticalSection();  
    uint8 bits[5]; 
    uint8 cnt = 7; 
    uint8 idx = 0; 
    int   calc=0; 
    int   timeout=0; 
    for (i=0; i< 5; i++)  
        bits[i] = 0; 
    
    param->WritePin(0x00);
    CyDelayUs(1000);
    param->WritePin(0xFF);
        
    while(param->ReadPin()==1) 
   { 
       timeout++; 
        if(timeout>500) {
           param->Humidity = 0xFFFF;
            param->Temperature = 0xFFFF;
            param->IsValid = false;
            CyExitCriticalSection(IState); 
            return param->IsValid;
        }
 
   } 
   
   while(param->ReadPin()==0) 
   {         
       timeout++; 
       if(timeout>500) {
           param->Humidity = 0xFFFF;
            param->Temperature = 0xFFFF;
            param->IsValid = false;
            CyExitCriticalSection(IState); 
            return param->IsValid;
        }
   } 
   calc=timeout; 
   timeout=0; 
   while(param->ReadPin()==1); 
   for (i=0; i<40; i++) 
	{ 
       timeout = 0; 
        while((param->ReadPin()==0)) { 
            timeout++;
             if(timeout>500) {
                param->Humidity = 0xFFFF;
                param->Temperature = 0xFFFF;
                param->IsValid = false;
                CyExitCriticalSection(IState); 
                return param->IsValid;
            }
        }
        timeout = 0;
        while(param->ReadPin()==1) {
            timeout++;
            if(timeout>500) {
                param->Humidity = 0xFFFF;
                param->Temperature = 0xFFFF;
                param->IsValid = false;
                CyExitCriticalSection(IState); 
                return param->IsValid;
            }
        }
   
       //Data acquiring point 
       if ((timeout) > (calc/2)) 
           bits[idx] |= (1 << cnt); 
       if (cnt == 0)   // next byte? 
   	{ 
   		cnt = 7;    // restart at MSB 
   		idx++;      // next byte! 
   	} 
   	else cnt--; 
   } 
   param->Humidity    = bits[0]<<8 | bits[1] ;  
   param->Temperature = bits[2]<<8 | bits[3]; 
   CyExitCriticalSection(IState);    
//    if ( (param->Humidity > 0 ) && (param->Temperature > 0) )
        param->IsValid = true;
//   else 
//        param->IsValid = false;

   //CyDelay(1); 
   return param->IsValid; 
    
}
/*
int DHTread() 
{     
   int i;
   uint8 IState;
   IState=CyEnterCriticalSection();  
   uint8 bits[5]; 
   uint8 cnt = 7; 
   uint8 idx = 0; 
   int   calc=0; 
   int   timeout=0; 
   for (i=0; i< 5; i++)  
       bits[i] = 0; 
   AfvoerPin_Write(0u); 
   CyDelayUs(1200); 
   AfvoerPin_Write(1u); 
   while(AfvoerPin_Read()==1) 
   { 
       timeout++; 
       if(timeout>500) 
           goto r99;  //DHT error function 
   } 
   while(AfvoerPin_Read()==0) 
   {         
       timeout++; 
       if(timeout>500) 
           goto r99; //DHT error function 
   } 
   calc=timeout; 
   timeout=0; 
   while(AfvoerPin_Read()==1); 
   for (i=0; i<40; i++) 
	{ 
       timeout=0; 
       while(AfvoerPin_Read()==0); 
       while(AfvoerPin_Read()==1) 
           timeout++; 
       //Data acquiring point 
       if ((timeout) > (calc/2)) 
           bits[idx] |= (1 << cnt); 
       if (cnt == 0)   // next byte? 
   	{ 
   		cnt = 7;    // restart at MSB 
   		idx++;      // next byte! 
   	} 
   	else cnt--; 
   } 
   humidity    = bits[0]<<8 | bits[1] ;  
   temperature = bits[2]<<8 | bits[3];  
   CyExitCriticalSection(IState); 
   CyDelay(1); 
   return 0; 
   r99:    //Goto label for error in DHT reading 
       humidity    = 99;  
       temperature = 99;  
       CyExitCriticalSection(IState); 
       return 99; 
}
*/