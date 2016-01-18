/*
*   main.c
*   EnviRobo main controller support
*   Controls motors, UART communication, and distance sensors
*   Author: prananda0203
*/

#include "msp.h"
#include <stdio.h>
#include <stdbool.h>
#include "oled.h"
#include "i2c_protocol.h"

/*
 * Center Duty Cycle = 4490 (7.5%)
 * left wheel (as viewed from back) (P2.4)
 * right wheel (as viewed from back) (P2.5)
 *
 * 				left | right
 * Stop 		4480 | 4480
 * Forward		4800 | 4160
 * Left			4200 | 4180
 * Right		4800 | 4700
 * Backwards    4180 | 4800
 *
 * UART3 for Raspberry PI
 * UART2 for bluetooth
 */

//Function Prototypes
void setup();
void SMCLKSetUp(const uint8_t);
void UARTA2Init(int);
void UARTA3Init(int);

void manualControl();
void UARTA2SendByte(uint8_t);
void UARTA3SendByte(uint8_t);
void UARTA2SendString(unsigned char*);

//Global Variables
//volatile uint8_t okay_to_display;
volatile unsigned char rxA2;
volatile unsigned char rxA3;
bool piReceive = false;
bool btReceive = false;
int wallCount = 0;

//change when done testing
bool mControl = true;


