#include "configbits.h" // Bits de configuration
#include <xc.h>         // Definition des registres specifiques au uC
#include "spi.h"


void delai_approx(void)
{
  unsigned int i, j;

    for(i = 0; i < 1000; i++)
    {
        for(j = 0; j < 500; j++)
        {
            NOP();
        }
    }
}

void main(void) {
    NOP();

    // Horloge 8MHz
    OSCCON = 0b01110010;

    ANSELD = 0x00;
    TRISD  = 0x00;
    LATD   = 0x00;
    
    ANSELB = 0x00;         // PORTB en numérique
    TRISB  = 0x00;         // PORTB en sortie
    LATB   = 0x00;         

    while(1)
    {
        LATD = 0x0F;
        LATB = 0x00;
        
        delai_approx();
        
        LATD = 0xF0;
        LATB = 0x0F;
        
        delai_approx();
        
        
    }
}


