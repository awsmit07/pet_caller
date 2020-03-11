#include <pic.h>
#include "Lab2_Library.h"

// REQUIRES: Nothing
// PROMISES: Returns 500*(1+sin(2*pi*x/100)), where x is the number of times 
//           SineArray has been called before. The first three return values are
//           500*(1+sin(0*pi)), 500*(1+sin(0.02*pi)), 500*(1+sin(0.04*pi)), etc.
short SineArray(void);

void main(void)
{
    // Set the system clock speed to 32MHz.
    OSCCON = 0xF4;
    int index = 0;
    char period[] = {45, 43, 40, 38, 36, 34, 32, 30, 29, 27, 26, 24, 23};
    //Wait for the clock switch until the "HF Oscillator Ready" flag is true.
    while(OSCSTATbits.HFIOFR == 0);
    
    // Configure pin RC6 as a digital input, to read the button.
    TRISCbits.TRISC6 = 1;
    ANSELCbits.ANSC6 = 0;
        
    //Configure DAC1. Replace this with your own code in Exercise A.
    //Lab2_ConfigureDAC1();
    TRISAbits.TRISA2 = 0;
    ANSELAbits.ANSA2 = 1;
    
    DAC1CON0 = 0xB0;    //0b01000000
    //DACLD = 0x01;
    //DAC1REFH = 0x00;
    //DAC1REFL = 0x00;
    //Configure Timer2 to have a period of 100 microseconds.
    //Calling Lab2_ConfigureTimer2 again can set a new period.
    Lab2_ConfigureTimer2(period[index++]);
    int swi = 1;
    //This loop runs based on Timer2 rather than __delay_ms or __delay_us
    
    /*PIC User Manual Chapter 13: Interrupts
     This is our configuration to set up RC6 as a interrupt on rising edge logic*/
    INTCONbits.IOCIE = 1; //Tells Processor to allow for interrupts to occur.
    IOCCPbits.IOCCP6 = 1; //Assigns RC6 to interrupt on rising edge.
    
    while(1)
    {
        //DACLD = 0x01;
        
        if(IOCCFbits.IOCCF6 == 1 && swi == 1) // If flag is true and switch is set to off
        {
            IOCCFbits.IOCCF6 = 0; // Removes flag
            Lab2_ConfigureTimer2(period[index++]);
            swi = 0;
        }
        else if(IOCCFbits.IOCCF6 == 1 && swi == 0) // If flag is true and switch is set to on
        {
            IOCCFbits.IOCCF6 = 0; // Removes flag
            Lab2_ConfigureTimer2(period[index++]);
            swi = 1;
        }
        index = index % 13;
        
        // Call the SineArray function to make sure it runs quickly
        // enough not to stall the DAC output. Use this in Exercise B.
        short x = SineArray();
        
        // Output an ascending sawtooth wave, or a descending sawtooth wave if
        // pin RB6 is high. Replace this in Exercise B.
        //Lab2_OutputSawtooth(); 
        Lab2_writeDAC(x);
        
//        __delay_ms(10);
//        Lab2_writeDAC(0);
//        __delay_ms(10);
        
        // Wait until the timer has expired by checking the interrupt flag.
        while(!PIR1bits.TMR2IF);
        
        // Clear the interrupt flag. The timer restarts automatically.
        PIR1bits.TMR2IF = 0;
    }
        
    return;
}

// REQUIRES: Nothing
// PROMISES: Returns 500*(1+sin(2*pi*x/100)), where x is the number of times 
//           SineArray has been called before. The first three return values are
//           500*(1+sin(0*pi)), 500*(1+sin(0.02*pi)), 500*(1+sin(0.04*pi)), etc.
short SineArray(void)
{
    // Declare an array of 500*(1+sin(2*pi*x/100)) for x = 0:99
    const short Array[] = 
        {500, 531, 563, 594, 624, 655, 684, 713, 741, 768,
         794, 819, 842, 864, 885, 905, 922, 938, 952, 965, 
         976, 984, 991, 996, 999,1000, 999, 996, 991, 984,
         976, 965, 952, 938, 922, 905, 885, 864, 842, 819,
         794, 768, 741, 713, 684, 655, 624, 594, 563, 531,
         500, 469, 437, 406, 376, 345, 316, 287, 259, 232,
         206, 181, 158, 136, 115,  95,  78,  62,  48,  35,
          24,  16,   9,   4,   1,   0,   1,   4,   9,  16, 
          24,  35,  48,  62,  78,  95, 115, 136, 158, 181, 
         206, 232, 259, 287, 316, 345, 376, 406, 437, 469};

    // Declare an index to track which array element to read next.
    // ArrayIndex is declared as a static variable. It is only initialized the
    // first time SineArray is called. In all later calls to SineArray, this
    // line will be skipped and ArrayIndex will keep its previous value.
    static char ArrayIndex = 0;
    
    // If ArrayIndex exceeds the size Array, reset it to zero.
    if (ArrayIndex == 100) ArrayIndex = 0;
    
    // Return the Array pointed to by ArrayIndex, and then increment ArrayIndex.
    return Array[ArrayIndex++];
}

/* lab stuff
    Q1
 * 2.5456 Volts
 * 
 * Q4
 * PCL: 0xE5 then 0xE5
 * BSR: 0x0B then 0x0B then 0x00
 * WREG: 0xF4 then 0xF4 then 0x3B
 * TRISA: 0xFF then 0xFF
 * DAC1CON0: 0xA0 then 0xA0
 * 
 * SAW TOOTH VALUE:
 * measured values:
 * 8mV per division
 * 10kHz per division
 * 
 * calculated average:
 * 9.44kHz per division
 * 4.88mV per division
 * 
 * 
 * 
 */

void Lab2_writeDAC(short write_value)
{
    DAC1REFH = write_value >> 8;
    DAC1REFL = write_value;
    DACLD = 0x01;
}
