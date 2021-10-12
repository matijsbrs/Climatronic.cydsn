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


@version 1.1 18-11-2015 + added support for analog io on Dali Bus. 
                        + Stop Function.
*/

#ifndef _DALI_MASTER_H_
#define _DALI_MASTER_H_

#include <project.h>
    
#define `$INSTANCE_NAME`_CMD_OFF 0
#define `$INSTANCE_NAME`_CMD_ON 1
#define `$INSTANCE_NAME`_CMD_UP_ 2
#define `$INSTANCE_NAME`_CMD_DOWN_ 3
#define `$INSTANCE_NAME`_CMD_RECALL_MAX_LEVEL 4
#define `$INSTANCE_NAME`_CMD_RECALL_MIN_LEVEL 5
#define `$INSTANCE_NAME`_GOTO_SCENE_0 16
#define `$INSTANCE_NAME`_GOTO_SCENE_1 17
#define `$INSTANCE_NAME`_GOTO_SCENE_2 18
#define `$INSTANCE_NAME`_GOTO_SCENE_3 19
#define `$INSTANCE_NAME`_GOTO_SCENE_4 20
#define `$INSTANCE_NAME`_GOTO_SCENE_5 21
#define `$INSTANCE_NAME`_GOTO_SCENE_6 22
#define `$INSTANCE_NAME`_GOTO_SCENE_7 23
#define `$INSTANCE_NAME`_GOTO_SCENE_8 24
#define `$INSTANCE_NAME`_GOTO_SCENE_9 25
#define `$INSTANCE_NAME`_GOTO_SCENE_10 26
#define `$INSTANCE_NAME`_GOTO_SCENE_11 27
#define `$INSTANCE_NAME`_GOTO_SCENE_12 28
#define `$INSTANCE_NAME`_GOTO_SCENE_13 29
#define `$INSTANCE_NAME`_GOTO_SCENE_14 30
#define `$INSTANCE_NAME`_GOTO_SCENE_15 31
#define `$INSTANCE_NAME`_CMD_RESET 32
#define `$INSTANCE_NAME`_QUERY_STATUS 144
#define `$INSTANCE_NAME`_QUERY_LAMP_FAILURE 146
#define `$INSTANCE_NAME`_QUERY_LAMP_POWER_ON 147
#define `$INSTANCE_NAME`_QUERY_ACTUAL_LEVEL  160
#define `$INSTANCE_NAME`_QUERY_MAX_LEVEL 161
#define `$INSTANCE_NAME`_QUERY_MIN_LEVEL 162
#define `$INSTANCE_NAME`_DIRECT_ARC 254

#define `$INSTANCE_NAME`_RESPONSE_OK 255
#define `$INSTANCE_NAME`_BROADCAST 0xFF 
    
#define `$INSTANCE_NAME`_ERROR_FAULTY_TIMING 0X80000000
#define `$INSTANCE_NAME`_ERROR_WRONG_BITCOUNT 0X40000000
#define `$INSTANCE_NAME`_ERROR_NO_ANSWER 0X20000000
#define `$INSTANCE_NAME`_ERROR_INCONCLUSIVE_DATA 0X10000000
#define `$INSTANCE_NAME`_NO_ANSWER 0X04000000
#define `$INSTANCE_NAME`_BACKFRAME 0X02000000
#define `$INSTANCE_NAME`_SUCCES 0X01000000
    
    
#define `$INSTANCE_NAME`_Succes(x) (((x>>24)&0xF0) == 0)
#define `$INSTANCE_NAME`_getState(x) ((x>>24)&0xFF)
#define `$INSTANCE_NAME`_getValue(x) ((x>>16)&0xFF)
#define `$INSTANCE_NAME`_getCommand(x) ((x>>8)&0xFF)
#define `$INSTANCE_NAME`_getAnswer(x) ((x)&0xFF)
    
    
#define `$INSTANCE_NAME`_add(record, x,z,i) record.value = x; record.state = z; record.PollInterval = i;

enum rx_buf_state_enum {FREE, RECEIVING, READY} rx_state;
char byteArr[3];

extern uint16_t daliIn[64];
extern uint32_t `$INSTANCE_NAME`_result;

struct value_struct {
	//char address;
	char value;
	char answer;
	enum frame_state_enum { CLEAR, UNKNOWN, OUTBOUND, INBOUND, SENT, RECEIVED} state;
	char PollInterval;
	//enum frame_type_enum { SEND, RECEI
} Dali_DB[32];
// `$INSTANCE_NAME`_



void `$INSTANCE_NAME`_Start();
void `$INSTANCE_NAME`_Stop();
void `$INSTANCE_NAME`_Transmit(char address, char command);
uint32_t `$INSTANCE_NAME`_Listen();
uint32_t `$INSTANCE_NAME`_Query_Advanced(char address, char command, char * Target);
uint32_t `$INSTANCE_NAME`_Query(char address, char command, char * Target, const char DefaultValue);
void `$INSTANCE_NAME`_DisableDaliInput();
void `$INSTANCE_NAME`_EnableDaliInput();
void `$INSTANCE_NAME`_DaliInputSetup();
void `$INSTANCE_NAME`_Direct_Pulse_2();
void `$INSTANCE_NAME`_Direct_Pulse();
void `$INSTANCE_NAME`_Direct_Ready();
void `$INSTANCE_NAME`_Prepare_DB(struct value_struct db[]);
void `$INSTANCE_NAME`_Analog_Mode();
void `$INSTANCE_NAME`_Dali_Mode();
void `$INSTANCE_NAME`_Dali_Listen_Mode();



#endif
/* [] END OF FILE */
