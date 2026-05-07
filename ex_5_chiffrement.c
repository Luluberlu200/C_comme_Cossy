/*
 * TP_cesar.c
 * Exercice 5.9 - Chiffrement/Dechiffrement de code Cesar
 * Carte Explorer 8 - JUNIA ISEN
 *
 * Fonctionnement :
 *   (a) L'utilisateur entre un message sur le terminal serie (Tera Term)
 *   (b) Le message chiffre/dechiffre est renvoye caractere par caractere
 *   (c) Le potentiometre change la valeur du decalage (offset)
 *   (d) Le bouton S1 (RB0) commute entre chiffrement et dechiffrement
 *   (e) L'ecran LCD affiche l'offset et le mode en cours
 *   (f) Alphabet : 0123456789abcdefghijklmnopqrstuvwxyz (36 caracteres)
 *   (g) Majuscules A-Z interpretees comme minuscules a-z
 *   (h) Caracteres hors alphabet ignores
 *
 * Corrections v2 :
 *   - Nop() remplace par __builtin_nop() (compatible C99 strict XC8)
 *   - chercher_index retourne signed char pour permettre la valeur -1
 *   - cesar_chiffrer / cesar_dechiffrer utilisent signed char pour idx
 */

#include "configbits.h"
#include <xc.h>
#include "spi.h"
#include "lcd.h"

/* -------------------------------------------------------------------------
 * Constantes
 * ------------------------------------------------------------------------- */
#define ALPHABET_SIZE  36
#define BTN_S1         PORTBbits.RB0

/* Alphabet gere par l'application */
const char alphabet[ALPHABET_SIZE] = "0123456789abcdefghijklmnopqrstuvwxyz";

/* -------------------------------------------------------------------------
 * Variables globales
 * ------------------------------------------------------------------------- */
volatile unsigned char mode_chiffrement = 1;  /* 1=chiffrement, 0=dechiffrement */
volatile unsigned char offset = 0;            /* decalage Cesar (0..35)          */

/* -------------------------------------------------------------------------
 * Prototypes
 * ------------------------------------------------------------------------- */
void init_oscillateur(void);
void init_gpio(void);
void init_uart(void);
void init_adc(void);
void init_lcd(void);
void init_interruptions(void);

void uart_send_char(unsigned char c);
void uart_send_string(const char *str);
unsigned char lire_adc(void);
void mettre_a_jour_offset(void);
void mettre_a_jour_lcd(void);

signed char chercher_index(char c);
char cesar_chiffrer(char c);
char cesar_dechiffrer(char c);

void __interrupt() isr(void);

/* =========================================================================
 * main
 * ========================================================================= */
void main(void)
{
    NOP();

    init_oscillateur();
    init_gpio();
    init_uart();
    init_adc();
    init_lcd();
    init_interruptions();

    mettre_a_jour_offset();
    mettre_a_jour_lcd();

    while (1)
    {
        mettre_a_jour_offset();
        mettre_a_jour_lcd();
    }
}

/* =========================================================================
 * Initialisation oscillateur interne 16 MHz
 * ========================================================================= */
void init_oscillateur(void)
{
    OSCCON = 0b01111010;
    while (!OSCSTATbits.HFIOFR);
}

/* =========================================================================
 * Initialisation GPIO
 * ========================================================================= */
void init_gpio(void)
{
    /* RB0 : entree S1 avec pull-up */
    TRISBbits.TRISB0      = 1;
    ANSELBbits.ANSB0      = 0;
    WPUBbits.WPUB0        = 1;
    OPTION_REGbits.nWPUEN = 0;

    /* RA0 : entree analogique (potentiometre) */
    TRISAbits.TRISA0 = 1;
    ANSELAbits.ANSA0 = 1;
    WPUAbits.WPUA0   = 0;
}

/* =========================================================================
 * Initialisation EUSART 9600 bauds
 * ========================================================================= */
void init_uart(void)
{
    RC6PPS = 0x14;
    RXPPS  = 0x17;

    TRISCbits.TRISC6 = 0;
    ANSELCbits.ANSC6 = 0;
    LATCbits.LATC6   = 1;

    TRISCbits.TRISC7 = 1;
    ANSELCbits.ANSC7 = 0;

    SP1BRGH  = 1;
    SP1BRGL  = 0xA0;

    TX1STA   = 0b00100100;
    BAUD1CON = 0b00001000;
    RC1STA   = 0b10010000;
}

/* =========================================================================
 * Initialisation ADC
 * ========================================================================= */
void init_adc(void)
{
    ADCON2 = 0x00;
    ADCON1 = 0b00110000;
    ADCON0 = 0b00000001;
}

