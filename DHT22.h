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


#if !defined(_DHT22_H) /* Pins DHT22_A_H */
#define _DHT22_H

#include "project.h"
#include "stdbool.h"
  
struct DHT22_Param_s {
    uint8_t (*ReadPin)();           // @4_32
    void (*WritePin) (uint8_t);     // @4_32
    uint16_t Humidity;              // @2_16
    uint16_t Temperature;           // @2_16
    bool IsValid;                   // @1_8   true when last measurement is valid.
    bool IsEnabled;                 // @1_8   true read values
    bool IsActive;                  // @1_8   true when beeing read
    bool Reserved;                  // @1_8   
};  // TotalSize = 16 bytes / 128 Bit

bool DHT22_Download(struct DHT22_Param_s * param);

#endif
/* [] END OF FILE */
