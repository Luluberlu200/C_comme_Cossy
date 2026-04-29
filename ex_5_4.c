#include "configbits.h" 
#include <xc.h>        
#include "spi.h"

unsigned char led = 0x01;


void __interrupt() isr(void)
{
    if (PIR1bits.TMR2IF == 1)
    {
        PIR1bits.TMR2IF = 0;   // remise à zéro du flag

      
        if (led <= 0x08)
        {
            LATD = led;     // D1 à D4 sur PORTD
            LATB = 0x00;
        }
        else
        {
            LATD = 0x00;
            LATB = led >> 4;   // D5 à D8 sur PORTB
        }

        // LED suivante
        led = led << 1;

        if (led == 0x00)
        {
            led = 0x01;
        }
    }
}



void main(void) {
    NOP();

    // Oscillateur interne à 8 MHz
    OSCCON = 0b01111010;

    // LEDs en sortie numérique
    ANSELD = 0x00;
    TRISD  = 0x00;
    LATD   = 0x00;

    ANSELB = 0x00;
    TRISB  = 0x00;
    LATB   = 0x00;

    // Configuration Timer2
    T2CONbits.T2CKPS = 0b11;      // prescaler 1:64
    T2CONbits.T2OUTPS = 0b1111;   // postscaler 1:16
    PR2 = 244;                    // période ~125 ms
    TMR2 = 0;
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;          // autorise interruption Timer2
    INTCONbits.PEIE = 1;          // interruptions périphériques
    INTCONbits.GIE  = 1;          // interruptions globales
    T2CONbits.TMR2ON = 1;         // démarrage Timer2

    // État initial : D1 allumée
    LATD = 0x01;
    LATB = 0x00;

    while(1)
    {
  
    }
}