/* =========================================================================
 * Initialisation LCD
 * ========================================================================= */
void init_lcd(void)
{
    SPI_InitializePins();
    LCD_InitializePins();
    SPI_Initialize();
    LCD_Initialize();
    LCD_Clear();
}

/* =========================================================================
 * Activation des interruptions
 * ========================================================================= */
void init_interruptions(void)
{
    PIE1bits.RCIE    = 1;   /* reception UART   */
    IOCBNbits.IOCBN0 = 1;   /* front desc. RB0  */
    INTCONbits.IOCIE = 1;   /* interrupt on change */
    INTCONbits.PEIE  = 1;
    INTCONbits.GIE   = 1;
}

/* =========================================================================
 * Envoi d'un caractere sur l'UART
 * ========================================================================= */
void uart_send_char(unsigned char c)
{
    while (!PIR1bits.TXIF);
    TX1REG = c;
}

/* =========================================================================
 * Envoi d'une chaine sur l'UART
 * ========================================================================= */
void uart_send_string(const char *str)
{
    while (*str != '\0')
    {
        uart_send_char((unsigned char)*str);
        str++;
    }
}

/* =========================================================================
 * Lecture ADC
 * ========================================================================= */
unsigned char lire_adc(void)
{
    unsigned int i;
    for (i = 0; i < 320; i++) { NOP(); }  /* 20 TAD acquisition */
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    return ADRESH;
}

/* =========================================================================
 * Mise a jour de l'offset depuis le potentiometre
 * offset = (ADRESH * 35) / 255  => plage 0..35
 * ========================================================================= */
void mettre_a_jour_offset(void)
{
    unsigned char adc = lire_adc();
    offset = (unsigned char)(((unsigned int)adc * 35U) / 255U);
}

/* =========================================================================
 * Mise a jour LCD
 * Ligne 0 : mode
 * Ligne 1 : offset
 * ========================================================================= */
void mettre_a_jour_lcd(void)
{
    LCD_GoTo(0, 0);
    if (mode_chiffrement)
    {
        LCD_WriteString("Mode: CHIFFRE  ");
    }
    else
    {
        LCD_WriteString("Mode: DECHIFFRE");
    }

    LCD_GoTo(1, 0);
    LCD_WriteString("Offset: ");
    LCD_WriteByte((char)('0' + offset / 10));
    LCD_WriteByte((char)('0' + offset % 10));
    LCD_WriteString("       ");
}

/* =========================================================================
 * Chercher l'index d'un caractere dans l'alphabet
 * Retourne -1 (signed char) si non trouve
 * ========================================================================= */
signed char chercher_index(char c)
{
    unsigned char i;
    for (i = 0; i < ALPHABET_SIZE; i++)
    {
        if (alphabet[i] == c)
        {
            return (signed char)i;
        }
    }
    return (signed char)-1;
}

/* =========================================================================
 * Chiffrement Cesar
 * ========================================================================= */
char cesar_chiffrer(char c)
{
    signed char idx = chercher_index(c);
    if (idx == (signed char)-1) return (char)0;
    return alphabet[((unsigned char)idx + offset) % ALPHABET_SIZE];
}

/* =========================================================================
 * Dechiffrement Cesar
 * ========================================================================= */
char cesar_dechiffrer(char c)
{
    signed char idx = chercher_index(c);
    if (idx == (signed char)-1) return (char)0;
    return alphabet[((unsigned char)idx + ALPHABET_SIZE - offset) % ALPHABET_SIZE];
}

/* =========================================================================
 * ISR
 * ========================================================================= */
void __interrupt() isr(void)
{
    char c;
    char resultat;

    /* Interruption reception UART */
    if (PIR1bits.RCIF)
    {
        if (RC1STAbits.OERR)
        {
            RC1STAbits.CREN = 0;
            RC1STAbits.CREN = 1;
        }

        c = (char)RC1REG;

        /* Majuscule -> minuscule */
        if (c >= 'A' && c <= 'Z')
        {
            c = (char)(c + ('a' - 'A'));
        }

        /* Chiffrer ou dechiffrer */
        if (mode_chiffrement)
        {
            resultat = cesar_chiffrer(c);
        }
        else
        {
            resultat = cesar_dechiffrer(c);
        }

        /* Envoyer si caractere valide */
        if (resultat != (char)0)
        {
            uart_send_char((unsigned char)resultat);
        }
    }

    /* Interruption bouton S1 (IOC RB0) */
    if (INTCONbits.IOCIF)
    {
        if (IOCBFbits.IOCBF0)
        {
            mode_chiffrement = !mode_chiffrement;
            IOCBFbits.IOCBF0 = 0;
        }
        INTCONbits.IOCIF = 0;
    }
}