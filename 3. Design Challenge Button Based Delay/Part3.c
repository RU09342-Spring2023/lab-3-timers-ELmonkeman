//Kieran Murphy
//3-11-23


#include <msp430.h>

// define constants
#define LED_PIN BIT0
#define BTN1_PIN BIT3
#define BTN2_PIN BIT1

volatile unsigned int time_held_down = 0;
volatile unsigned int period = 250; // initial period of 250ms (4Hz)

void main(void) {
    // stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;

    // set up LED pin as output
    P1DIR |= LED_PIN;
    P1OUT &= ~LED_PIN;

    // set up button pins as inputs with pull-up resistors enabled
    P2DIR &= ~BTN1_PIN;
    P2REN |= BTN1_PIN;
    P2OUT |= BTN1_PIN;

    P4DIR &= ~BTN2_PIN;
    P4REN |= BTN2_PIN;
    P4OUT |= BTN2_PIN;

    // enable interrupts for button 1
    P2IE |= BTN1_PIN;
    P2IES |= BTN1_PIN;
    P2IFG &= ~BTN1_PIN;

    // enable interrupts for button 2
    P4IE |= BTN2_PIN;
    P4IES |= BTN2_PIN;
    P4IFG &= ~BTN2_PIN;

    // enable interrupts globally
    _BIS_SR(GIE);

    while (1) {
        // toggle LED state
        P1OUT ^= LED_PIN;

        // delay for current period
        __delay_cycles(1000 * period);
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
    // check if button 1 was pressed
    if (P2IFG & BTN1_PIN) {
        // clear button 1 interrupt flag
        P2IFG &= ~BTN1_PIN;

        // start counting time button is held down
        while (!(P2IN & BTN1_PIN)) {
            __delay_cycles(1000);
            time_held_down++;
        }

        // update period based on time button was held down
        period = time_held_down;

        // reset time held down
        time_held_down = 0;
    }
}

#pragma vector=PORT4_VECTOR
__interrupt void PORT4_ISR(void) {
    // check if button 2 was pressed
    if (P4IFG & BTN2_PIN) {
        // clear button 2 interrupt flag
        P4IFG &= ~BTN2_PIN;

        // reset period to default value
        period = 250;
    }
}



