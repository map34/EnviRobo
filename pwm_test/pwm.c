//*****************************************************************************
//
// MSP432 main.c template - P1.0 port toggle
//
//****************************************************************************

#include "msp.h"


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    // Configure GPIO
    P2DIR |= BIT4;                            // P2.4 set TA0.1
    P2SEL0 |= BIT4;

    TA0CCR0 = 2-1;
    TA0CCTL1 = OUTMOD_7;
    TA0CCR1 = 5;
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;
    /*TA1CCR0 = 1000-1;                         // PWM Period
    TA1CCTL1 = OUTMOD_7;                      // CCR1 reset/set
    TA1CCR1 = 750;                            // CCR1 PWM duty cycle
    TA1CCTL2 = OUTMOD_7;                      // CCR2 reset/set
    TA1CCR2 = 250;                            // CCR2 PWM duty cycle
    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR;  // SMCLK, up mode, clear TAR*/

    //__sleep();
    //__no_operation();                         // For debugger
    while(1);
}
