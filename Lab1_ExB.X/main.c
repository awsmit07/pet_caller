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

#define pause 500

void main(void) {

    // This function configures pins RC7 and RB0-7 for digital output, and pin
    // RC6 for digital input. It will be replaced in Exercise B
    
    TRISB = 0b00000000; 
    PORTB = 0x00;
    while(1)
    {
        // This function reads the button connected to RC6, and turns the LED
        // connected to RC7 on or off. It will be replaced in Exercise B.
        
        Lab1_LEDPattern_Andy();
//        PORTB = 0b11110000;
//        __delay_ms(pause);
//        PORTB = 0b11111111;
//        __delay_ms(pause);
//        PORTB = 0b00001111;
//        __delay_ms(pause);
//        PORTB = 0b00000000;
//        __delay_ms(pause);
        
//        PORTBbits.RB3 = 1;
//        PORTBbits.RB4 = 1;
//        PORTBbits.RB5 = 1;
//        __delay_ms(pause);  //changed from 10
//        
//        PORTBbits.RB0 = 1;
//        PORTBbits.RB1 = 1;
//        PORTBbits.RB2 = 1;
//        __delay_ms(pause);  //changed from 10
//
//        PORTBbits.RB3 = 0;
//        PORTBbits.RB4 = 0;
//        PORTBbits.RB5 = 0;
//        __delay_ms(pause);  //changed from 10
//        
//        PORTBbits.RB0 = 0;
//        PORTBbits.RB1 = 0;
//        PORTBbits.RB2 = 0;
//        __delay_ms(pause);  //changed from 10
        
        // This function lights each the LEDs connected to RB0-7, one by one.
        // It will be replaced in Exercise C.
        
        // This function is built into MPLAB's compiler. It will waste cycles
        // for the specified time. It requires _XTAL_FREQ to be defined.
    }
    return;
}

void Lab1_LEDPattern_Andy(void)
{
    // Set the pattern to 0b0000_0001
    char pattern = 0xfe;
    
    // Set the count for the position in the pattern.
    static unsigned char count = 0;
    
    // Move the pattern count bits left. 
    PORTB = pattern << count | pattern >> (8-count);
    __delay_ms(pause);
    
    // Increment the count mod 8.
    count = (++count)%8;    
}