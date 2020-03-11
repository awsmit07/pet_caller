#include "Lab2_Library.h"

// REQUIRES: System clock speed is 32MHz.
//           The period is limited to a maximum of 255us.
// PROMISES: Configure Timer2 to trigger with the given period in microseconds.
//           Start Timer2. It will automatically restart when it finishes.
void Lab2_ConfigureTimer2(char TimerPeriod_us)
{    
    // T2PR: TMR2 PERIOD REGISTER (Page 287)
    // The timer runs at 1MHz, so it increments every 1us.
    T2PR = TimerPeriod_us;
    
    // T2CLKCON: TIMER2 CLOCK SELECTION REGISTER (Page 306))
    // Bits 8:4 = 0b00000   These bits are not implemented
    // Bits 3:0 = 0b0010    Clock source is the system clock
    T2CLKCON = 0x02;

    // T2CON: TIMERx CONTROL REGISTER (Page 307)
    // Bit  7   = 0b1       Timer is on.
    // Bits 6:4 = 0b101     Prescaler is 1:32 (Timer increments every 32 cycles)
    // Bits 3:0 = 0b0000    Postscaler is 1:1 
    T2CON = 0xD0;
            
    // TMR2IF is an interrupt flag, and is set whenever Timer2 expires.
    // We are not using interrupts, but it can still be checked manually.
    // It is cleared here as we start the timer.
    PIR1bits.TMR2IF = 0;
    
    return;
}