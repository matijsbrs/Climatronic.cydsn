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
#include "toolkit.h"

int add_int(int a, int b) {
    return a+b;
} 

int32 map_int32(int32 x, int32 in_min, int32 in_max, int32 out_min, int32 out_max) {
    int32 output = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if ( output > out_max ) output = out_max;
    if ( output < out_min ) output = out_min;
    return output;
}

int16 map_int16(int16 x, int16 in_min, int16 in_max, int16 out_min, int16 out_max) {
    int16 output = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if ( output > out_max ) output = out_max;
    if ( output < out_min ) output = out_min;
    return output;
}

/* qsort int comparison function */ 
int int_cmp(const void *a, const void *b) { 
    const uint32 *ia = (const uint32 *)a; // casting pointer types 
    const uint32 *ib = (const uint32 *)b;
    return *ia  - *ib; 
	// integer comparison: returns negative if b > a and positive if a > b 
} 



/* [] END OF FILE */