int main(void) {

	setup();

    bool right;
    bool left;
    bool rFront;
    bool lFront;
    bool brFront;
    bool blFront;
    bool ctFront;

    bool itemHeld = false;

    while(1){

    	if(mControl) {
    		manualControl();
    	} else {

			//variables for checking whether the sensor has gone off.
			right = P2IN & BIT3;
			left = P5IN & BIT6;
			rFront = P6IN & BIT6;
			lFront = P6IN & BIT7;
			brFront = P5IN & BIT7;
			blFront = P5IN & BIT2;
			ctFront = P5IN & BIT0;

			//If center bottom front sensor goes off, stop, use image processing to determine object in front.
			//only work if no object is trapped and top sensor not triggered
			if (ctFront && !itemHeld && (!brFront || !blFront)) {
				TA0CCR1 = 4480;
				TA0CCR2 = 4480;
				i2cOLEDClear();
				i2cOLEDSendString("Scanning Environment...");

				UARTA3SendByte('g'); //send command to take pictures

				while(!piReceive);	//waits for UARTA2 to send data back
				piReceive = false;	//reset bool set from the interrupt

				if(rxA3 == 'F') {
					//trap object
					TA0CCR3 = 5000;
					TA0CCR4 = 3500;
					itemHeld = true;
					bool North = false;
					while (!North) {
						TA0CCR1 = 4480;
						TA0CCR2 = 4480;
						UARTA3SendByte('t');
						while(!piReceive);
						piReceive = false;
						if (rxA3 == 's') { //direction is North, go straight
							TA0CCR1 = 4800;
							TA0CCR2 = 4160;
							North = true;
						} else if (rxA3 == 'r') { //turn right
							TA0CCR1 = 4800;
							TA0CCR2 = 4700;
							_delay_cycles(480000); //delay for 20ms
						} else { //default turn left
							TA0CCR1 = 4200;
							TA0CCR2 = 4180;
							_delay_cycles(480000); //delay for 20ms
						}
					}
				} else { //received == "M"
					//wall or undesired object - turn until safe to move forwards
					if ((right && rFront) || !left || !lFront) {	//If right sensor is not triggered turn right
						TA0CCR1 = 4800;
						TA0CCR2 = 4700;
						_delay_cycles(1800000); //delay for 75ms
					} else if ((left && lFront) || !right || !rFront) {	//If left sensor is not triggered turn left
						TA0CCR1 = 4200;
						TA0CCR2 = 4180;
						_delay_cycles(1800000); //delay for 75ms
					} else if (!left && !right) {	//move backwards
						TA0CCR1 = 4180;
						TA0CCR2 = 4800;
						_delay_cycles(1800000); //delay for 75ms
					} else {				//If none of these cases are satisfied, something went wrong. Stop
						TA0CCR1 = 4480;
						TA0CCR2 = 4480;
						i2cOLEDClear();
						i2cOLEDSendString("Wrong Object Error!!!!!");
						while(1);
					}
				}

			} else if(!ctFront) { //if top front sensor triggered, default turn right
				i2cOLEDClear();
				i2cOLEDSendString("Moving around object");
				if (itemHeld && wallCount == 0) {
					wallCount++;
					UARTA3SendByte('w');
					while(!piReceive);
					piReceive = false;
					if (rxA3 == 'z') {
						TA0CCR1 = 4200;
						TA0CCR2 = 4180;
						_delay_cycles(6000000); //delay for 250ms
					} else {
						TA0CCR1 = 4800;
						TA0CCR2 = 4700;
						_delay_cycles(6000000); //delay for 250ms
					}
				} else if (itemHeld && wallCount == 1) {
					wallCount = 0;
					TA0CCR1 = 4480;	//stop
					TA0CCR2 = 4480;
					TA0CCR3 = 2700;	//release object
					TA0CCR4 = 7000;
					itemHeld = false;
					UARTA3SendByte('p');
					UARTA3SendByte('p');
					i2cOLEDClear();
					i2cOLEDSendString("Object Release");
				} else if ((right && rFront) || !left || !lFront) {	//If right sensor is not triggered turn right
					TA0CCR1 = 4800;
					TA0CCR2 = 4700;
					_delay_cycles(7200000); //delay for 300ms
				} else if ((left && lFront) || !right || !rFront) {	//If left sensor is not triggered turn left
					TA0CCR1 = 4200;
					TA0CCR2 = 4180;
					_delay_cycles(7200000); //delay for 300ms
				} else if ((!left && !right) || (!rFront && !lFront)) {	//move backwards
					TA0CCR1 = 4180;
					TA0CCR2 = 4800;
					_delay_cycles(7200000); //delay for 300ms
				} else {				//If none of these cases are satisfied, something went wrong. Stop
					TA0CCR1 = 4480;
					TA0CCR2 = 4480;
					i2cOLEDClear();
					i2cOLEDSendString("Front Sensor Error!!!!!");
					while(1);
				}
			} else if (!right || !rFront) {	//If right sensor is triggered turn left
				TA0CCR1 = 4200;
				TA0CCR2 = 4180;
				i2cOLEDClear();
				i2cOLEDSendString("Turning Left");
			} else if (!left || !lFront) {	//If left sensor is triggered turn right
				TA0CCR1 = 4800;
				TA0CCR2 = 4700;
				i2cOLEDClear();
				i2cOLEDSendString("Turning Right");
			} else if(ctFront || (!left && !right)){ //move forward if the front sensor isn't triggered, or both the left and right sensor are triggered
				TA0CCR1 = 4800;
				TA0CCR2 = 4160;
				i2cOLEDClear();
				i2cOLEDSendString("Moving Forward");
			} else {				//If none of these cases are satisfied, something went wrong. Stop
				TA0CCR1 = 4480;
				TA0CCR2 = 4480;
				i2cOLEDClear();
				i2cOLEDSendString("Navigation Error!!!!!!");
				while(1);
			}
		}
    }
}

//function for controlling the robot through bluetooth
void manualControl() {
	while(!btReceive);
	btReceive = false;
	if(rxA2 == 'f') { //move forwards
		TA0CCR1 = 4800;
		TA0CCR2 = 4180;
	} else if(rxA2 == 'l') { //turn left
		TA0CCR1 = 4200;
		TA0CCR2 = 4180;
	} else if(rxA2 == 'r') { //turn right
		TA0CCR1 = 4800;
		TA0CCR2 = 4700;
	} else if (rxA2 == 'b') { //move backwards
		TA0CCR1 = 4180;
		TA0CCR2 = 4800;
	} else if(rxA2 == 't') { //trap object
		TA0CCR1 = 4480;
		TA0CCR2 = 4480;
		TA0CCR3 = 5000;
		TA0CCR4 = 3500;
	} else if(rxA2 == 'y') { //release object
		TA0CCR1 = 4480;
		TA0CCR2 = 4480;
		TA0CCR3 = 2700;
		TA0CCR4 = 7000;
	} else { //stop (tell user to send 's')
		TA0CCR1 = 4480;
		TA0CCR2 = 4480;
	}
}

