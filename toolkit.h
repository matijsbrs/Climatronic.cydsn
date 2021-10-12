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

#include <project.h>

#ifndef _TOOLKIT_H
#define _TOOLKIT_H

//#define Random_Between(min, max) (min + (rand()%(max-min)))
#define Random_Between_StepSize(min, max, stepSize) ((min + (rand()%(max-min)))/stepSize*stepSize)
#define Between(input, min, max) (min + (input%(max-min)))
#define Constrain(input, min, max) 
#define IsBetween(a, b, c) ((a>=b) && (a<=c))
#define Absolute_Diff(a, b) ((a)>(b) ? (a)-(b) : (b)-(a))
#define SetOnGreater(base, target) if ((target)>(base)) base = target;
#define SetOnSmaller(base, target) if ((target)<(base)) base = target;
#define WeightedAvg(Current, NewValue, Ratio) Ratio>0?(Current = ((Current*Ratio)+NewValue) / (Ratio+1)):(Current=NewValue)
#define MIN(a, b)   ((a<b)?a:b)
#define MAX(a, b)   ((a>b)?a:b)
#define ABS(a)      ((a>=0)?a:-a)    
    
    
#define  forever    1
    
    //int16_t rssi_avg = ((Telecontroller_ConnStatus.Radio_Status.rssi_avg * GRFC) + rssi) / (GRFC+1);    
    
#define Check(a,b) ((a&b)==b)
#define SetBit(a,b) (b|(1<<a))
#define ResetBit(a,b) (b&(0xFF^(1<<a)))
#define ResetBit16(a,b) (b&(0xFFFF^(1<<a)))
#define ResetBit32(a,b) (b&(0xFFFFFFFF^(1<<a)))
#define AND(a,b) ((a&b)==b)
#define NAND(a,b) ((a&b)!=b)
#define _ENABLED    (1==1)
#define _DISABLED   (1==0)
#define _SET    (1==1)
#define _RESET   (1==0)
    
#define JSigned(input) (input&0x80)?((input&0x7F)):((((signed char)input)*-1));

int add(int a, int b);
int32 map_int32(int32 x, int32 in_min, int32 in_max, int32 out_min, int32 out_max);
int16 map_int16(int16 x, int16 in_min, int16 in_max, int16 out_min, int16 out_max);

int int_cmp(const void *a, const void *b);
#endif
/* [] END OF FILE */
