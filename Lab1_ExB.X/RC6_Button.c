// In order to use device register names like PORTB you must include <xc.h> 
#include <xc.h>

// Include provided code for Lab 1.
#include "Lab1_ExA.h"

// This instruction sets the watchdog timer to user-controlled, 
// so the device will not automatically reset itself.
#pragma config WDTE = SWDTEN 

// This is necessary for the builtin function __delay_ms() to work.
// It tells the compiler what frequency the device will be running at.
#define _XTAL_FREQ 500000

void main(void) {
    
    /*Pin Configuration*/
    TRISB = 0;
    TRISCbits.TRISC7 = 0;
    TRISCbits.TRISC6 = 1;
    ANSELB = 0;
    ANSELC = 0;
    PORTB = 0x0;
    PORTCbits.RC7 = 0;
    PORTCbits.RC6 = 0;
    
    /*PIC User Manual Chapter 13: Interrupts
     This is our configuration to set up RC6 as a interrupt on rising edge logic*/
    INTCONbits.IOCIE = 1; //Tells Processor to allow for interrupts to occur.
    IOCCPbits.IOCCP6 = 1; //Assigns RC6 to interrupt on rising edge.
    
    while(1)
    {
        RC6button(0b00011000);
    }
    return;
}
void RC6button(unsigned char a)
{
    /*This function is designed to set bits a to an on/off logic switch*/
    
    /*Need to refine code so that the switch is uniquely set to bits (a), right now it resets all bits in PORTB to 0
     we want to make it so that it only sets bits a to zero*/
    static int swi = 1;

    if(IOCCFbits.IOCCF6 == 1 && swi == 1) // If flag is true and switch is set to off
    {
        IOCCFbits.IOCCF6 = 0; // Removes flag
        PORTB = a | PORTB;
        swi = 0;
    }
    else if(IOCCFbits.IOCCF6 == 1 && swi == 0) // If flag is true and switch is set to on
    {
        IOCCFbits.IOCCF6 = 0; // Removes flag
        PORTB = 0;
        swi = 1;
    }
}