// UARTA2 Sending 1 string
void UARTA2SendString(unsigned char* string) {
	while(*string) {
		UARTA2SendByte(*string++);
	}
}

// UARTA2 Sending 1 byte
void UARTA2SendByte(uint8_t byte) {
	while(!(UCA2IFG & UCTXIFG));
	UCA2TXBUF = byte;
}

// UARTA3 Sending 1 byte
void UARTA3SendByte(uint8_t byte) {
	while(!(UCA3IFG & UCTXIFG));
	UCA3TXBUF = byte;
}



void setup() {
    WDTCTL = WDTPW | WDTHOLD;       // Stop WDT

    SMCLKSetUp(24);					//set system clock

    //setup for both uart ports
    UARTA2Init(115200);
    UARTA3Init(115200);

    //Setup for OLED using i2c
    i2c_init();
    i2cOLEDInitialize();
    i2cOLEDClear();
    i2cOLEDSetPos(0,0);

    /********************* Motor Setup **********************/

    // Configure GPIO
    P2DIR |= BIT4 | BIT5 | BIT6 | BIT7;					// P2.4 set TA0.1 & P2.5 set TA0.2
    P2SEL0 |= BIT4 | BIT5| BIT6 | BIT7;					// P2.6 set TA0.3 for left servo (arm), P2.7 set TA0.4 for right servo (arm)

    TA0CCR0 = 60000-1;						// PWM Period = 50Hz
    TA0CCTL1 = OUTMOD_7;					// CCR1 reset/set
    TA0CCR1 = 4480;							// Left wheel (white)
    TA0CCTL2 = OUTMOD_7;
    TA0CCR2 = 4480;							// Right wheel (brown)

    TA0CCTL3 = OUTMOD_7;
    TA0CCR3 = 2700;							// Left servo arm
    TA0CCTL4 = OUTMOD_7;
    TA0CCR4 = 7000;							// Right servo arm

    //Need to divide clock rate
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR | ID__8;  // SMCLK, up mode, clear TAR

    /************************** Sensor Setup ************************/
    P2DIR &= ~(uint8_t) BIT3;
	P2OUT |= BIT3;
	P2REN |= BIT3;							// Enable pull-up resistor (P2.3 output high)

	P5DIR &= ~(uint8_t) BIT0 & ~(uint8_t) BIT2 & ~(uint8_t) BIT6 & ~(uint8_t) BIT7;
	P5OUT |= BIT0 | BIT2 | BIT6 | BIT7;
	P5REN |= BIT0 | BIT2 | BIT6 | BIT7;

	P6DIR &= ~(uint8_t) BIT6 & ~(uint8_t) BIT7;
	P6OUT |= BIT6 | BIT7;
	P6REN |= BIT6 | BIT7;

	/*
	 * Add 2 more distance sensors
	 *
	 */

	//__enable_interrupt();			May be needed later

    /******************* Terminate all other pins ********************/
    P4DIR |= 0xFF; P4OUT = 0;
    P7DIR |= 0xFF; P7OUT = 0;
    P8DIR |= 0xFF; P8OUT = 0;
    P10DIR |= 0xFF; P10OUT = 0;

	/**************************************************************/
}

// Set a system clock SMCLK of 3,6,12,24, and 48 (MHz)
void SMCLKSetUp(const uint8_t freq) {
	CSKEY = CSKEY_VAL;		// Unlock Clock Select Register
	CSCTL0 = 0;
	// Frequency in MHz
	switch(freq) {
		case 3:
			CSCTL0 = DCORSEL_1;
			break;
		case 6:
			CSCTL0 = DCORSEL_2;
			break;
		case 12:
			CSCTL0 = DCORSEL_3;
			break;
		case 24:
			CSCTL0 = DCORSEL_4;
			break;
		case 48:
			CSCTL0 = DCORSEL_5;
			break;
		default:
			CSCTL0 = DCORSEL_1;
			break;
	}
	// ACLK = REFO, SMCLK = MCLK = DCO
	CSCTL1 = SELA_2 | SELS_3 | SELM_3;
	CSKEY = 0;				// Lock Clock Select Register
}

