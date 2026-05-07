#include "configbits.h"
#include <xc.h>


void init_uart(void);
void uart_send_char(unsigned char c);
void uart_send_string(const char *str);
void __interrupt() isr(void);

/* =========================================================================
 * main
 * ========================================================================= */
void main(void)
{
    NOP();

    OSCCON = 0b01111010;

    init_uart();

    while (1);
}



void init_uart(void)
{
    /* PPS */
    RC6PPS = 0x14;
    RXPPS  = 0x17;

    /* GPIO */
    TRISCbits.TRISC6  = 0;
    ANSELCbits.ANSC6  = 0;
    LATCbits.LATC6    = 1;

    TRISCbits.TRISC7  = 1;
    ANSELCbits.ANSC7  = 0;

    /* Baudrate 9600 */
    SP1BRGH = 1;
    SP1BRGL = 0xA0;

    /* Activation EUSART */
    TX1STA   = 0b00100100;
    BAUD1CON = 0b00001000;
    RC1STA   = 0b10010000;

    /* Interruptions reception */
    PIE1bits.RCIE   = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE  = 1;
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
        uart_send_char(*str);
        str++;
    }
}

void __interrupt() isr(void)
{
    unsigned char caractere;

    if (PIR1bits.RCIF)
    {
        /* Gestion overrun */
        if (RC1STAbits.OERR)
        {
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
        }

        /* Lire le caractere recu (efface RCIF) */
        caractere = RC1REG;

        /* Renvoyer le meme caractere */
        uart_send_char(caractere);
    }
}
