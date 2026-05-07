
#include "configbits.h"
#include <xc.h>

/* -------------------------------------------------------------------------
 * Prototypes
 * ------------------------------------------------------------------------- */
void init_oscillateur(void);
void init_uart(void);
void uart_send_char(unsigned char c);
void uart_send_string(const char *str);

/* =========================================================================
 * main
 * ========================================================================= */
void main(void)
{
    NOP();

    init_oscillateur();
    init_uart();

    while (1)
    {
        uart_send_string("Hello, World !\n\r");
    }
}   

/* =========================================================================
 * Initialisation oscillateur interne 16 MHz
 * ========================================================================= */
void init_oscillateur(void)
{
    OSCCON = 0b01111010;   /* IRCF=1111 (16MHz), SCS=10 (interne) */
    while (!OSCSTATbits.HFIOFR);
}

void init_uart(void)
{
    /* Etape 1 : PPS */
    RC6PPS = 0x14;
    RXPPS  = 0x17;

    /* Etape 2 : GPIO */
    TRISCbits.TRISC6 = 0;
    ANSELCbits.ANSC6 = 0;
    LATCbits.LATC6   = 1;

    TRISCbits.TRISC7 = 1;
    ANSELCbits.ANSC7 = 0;

    /* Etape 3 : Baudrate */
    SP1BRGH = 1;
    SP1BRGL = 0xA0;

    /* Etape 4 : Activation */
    TX1STA   = 0b00100100;
    BAUD1CON = 0b00001000;
    RC1STA   = 0b10010000;
}

void uart_send_char(unsigned char c)
{
    while (!PIR1bits.TXIF);
    TX1REG = c;
}

void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_char((unsigned char)*str);
        str++;
    }
}