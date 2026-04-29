#include "configbits.h"
#include <xc.h>

#define DUTY_10_H   25
#define DUTY_10_L   0x00
#define DUTY_100_H  250
#define DUTY_100_L  0x00
#define BTN_S1  PORTBbits.RB0

void init_oscillateur(void);
void init_gpio(void);
void init_timer2(void);
void init_pwm4(void);
void set_duty(unsigned char dch, unsigned char dcl);


void main(void)
{
    NOP();  

    init_oscillateur();
    init_gpio();
    init_timer2();
    init_pwm4();

    while (1)
    {
        if (BTN_S1 == 0)
        {
            //100% 
            set_duty(DUTY_100_H, DUTY_100_L);
        }
        else
        {
            //10%
            set_duty(DUTY_10_H, DUTY_10_L);
        }
    }
}

void init_oscillateur(void)
{
    OSCCON = 0b01111010;
    while (!OSCSTATbits.HFIOFR);   /* attendre la stabilisation */
}

void init_gpio(void)
{
    /* RB0 : entree pour S1 */
    TRISBbits.TRISB0      = 1;
    ANSELBbits.ANSB0      = 0;
    WPUBbits.WPUB0        = 1;
    OPTION_REGbits.nWPUEN = 0;   /* activation globale des pull-ups */

    /* RD0 : sortie pour LED D1 / PWM4 (TRIS=1 pendant config PWM) */
    TRISDbits.TRISD0 = 1;
    ANSELDbits.ANSD0 = 0;
    LATDbits.LATD0   = 0;
}


void init_timer2(void)
{
    TMR2  = 0;
    PR2   = 249;
    PIR1bits.TMR2IF = 0;   /* effacer le flag d'overflow */
    T2CON = 0b00000110;    /* postscaler 1:1, TMR2ON=1, prescaler 1:16 */
}

void init_pwm4(void)
{

    PWM4CON = 0x00;

    PWM4DCH = DUTY_10_H;
    PWM4DCL = DUTY_10_L;

    while (!PIR1bits.TMR2IF);
    PIR1bits.TMR2IF = 0;

    TRISDbits.TRISD0 = 0;
    RD0PPS = 0x0F; 

    PWM4CON = 0b10000000;  
}


void set_duty(unsigned char dch, unsigned char dcl)
{
    PWM4DCH = dch;
    PWM4DCL = dcl;
}