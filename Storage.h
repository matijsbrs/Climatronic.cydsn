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

#include "project.h"
#include "MyTypes.h"

#ifndef _STORAGE_H
    #define _STORAGE_H
    
    #define LOGICAL_EEPROM_START    0u

    
    void Storage_Init();
    void Storage_Clear();
    cystatus Storage_Write();
    void * Storage_Read();
    
#endif
    

/* [] END OF FILE */
