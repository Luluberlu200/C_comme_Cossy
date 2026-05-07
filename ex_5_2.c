#include "configbits.h" // Bits de configuration
#include <xc.h>         // Definition des registres specifiques au uC
#include "spi.h"


void delai_timer0(void)
{
   TMR0 = 0;                    // remise a  zero du timer
   INTCONbits.TMR0IF = 0;       // remise zero   du flag

   while(INTCONbits.TMR0IF == 0);  // on attendd le débordement
}

void main(void) {
    NOP();
    // Oscillateur a 250 kHz
    OSCCON = 0b01001010;
    
    // Initialisation du Timer0
    OPTION_REGbits.T0CS = 0;   // horloge interne (Fosc/4)
    OPTION_REGbits.PSA  = 0;   // prescaler activé
    OPTION_REGbits.PS   = 0b111; // prescaler 1:256
    
    
    //Registre LEDS
    ANSELD = 0x00;         // PORTD en numérique
    TRISD  = 0x00;         // PORTD en sortie
    LATD   = 0x00;         // Init LED à 0 (éteinte)
    ANSELB = 0x00;         // PORTB en numérique
    TRISB  = 0x00;         // PORTB en sortie
    LATB   = 0x00;         // Init LED à 0 (éteinte)

    while(1)
    {
        LATD = 0x0F;
        LATB = 0x00;
        
        delai_timer0();
        
        LATD = 0xF0;
        LATB = 0x0F;
        
        delai_timer0();
        
        
    }
}


