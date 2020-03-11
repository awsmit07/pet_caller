#ifndef LAB2_LIBRARY_H
#define	LAB2_LIBRARY_H

// Device configuration 
#pragma config FOSC = INTOSC    // Oscillator Selection Bits->INTOSC oscillator: I/O function on CLKIN pin
#pragma config WDTE = OFF    // Watchdog Timer Enable->WDT disabled
#pragma config PLLEN = ON    // Phase Lock Loop enable->4x PLL is always enabled

// Library Files
#include <stdbool.h>
#include <xc.h>




/* -------------------- Functions provided in Timer.c -------------------- */

// REQUIRES: System clock is set to 16MHz
// PROMISES: Configure Timer2 to trigger with the given period in microseconds.
//           Start Timer2. It will automatically restart when it finishes.
void Lab2_ConfigureTimer2(char TimerPeriod_us);

/* --------------------- Functions provided in DAC.X.a ---------------------- */

// REQUIRES: Nothing.
// PROMISES: Configures the Digital to Analog Converter 3 to output on pin RB2.
void Lab2_ConfigureDAC1(void);

// REQUIRES: DAC configured using Lab2_ConfigureDAC1 or equivalent
//           Pin RB3 set as a digital input
// PROMISES: If Pin RB3 is high, output a descending sawtooth wave. |\|\|\|\
//           If pin RB3 is low, output an ascending sawtooth wave. /|/|/|/|
void Lab2_OutputSawtooth(void);

void Lab2_writeDAC(short write_value);

#endif	/* LAB2_LIBRARY_H */