// UARTA2 Initialization
void UARTA2Init(int baud_rate) {
	// 3.2 RX, 3.3 TX
	P3SEL0 |= (BIT3 | BIT2);
	P3SEL1 &= ~(BIT3 | BIT2);

	NVIC_ISER0 = 1 << ((INT_EUSCIA2 - 16) & 31);	// Enable EUSCIA2 interrupt in NVIC module

	// Configuring UARTA2
	UCA2CTLW0 |= UCSWRST;
	UCA2CTLW0 |= UCSSEL__SMCLK;             // Put eUSCI in reset
	// Baud Rate calculation
	switch(baud_rate) {
		case 115200:
			UCA2BR0 = 13;						// 24000000/16/115200 -> 13.0283
			UCA2BR1 = 0x00;						// Fractional portion = 0.0283
			UCA2MCTLW = 0x0000 | UCOS16;		// User's Guide Table 21-4: UCBRSx = 0x00
			break;								// UCBRFx = int((13.0283-13)*16) = 0
		case 9600:
			UCA2BR0 = 156;							// 24000000/16/9600 -> 156.25
			UCA2BR1 = 0x00;							// Fractional portion = 0.25
			UCA2MCTLW = 0x4400 | UCOS16 | 0x0040;	// User's Guide Table 21-4: UCBRSx = 0x44
			break;									// UCBRFx = int ( (156.25-156)*16) = 4
		default:
			UCA2BR0 = 13;						// 24000000/16/115200 -> 13.0283
			UCA2BR1 = 0x00;						// Fractional portion = 0.0283
			UCA2MCTLW = 0x0000 | UCOS16;		// User's Guide Table 21-4: UCBRSx = 0x00
			break;								// UCBRFx = int((13.0283-13)*16) = 0
	}

	UCA2CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
	UCA2IE |= UCRXIE;                       // Enable USCI_A2 RX interrupt
}


// UARTA3 Initialization
void UARTA3Init(int baud_rate) {
	// 9.6 RX, 9.7 TX
	P9SEL0 |= (BIT6 | BIT7);
	P9SEL1 &= ~(BIT6 | BIT7);

	NVIC_ISER0 = 1 << ((INT_EUSCIA3 - 16) & 31);	// Enable EUSCIA2 interrupt in NVIC module

	// Configuring UARTA3
	UCA3CTLW0 |= UCSWRST;
	UCA3CTLW0 |= UCSSEL__SMCLK;             // Put eUSCI in reset
	// Baud Rate calculation
	switch(baud_rate) {
		case 115200:
			UCA3BR0 = 13;					// 24000000/16/115200 -> 13.0283
			UCA3BR1 = 0x00;					// Fractional portion = 0.0283
			UCA3MCTLW = 0x0000 | UCOS16;	// User's Guide Table 21-4: UCBRSx = 0x00
			break;							// UCBRFx = int((13.0283-13)*16) = 0
		case 9600:
			UCA3BR0 = 156;							// 24000000/16/9600 -> 156.25
			UCA3BR1 = 0x00;							// Fractional portion = 0.25
			UCA3MCTLW = 0x4400 | UCOS16 | 0x0040;	// User's Guide Table 21-4: UCBRSx = 0x44
			break;									// UCBRFx = int ( (156.25-156)*16) = 4
		default:
			UCA3BR0 = 13;					// 24000000/16/115200 -> 13.0283
			UCA3BR1 = 0x00;					// Fractional portion = 0.0283
			UCA3MCTLW = 0x0000 | UCOS16;	// User's Guide Table 21-4: UCBRSx = 0x00
			break;							// UCBRFx = int((13.0283-13)*16) = 0
	}

	UCA3CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
	UCA3IE |= UCRXIE;                       // Enable USCI_A2 RX interrupt
}

// UART2 interrupt service routine
void eUSCIA2IsrHandler(void) {
	// RX interrupt
    if (UCA2IFG & UCRXIFG) {
    	rxA2 = UCA2RXBUF;
    	if (rxA2 == 'm'){
    		mControl = true;
    	} else if (rxA2 == 'a') {
    		mControl = false;
    	}
    	btReceive = true;
    }
}

// UART3 interrupt service routine
void eUSCIA3IsrHandler(void) {
	// RX interrupt
    if (UCA3IFG & UCRXIFG) {
    	rxA3 = UCA3RXBUF;
    	piReceive = true;
    }
}
