
#include "configbits.h"
#include <xc.h>
#include "spi.h"
#include "lcd.h"



/* =========================================================================
 * main
 * ========================================================================= */
void main(void)
{
    NOP();
    // Horloge 8MHz
    OSCCON = 0b01110010;

    SPI_InitializePins();
    LCD_InitializePins();

    SPI_Initialize();
    LCD_Initialize();

    /*Effacer l'ecran */
    LCD_Clear();

    LCD_GoTo(0, 0);

    
    LCD_WriteString("Hello, World !");

    
    while (1);
}


