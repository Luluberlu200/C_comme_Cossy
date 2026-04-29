#include "configbits.h"
#include <xc.h>

void main(void)
{
    unsigned int i;
    unsigned char valeur;
    unsigned char index;

    NOP();

    // Oscillateur 16 MHz
    OSCCON = 0b01111010;
    while(!OSCSTATbits.HFIOFR);

    // RA0 en entrée analogique (potentiomètre)
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;

    // LEDs RD0-RD3
    TRISD = 0xF0;
    ANSELD = 0x00;
    LATD = 0x00;

    // LEDs RB0-RB3
    TRISB = 0xF0;
    ANSELB = 0x00;
    LATB = 0x00;

    // ADC AN0
    ADCON2 = 0x00;
    ADCON1 = 0b00110000;
    ADCON0 = 0b00000001;

    while(1)
    {
     

        // Lancer conversion
        ADCON0bits.GO = 1;

        // Attendre fin
        while(ADCON0bits.GO);

        // Lire valeur
        valeur = ADRESH;

        // Convertir en index 0-7
        index = valeur >> 5;

        // Éteindre toutes les LEDs
        LATD = 0x00;
        LATB = 0x00;

        // Allumer une LED
        if(index < 4)
        {
            LATD = 1 << index;
        }
        else
        {
            LATB = 1 << (index - 4);
        }
    }
}