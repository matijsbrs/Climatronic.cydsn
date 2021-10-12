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

 Since 3 Februari 2017
        +!01 Bug fix, problems due to wrong bitcounts in the backward frame are solved
        ~ The processing of the backward frame is now done based on timeposition 
*/

#include "`$INSTANCE_NAME`.h"
#include <project.h>

int `$INSTANCE_NAME`_ptr = 0;
int TimeTable[64] = {0};
char arr[64] = {0};
int diff;



//char byteArr[3] ={0};
char bit = 1;
int y = 0;
int arrptr = 0;


void `$INSTANCE_NAME`_Prepare_DB(struct value_struct db[]) {
	int i;
	for ( i = 0; i < 32 ; i++ ) {
		db[i].value = 0;
		db[i].state = UNKNOWN;
		db[i].PollInterval = 0;
	}
	/*
	`$INSTANCE_NAME`_add(db[0],_CMD_OFF,SENT,0);
	`$INSTANCE_NAME`_add(db[1],_CMD_ON, SENT,0);
	`$INSTANCE_NAME`_add(db[2],_QUERY_MAX_LEVEL,INBOUND,0);
	`$INSTANCE_NAME`_add(db[3],_QUERY_MIN_LEVEL,INBOUND,0);
	`$INSTANCE_NAME`_add(db[4],_QUERY_ACTUAL_LEVEL,RECEIVED,0);
	`$INSTANCE_NAME`_add(db[5],_QUERY_LAMP_FAILURE,INBOUND,1);
	*/
}


 
uint16_t daliIn[64] = {0};
uint32_t `$INSTANCE_NAME`_result = 0;
uint16_t ref =0;



void `$INSTANCE_NAME`_Direct_Ready_new() {
    `$INSTANCE_NAME`_ISR_Pulse_ClearPending();
    if ( `$INSTANCE_NAME`_ptr >= 0 ) rx_state = READY;
    
    
    
    //print("--=End Of Frame -> (%h %h %h) =--\n",val1, val2, val3);
}


void `$INSTANCE_NAME`_Direct_Pulse() {
    `$INSTANCE_NAME`_ISR_Timer_Ready_ClearPending();
	if ( rx_state == FREE ) {
		`$INSTANCE_NAME`_Timer_Start(); // assume first flank of startbit.
        `$INSTANCE_NAME`_ptr = 0;
        rx_state = RECEIVING;
		//UART_SpiUartWriteTxData('F');
	}
	if ( rx_state == RECEIVING ) {
		
		daliIn[`$INSTANCE_NAME`_ptr] = `$INSTANCE_NAME`_Timer_ReadCounter();
		`$INSTANCE_NAME`_ptr++;
        if ( `$INSTANCE_NAME`_ptr >= 64 ) {
            rx_state = READY; // Faulty indeed...
        }
	//	diff = 0xFFFF - Timer_ReadCounter();
	//	Timer_WriteCounter(0xFFFF);
	}
}




void `$INSTANCE_NAME`_DaliInputSetup() {
	`$INSTANCE_NAME`_ISR_Pulse_SetVector(&`$INSTANCE_NAME`_Direct_Pulse);
	`$INSTANCE_NAME`_ISR_Timer_Ready_SetVector(&`$INSTANCE_NAME`_Direct_Ready);
	`$INSTANCE_NAME`_ISR_Pulse_Start();
	`$INSTANCE_NAME`_ISR_Timer_Ready_Start();
}

void `$INSTANCE_NAME`_EnableDaliInput() {
	//ISR_Pulse_StartEx(&Direct_Pulse);
	//ISR_Timer_Ready_StartEx(&Direct_Ready);
	`$INSTANCE_NAME`_ISR_Pulse_Enable();
	`$INSTANCE_NAME`_ISR_Timer_Ready_Enable();
}

