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

    // Configure Port B as an output
    TRISB = 0b00000000;
    PORTB = 0x00;

    while(1)
    {
        // Display LED pattern.
        Lab1_LEDPattern();
    }

    return;
}


void Lab1_LEDPattern(void)
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
