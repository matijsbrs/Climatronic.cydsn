/* ========================================
 *
 * Copyright Sam Walsh, 2014
 * All Rights Reserved.
 *
 * Modbus.h Contains all the modbus function prototypes
 *
 * ========================================
*/
#include <project.h>
#include <cydevice_trm.h>
#include <CyLib.h>
#include <interrupts.h>
#include <ModbusUART.h>
#include <MessageTimer.h>

#ifndef _MODBUS
    #define _MODBUS

typedef void (*UART_TxByte)(uint32 data);
typedef uint32 (*UART_RxByte)(void);
typedef cystatus (*StoreHoldingRegister) (void);

extern struct Modbus_Config_s {
    UART_TxByte Uart_PutByte;
    UART_RxByte Uart_GetByte;
    unsigned char * SlaveAddress;
    StoreHoldingRegister StoreWorker;
    
} Modbus_Config;

//Function prototypes//
void decodeMessage(unsigned char data);
void checkIfEndOfMessage(void);
void processMessage(void);
unsigned int generateCRC(unsigned char messageLength);
unsigned char checkCRC(void);

void Modbus_Init();

void readReg(void);        
void readInputReg(void);
void writeReg(void);
void writeMultipleRegs(void);
void readCoil(void);
void readInputCoil(void);
void writeCoil(void);
void writeMultipleCoils(void);

void responseFinished(int delay);

#endif
/* [] END OF FILE */
