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
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    ANSELB = 0;
    ANSELC = 0;
    PORTB = 0x0;
    PORTCbits.RC7 = 0;
    PORTCbits.RC6 = 0;
    
    while(1)
    {
        RC6button(0b00011000);
    }
    return;
}
void RC6button(unsigned char a)
{
    /*Need to refine code so that the switch is uniquely set to a bit, right now it resets all bits in PORTB to 0
     we want to make it so that it only sets bits (a) to zero*/
    static int swi = 1;
    if(PORTCbits.RC6 == 1 && swi == 1)
    {
        PORTB = a | PORTB;
        swi = 0;
    }
    else if(PORTCbits.RC6 == 1 && swi == 0)
    {
        PORTB = 0;
        swi = 1;
    }
}
