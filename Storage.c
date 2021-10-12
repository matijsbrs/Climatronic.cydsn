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
#include "Storage.h"

const uint8_t eepromArray[EEPROM_PHYSICAL_SIZE] __ALIGNED(CY_FLASH_SIZEOF_ROW) = {0u};


void Storage_Init() {
    cy_en_em_eeprom_status_t eepromReturnValue = EEPROM_Init((uint32_t)eepromArray);
    if(eepromReturnValue != CY_EM_EEPROM_SUCCESS)
    {
        RegisterInterface->System.StorageController = 0x80 | eepromReturnValue;
        RegisterInterface->System.Error |= _ERR_FLASH_INIT;
        
    } else {
        RegisterInterface->System.Error ^= 0xFFFF ^ _ERR_FLASH_INIT;   
    }
}

void Storage_Clear() {
     // clear memory
    memset(&holdingReg[0], 0, sizeof(holdingReg));   
        
}

cystatus Storage_Write() {
	uint16 eepromSize	= sizeof(holdingReg);
    cy_en_em_eeprom_status_t eepromReturnValue = EEPROM_Write(LOGICAL_EEPROM_START, holdingReg, eepromSize); 
    if(eepromReturnValue != CY_EM_EEPROM_SUCCESS)
    {
        RegisterInterface->System.StorageController = 0x80 | eepromReturnValue;
        RegisterInterface->System.Error |= _ERR_FLASH_WRITE;
        
    } else {
        RegisterInterface->System.Error ^= 0xFFFF ^ _ERR_FLASH_WRITE;   
    }
    
	return eepromReturnValue;
}

void * Storage_Read() {
    uint16 eepromSize	= sizeof(holdingReg);
    cy_en_em_eeprom_status_t eepromReturnValue = EEPROM_Read(LOGICAL_EEPROM_START, holdingReg, eepromSize);
    if(eepromReturnValue != CY_EM_EEPROM_SUCCESS)
    {
        RegisterInterface->System.StorageController = 0x80 | eepromReturnValue;
        RegisterInterface->System.Error |= _ERR_FLASH_READ;
    } else {
        RegisterInterface->System.Error ^= 0xFFFF ^ _ERR_FLASH_READ;  
    }
    return holdingReg;
}


/* [] END OF FILE */
