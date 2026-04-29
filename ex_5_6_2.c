#include "configbits.h"
#include <xc.h>

volatile unsigned char duty = 0;
volatile unsigned char sens = 1;
volatile unsigned char diviseur = 0;

void __interrupt() isr(void)
{
    if(PIR1bits.TMR2IF)
    {
        PIR1bits.TMR2IF = 0;

        diviseur++;
        if(diviseur < 8)
            return;

        diviseur = 0;

        if(sens)
        {
            duty++;
            if(duty == 255)
                sens = 0;
        }
        else
        {
            duty--;
            if(duty == 0)
                sens = 1;
        }

        PWM4DCH = duty;
        PWM4DCL = 0x00;
    }
}

void main(void)
{
    NOP();

    // Oscillateur 16 MHz
    OSCCON = 0b01111010;
    while(!OSCSTATbits.HFIOFR);

    // RD0 = sortie PWM4 (LED D1)
    TRISDbits.TRISD0 = 1;
    ANSELDbits.ANSD0 = 0;
    LATDbits.LATD0 = 0;

    // Timer2
    TMR2 = 0;
    PR2 = 249;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
    T2CON = 0b00000110;

    // PWM4
    PWM4CON = 0x00;
    PWM4DCH = 0;
    PWM4DCL = 0x00;

    while(!PIR1bits.TMR2IF);
    PIR1bits.TMR2IF = 0;

    TRISDbits.TRISD0 = 0;
    RD0PPS = 0x0F;
    PWM4CON = 0b10000000;

    // Interruptions
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    while(1)
    {
       
    }
}