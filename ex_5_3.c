#include "configbits.h" 
#include <xc.h>        
#include "spi.h"


void delai_timer2(void)
{
    PIR1bits.TMR2IF = 0;          // remise à zéro du flag
    TMR2 = 0;                     // remise à zéro du timer

    while(PIR1bits.TMR2IF == 0);  // attente du débordement
}


void main(void) {
    NOP();
    // Oscillateur a 8 Mhz
    OSCCON = 0b01110010;
    
    // Configuration Timer2
    T2CONbits.T2CKPS = 0b11;   // préscaler 1:64
    T2CONbits.T2OUTPS = 0b1111; // postscaler 1:16
    PR2 = 244;                 // valeur de période
    TMR2 = 0;                  // remise à zéro
    PIR1bits.TMR2IF = 0;       // remise à zéro du flag
    T2CONbits.TMR2ON = 1;      // démarrage Timer2
    
    
    //Registre LEDS
    ANSELD = 0x00;
    TRISD  = 0x00;
    LATD   = 0x00;
    ANSELB = 0x00;         // PORTB en numérique
    TRISB  = 0x00;         // PORTB en sortie
    LATB   = 0x00;         

    while(1)
    {
        LATD = 0x01;  // D1
        LATB = 0x00;
        delai_timer2();

        LATD = 0x02;  // D2
        LATB = 0x00;
        delai_timer2();

        LATD = 0x04;  // D3
        LATB = 0x00;
        delai_timer2();

        LATD = 0x08;  // D4
        LATB = 0x00;
        delai_timer2();

        LATD = 0x00;
        LATB = 0x01;  // D5
        delai_timer2();

        LATD = 0x00;
        LATB = 0x02;  // D6
        delai_timer2();

        LATD = 0x00;
        LATB = 0x04;  // D7
        delai_timer2();

        LATD = 0x00;
        LATB = 0x08;  // D8
        delai_timer2();
        
    }
}


