#include "configbits.h"
#include <xc.h>

void main(void)
{
    unsigned char valeur;

    NOP();

    // Oscillateur 16 MHz
    OSCCON = 0b01111010;
    while(!OSCSTATbits.HFIOFR);

    // RA0 : entrée analogique (potentiomètre)
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;

    // RD0 : sortie PWM4 vers LED D1
    TRISDbits.TRISD0 = 1;   // entrée pendant config PWM
    ANSELDbits.ANSD0 = 0;
    LATDbits.LATD0   = 0;

    // Timer2
    TMR2 = 0;
    PR2 = 249;
    PIR1bits.TMR2IF = 0;
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

    // ADC
    ADCON2 = 0x00;
    ADCON1 = 0b00110000;
    ADCON0 = 0b00000001;

    while(1)
    {
        ADCON0bits.GO = 1;
        while(ADCON0bits.GO);

        valeur = ADRESH;

        PWM4DCH = valeur;
        PWM4DCL = 0x00;
    }
}