void `$INSTANCE_NAME`_DisableDaliInput() {
	`$INSTANCE_NAME`_ISR_Pulse_Disable();
	`$INSTANCE_NAME`_ISR_Timer_Ready_Disable();
}
char isQuery = 0;

void `$INSTANCE_NAME`_Direct_Ready() {
    `$INSTANCE_NAME`_ISR_Timer_Ready_ClearPending();
    int i;
    uint16_t diff;
    uint16_t Pos = 0;
    int backwardPos = 0;
    char bit = 1;
    char bitPtr = 0;
    char ibp = 0;
    char address = 0;
    char command = 0;
    char answer = 0;
    char TotalBitCount = 0;
    char isQuery = 1;
   `$INSTANCE_NAME`_result = 0;
    
    for ( i = 1 ; i < 64 ; i++ ) {
        diff = (daliIn[i-1] - daliIn[i]);
        if ( (bitPtr%2) == 0 ) {
            if (( bitPtr >= 2 )&& (bitPtr <= 16)) { 
                if ( bitPtr == 2 ) ibp = 7;
                address |= (bit<<ibp);
                ibp--;
                TotalBitCount++;
            }
            
            if (( bitPtr >= 18 )&& (bitPtr <= 32)) { 
                if ( bitPtr == 18 ) ibp = 7;
                command |= (bit<<ibp);
                ibp--;
                TotalBitCount++;
            } 
        }
        
       
        if ( (diff > 300) && (diff <= 550) ) {
            bitPtr++;
        } else if ((diff >= 700) && (diff <= 950)) {
            bit ^= 1;
            bitPtr+=2;
        } else if ((diff >= 2917) && (diff < 10083 ) ) {
            `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_BACKFRAME; // Has Backframe
            backwardPos = i;
            bitPtr+=2; // Changed back to 2 from 3, now the communications successeeds. 
            bit = 1;
        } else if ( ((diff > 950) && (diff < 2917 )) || ((diff > 550) && (diff < 700 )) ) {
            `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_ERROR_FAULTY_TIMING; // Faulty timing
        } else if ( (diff > 10083) && (diff <= 0xFFFF ) ) { // End of Frame condition.  
            if ( (`$INSTANCE_NAME`_result & `$INSTANCE_NAME`_BACKFRAME) == `$INSTANCE_NAME`_BACKFRAME ) { // has backframe?
                diff = daliIn[backwardPos] - daliIn[i-1];
                diff /= 17;
                if (TotalBitCount >= 23) { // Check if all bits are there. 
                    `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_SUCCES; // Succes!
                } else {
                    `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_ERROR_WRONG_BITCOUNT; // Wrong bit number!
                }
            } else {
                if ((TotalBitCount >= 16) && (isQuery == 0) ) { // Check if all bits are there.
                    `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_SUCCES; // Succes!
                } else {
                    if ( (`$INSTANCE_NAME`_result & `$INSTANCE_NAME`_BACKFRAME) == `$INSTANCE_NAME`_BACKFRAME ) {
                        `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_ERROR_WRONG_BITCOUNT; // Wrong bit number!
                    } else {
                        `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_NO_ANSWER | `$INSTANCE_NAME`_SUCCES; // no answer Succes
                    }
                }
            }
            break;
        }
        
        if ( ( Dali_result & Dali_BACKFRAME ) == Dali_BACKFRAME  ) {
            Pos = daliIn[backwardPos] - daliIn[i];
            Pos /= 833; // TE Time. 833.33 ±10% uS 
            if ( Pos > 0 ) {
                answer |= (bit<<(8-Pos));
                if ( (bitPtr%2) == 0 ) {
                    ibp--;
                    TotalBitCount++;
                }
            }
        }
        
        if ( (`$INSTANCE_NAME`_result & `$INSTANCE_NAME`_ERROR_FAULTY_TIMING) == `$INSTANCE_NAME`_ERROR_FAULTY_TIMING ) {
           break; // Return early Faulty condition.
        }
        
       

    }
    `$INSTANCE_NAME`_result |= (address<<16);
    `$INSTANCE_NAME`_result |= (command<<8);
    `$INSTANCE_NAME`_result |= (answer);
    //print("TotalBitCount: %d\n",TotalBitCount);
    //print("Addres:%h Command:%h Answer:%h Status:%h\n",address, command, answer, ((`$INSTANCE_NAME`_result>>24)&0xFF));
    
    `$INSTANCE_NAME`_Control_Reg_1_Write(0xFF);
	CyDelayUs(1);
	`$INSTANCE_NAME`_Control_Reg_1_Write(0x00);
	CyDelayUs(1);
    `$INSTANCE_NAME`_Timer_Stop();
	`$INSTANCE_NAME`_Timer_WriteCounter(0xFFFF);
}

uint32_t `$INSTANCE_NAME`_Listen() {
     if ( (`$INSTANCE_NAME`_Timer_ReadControlRegister() & `$INSTANCE_NAME`_Timer_CTRL_ENABLE) == `$INSTANCE_NAME`_Timer_CTRL_ENABLE) {
        int i;
        for ( i = 0 ; (i < 100) ; i++ ) {
            if (rx_state == READY) {
                break;
            }
            CyDelay(1); // 1mS delay.
        }
    }
    if (rx_state == FREE) {
        int i;
        for ( i = 0 ; i < 64 ; i++ ) {
            daliIn[i] = 0x00;   
        }
        return 0x00;
    }
    rx_state = FREE;
    return `$INSTANCE_NAME`_result;
}

uint32_t `$INSTANCE_NAME`_Query(char address, char command, char * Target, const char DefaultValue) {
    uint32_t results = `$INSTANCE_NAME`_Query_Advanced(address,command, Target);
    
    if ((((results>>24)&0xF0) > 0) ) Target[0] = DefaultValue; 
    return results;
}

uint32_t `$INSTANCE_NAME`_Query_Advanced(char address, char command, char * Target) {
    isQuery = 1;
    int i;
    for ( i = 0 ; i < 64 ; i++ ) daliIn[i] = 0; // clear array.
    `$INSTANCE_NAME`_Transmit(address, command); 
    // Wait for data to be processed. 
    for ( i = 0 ; (i < 100) ; i++ ) {
        if (rx_state == READY) {
            break;
        }
        CyDelay(1); // 1mS delay.
    }
    isQuery = 0;
    if ( `$INSTANCE_NAME`_getCommand(`$INSTANCE_NAME`_result) == command ) {
        if ( (`$INSTANCE_NAME`_getState(`$INSTANCE_NAME`_result) & 0xF0) == 0 ) {
            Target[0] = `$INSTANCE_NAME`_getAnswer(`$INSTANCE_NAME`_result);
        }
    } else {
        `$INSTANCE_NAME`_result |= `$INSTANCE_NAME`_ERROR_INCONCLUSIVE_DATA;
    }
    return `$INSTANCE_NAME`_result;
}

void `$INSTANCE_NAME`_Transmit(char address, char command) {
	int i;
	int delayValue = 400;
    rx_state = FREE;
	// start bit
	`$INSTANCE_NAME`_Dali_Out_Write(0xFF);
	CyDelayUs(delayValue); 
	`$INSTANCE_NAME`_Dali_Out_Write(0x00);
	CyDelayUs(delayValue); 
	
	char tmp[8] = {0};
	
	for ( i = 7 ; i >= 0 ; i-- ) {
		if ( ((address >> i ) & 0x01 ) == 0x01 ) {
			`$INSTANCE_NAME`_Dali_Out_Write(0xFF);
			CyDelayUs(delayValue); 
			`$INSTANCE_NAME`_Dali_Out_Write(0x00);
			CyDelayUs(delayValue); 
			tmp[i] = 1;
		} else {
			`$INSTANCE_NAME`_Dali_Out_Write(0x00);
			CyDelayUs(delayValue); 
			`$INSTANCE_NAME`_Dali_Out_Write(0xFF);
			CyDelayUs(delayValue); 
			tmp[i] = 2;
		}
	}
	
	for ( i = 7 ; i >= 0 ; i-- ) {
		if ( ((command >> i ) & 0x01 ) == 0x01 ) {
			`$INSTANCE_NAME`_Dali_Out_Write(0xFF);
			CyDelayUs(delayValue); 
			`$INSTANCE_NAME`_Dali_Out_Write(0x00);
			CyDelayUs(delayValue); 
		} else {
			`$INSTANCE_NAME`_Dali_Out_Write(0x00);
			CyDelayUs(delayValue); 
			`$INSTANCE_NAME`_Dali_Out_Write(0xFF);
			CyDelayUs(delayValue); 
		}
	}
	
	`$INSTANCE_NAME`_Dali_Out_Write(0x00); // high for stopbits
	
}

unsigned char interval = 0;

void `$INSTANCE_NAME`_Process() {
	int i;
	char address = 254;
	
	for( i = 0 ; i < 32 ; i++ ) {
		switch (Dali_DB[i].state) {
			case CLEAR:
				
				break;
			case UNKNOWN:
			
				break;
			case OUTBOUND:
				`$INSTANCE_NAME`_transmit(address, Dali_DB[i].value);
				Dali_DB[i].state = SENT;
				break;
			case INBOUND:
			
				break;
			case SENT:
				if ( Dali_DB[i].PollInterval > 0 ) {
					if ( (interval % Dali_DB[i].PollInterval) == (Dali_DB[i].PollInterval-1) ) 
						Dali_DB[i].state = OUTBOUND;
				}
				break;
			case RECEIVED:
			
				break;
			default:
				break;
		}
	}
	interval++;
}

void `$INSTANCE_NAME`_Analog_Mode() {
    `$INSTANCE_NAME`_Dali_In_SetDriveMode(`$INSTANCE_NAME`_Dali_In_DM_ALG_HIZ);
    `$INSTANCE_NAME`_Dali_Out_SetDriveMode(`$INSTANCE_NAME`_Dali_Out_DM_ALG_HIZ);
}

void `$INSTANCE_NAME`_Dali_Mode() {
    `$INSTANCE_NAME`_Dali_Out_SetDriveMode(`$INSTANCE_NAME`_Dali_Out_DM_STRONG);
    `$INSTANCE_NAME`_Dali_Out_Write(0x00); // bus high!
    `$INSTANCE_NAME`_Dali_In_SetDriveMode(`$INSTANCE_NAME`_Dali_In_DM_DIG_HIZ);
    `$INSTANCE_NAME`_Start();
}

void `$INSTANCE_NAME`_Stop() {
    rx_state = FREE;
    `$INSTANCE_NAME`_ptr = 0;
    `$INSTANCE_NAME`_Prepare_DB(Dali_DB);   
    `$INSTANCE_NAME`_timer_clock_Stop();
    `$INSTANCE_NAME`_ISR_Pulse_Stop();
	`$INSTANCE_NAME`_ISR_Timer_Ready_Stop();
	`$INSTANCE_NAME`_Dali_Out_Write(0x00); // bus high!
}

void `$INSTANCE_NAME`_Start() {
    rx_state = FREE;
    `$INSTANCE_NAME`_ptr = 0;
    `$INSTANCE_NAME`_Prepare_DB(Dali_DB);   
    `$INSTANCE_NAME`_timer_clock_Start();
    `$INSTANCE_NAME`_ISR_Pulse_StartEx(&`$INSTANCE_NAME`_Direct_Pulse);
	`$INSTANCE_NAME`_ISR_Timer_Ready_StartEx(&`$INSTANCE_NAME`_Direct_Ready);
	`$INSTANCE_NAME`_Dali_Out_Write(0x00); // bus high!
}

/* [] END OF FILE */
