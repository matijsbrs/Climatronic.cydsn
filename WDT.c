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
#include <stdio.h>

#define interruptNumWDT 9
char snum[32]; // Used for sprintf conversion
uint8 currentStatus;

/***************************************************************************//**
* Function Name: ISR_WatchDog
********************************************************************************
*
* Summary:
* This function sets a flag to the background process, and clears the interrupt
* source so that the interrupt can occur again.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(ISR_WatchDog)
{
    /* Sets flag for printing the current count status of the Watchdog Timer */
    currentStatus = 1u;
    
    /* Clear interrupts state and pending interrupts */
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
    CyIntClearPending(interruptNumWDT);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
*  The main function performs the following actions:
*   1. Displays the cause/source of reset: either WDT or power up / reset button.
*   2. Interrupt number and handler are initialized.
*   3. WDT counter 0 and 1 are set up to generate interrupts and resets properly.
*   4. Both WDT counters are enabled.
*   5. Count is kept track through the interrupt and displayed through UART.
*
*******************************************************************************/
void WDT_Start(void)
{
    /* Cause of reset flag */
    uint8 resetSource = *(reg32 *)CYREG_RES_CAUSE;

     
    if (0u == resetSource) /* Source is initial power or reset */
    {
        //UART_UartPutString("Power up or Reset button pressed\r\n");
    }
    else /* Source is Watchdog Timer */
    {   
        //UART_UartPutString("Watchdog Timer");
    }
    
    /* Sets interrupt handler for WDT and enables interrupt number */
    CyIntSetVector(interruptNumWDT, ISR_WatchDog);
    CyIntEnable(interruptNumWDT);
    
    /* Enabling global interrupts */
    CyGlobalIntEnable;
    
    /* Set the first WDT counter to generate interrupt on match */
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, 0x5000);
    
    /* Counter 0 is cleared when there is match */
    CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER0, 1u);
    
    /* Enable WDT conters 0 and 1 cascade */
    CySysWdtWriteCascade(CY_SYS_WDT_CASCADE_01);
    
    /* Set the second WDT counter to generate reset on match */
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER1, CY_SYS_WDT_MODE_RESET);
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER1, 9);
    
     /* Counter 1 is cleared when there is match */
    CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER1, 1u);
    
    /* Counter 0 and Counter 1 are enabled */
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK | CY_SYS_WDT_COUNTER1_MASK);

    /* Initialize current status flag */
    currentStatus = 0u;
   
}

/* [] END OF FILE */