//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "./oled.h"
#include "./accel.h"
#include "./i2c_protocol.h"
#include "./compass.h"
#include <stdlib.h>
#include <string.h>

void SMCLKSetUp(const uint8_t freq);
void UARTA2Init(int baud_rate);
void UARTA3Init(int baud_rate);
void UARTA2SendByte(uint8_t byte);
void UARTA2SendString(unsigned char* string);
void UARTA3SendByte(uint8_t byte);
void UARTA3SendString(unsigned char* string);
void BTInit();
void itoa(int num, unsigned char* num_string);
void reverse(unsigned char* s);

volatile uint8_t okay_to_display;
volatile unsigned char rxA2;
volatile unsigned char rxA3;
uint8_t read_byte = 0x00;

unsigned char x_str[10] = {'\0'};
unsigned char y_str[10] = {'\0'};
unsigned char z_str[10] = {'\0'};
int x = 0;
int y = 0;
int z = 0;

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    SMCLKSetUp(24);

    UARTA2Init(9600);
    UARTA3Init(115200);
    __enable_interrupt();
    i2c_init();

    i2cOLEDInitialize();
    i2cOLEDClear();
    i2cOLEDSetPos(0,0);

    //accelTurnOn();
    //read_byte = i2c_read_reg(ADXL345_ADDRESS, ADXL345_POWER_CTL);
    setCompassMeasurementMode(Measurement_Continuous);
    setCompassScale('1');

    while(1)
    {
    	/*if(okay_to_display)
    	{	if (rxA3 == '.')
    		{
    			i2cOLEDClear();
    			i2cOLEDSetPos(0,0);
    		}
    		else
    		{
    			i2cOLEDSendChar(rxA3);
    		}

    		okay_to_display = 0;
    	}*/
    	//UARTA3SendByte('h');
    	readCompassRaw(&x, &y, &z);
    	itoa(x, x_str);
    	itoa(y, y_str);
    	//itoa(z, z_str);
    	i2cOLEDClear();
    	i2cOLEDSetPos(0,0);
    	i2cOLEDSendString("x: ");
    	i2cOLEDSendString(x_str);
    	i2cOLEDSetPos(0,1);
    	i2cOLEDSendString("y: ");
    	i2cOLEDSendString(y_str);
    	__delay_cycles(2500000);

    }
	
}

/**
 * Initializing Bluetooth Sequence
 */
void BTInit()
{
		UARTA2SendByte('$');
	    UARTA2SendByte('$');
	    UARTA2SendByte('$');

	    __delay_cycles(100);

	    UARTA2SendByte('S');
	    UARTA2SendByte('M');
	    UARTA2SendByte(',');
	    UARTA2SendByte('0');
	    UARTA2SendByte('\r');
	    UARTA2SendByte('\n');


	    UARTA2SendByte('S');
		UARTA2SendByte('~');
		UARTA2SendByte(',');
		UARTA2SendByte('0');
		UARTA2SendByte('\r');
		UARTA2SendByte('\n');


		UARTA2SendByte('S');
		UARTA2SendByte('A');
		UARTA2SendByte(',');
		UARTA2SendByte('0');
		UARTA2SendByte('\r');
		UARTA2SendByte('\n');


		UARTA2SendByte('R');
		UARTA2SendByte(',');
		UARTA2SendByte('1');
		UARTA2SendByte('\r');
		UARTA2SendByte('\n');

		__delay_cycles(100);

		UARTA2SendByte('-');
		UARTA2SendByte('-');
		UARTA2SendByte('-');
		UARTA2SendByte('\r');
		UARTA2SendByte('\n');
}

/*
 * UARTA2 Initialization
 */
void UARTA2Init(int baud_rate)
{	// 3.2 RX, 3.3 TX
	P3SEL0 |= (BIT3 | BIT2);
	P3SEL1 &= ~(BIT3 | BIT2);

	NVIC_ISER0 = 1 << ((INT_EUSCIA2 - 16) & 31);	// Enable EUSCIA2 interrupt in NVIC module

	// Configuring UARTA2
	UCA2CTLW0 |= UCSWRST;
	UCA2CTLW0 |= UCSSEL__SMCLK;             // Put eUSCI in reset
	// Baud Rate calculation
	switch(baud_rate)
	{
		case 115200:
			UCA2BR0 = 13;
			UCA2BR1 = 0x00;
													// 24000000/16/115200 -> 13.0283
													// Fractional portion = 0.0283
													// User's Guide Table 21-4: UCBRSx = 0x00
													// UCBRFx = int((13.0283-13)*16) = 0
			UCA2MCTLW = 0x0000 | UCOS16;
			break;
		case 9600:
			UCA2BR0 = 156;
			UCA2BR1 = 0x00;
													// 24000000/16/9600 -> 156.25
													// Fractional portion = 0.25
													// User's Guide Table 21-4: UCBRSx = 0x44
			UCA2MCTLW = 0x4400 | UCOS16 | 0x0040;	// UCBRFx = int ( (156.25-156)*16) = 4
			break;
		default:
			UCA2BR0 = 13;
			UCA2BR1 = 0x00;
													// 24000000/16/115200 -> 13.0283
													// Fractional portion = 0.0283
													// User's Guide Table 21-4: UCBRSx = 0x00
													// UCBRFx = int((13.0283-13)*16) = 0
			UCA2MCTLW = 0x0000 | UCOS16;
			break;
	}


	UCA2CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
	UCA2IE |= UCRXIE;                       // Enable USCI_A2 RX interrupt

}


/*
 * UARTA3 Initialization
 */
void UARTA3Init(int baud_rate)
{	// 9.6 RX, 9.7 TX
	P9SEL0 |= (BIT6 | BIT7);
	P9SEL1 &= ~(BIT6 | BIT7);

	NVIC_ISER0 = 1 << ((INT_EUSCIA3 - 16) & 31);	// Enable EUSCIA2 interrupt in NVIC module

	// Configuring UARTA3
	UCA3CTLW0 |= UCSWRST;
	UCA3CTLW0 |= UCSSEL__SMCLK;             // Put eUSCI in reset
	// Baud Rate calculation
	switch(baud_rate)
	{
		case 115200:
			UCA3BR0 = 13;
			UCA3BR1 = 0x00;
													// 24000000/16/115200 -> 13.0283
													// Fractional portion = 0.0283
													// User's Guide Table 21-4: UCBRSx = 0x00
													// UCBRFx = int((13.0283-13)*16) = 0
			UCA3MCTLW = 0x0000 | UCOS16;
			break;
		case 9600:
			UCA3BR0 = 156;
			UCA3BR1 = 0x00;
													// 24000000/16/9600 -> 156.25
													// Fractional portion = 0.25
													// User's Guide Table 21-4: UCBRSx = 0x44
			UCA3MCTLW = 0x4400 | UCOS16 | 0x0040;	// UCBRFx = int ( (156.25-156)*16) = 4
			break;
		default:
			UCA3BR0 = 13;
			UCA3BR1 = 0x00;
													// 24000000/16/115200 -> 13.0283
													// Fractional portion = 0.0283
													// User's Guide Table 21-4: UCBRSx = 0x00
													// UCBRFx = int((13.0283-13)*16) = 0
			UCA3MCTLW = 0x0000 | UCOS16;
			break;
	}


	UCA3CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
	UCA3IE |= UCRXIE;                       // Enable USCI_A2 RX interrupt

}

/*
 * Set a system clock SMCLK of 3,6,12,24, and 48 (MHz)
 */
void SMCLKSetUp(const uint8_t freq)
{
	CSKEY = CSKEY_VAL;		// Unlock Clock Select Register
	CSCTL0 = 0;
	// Frequency in MHz
	switch(freq)
	{
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

// UART2 interrupt service routine
void eUSCIA2IsrHandler(void)
{
	// RX interrupt
    if (UCA2IFG & UCRXIFG)
    {
    	rxA2 = UCA2RXBUF;
    	okay_to_display = 1;
    	UARTA2SendByte(rxA2);
    }
}

// UART3 interrupt service routine
void eUSCIA3IsrHandler(void)
{
	// RX interrupt
    if (UCA3IFG & UCRXIFG)
    {
    	rxA3 = UCA3RXBUF;
    	okay_to_display = 1;
    	UARTA3SendByte(rxA3);
    }
}

/*
 * UARTA2 Sending 1 string
 */
void UARTA2SendString(unsigned char* string)
{
	while(*string)
	{
		UARTA2SendByte(*string++);
	}
}

/*
 * UARTA2 Sending 1 byte
 */
void UARTA2SendByte(uint8_t byte)
{
	while(!(UCA2IFG & UCTXIFG));
	UCA2TXBUF = byte;
}

/*
 * UARTA3 Sending 1 string
 */
void UARTA3SendString(unsigned char* string)
{
	while(*string)
	{
		UARTA3SendByte(*string++);
	}
}

/*
 * UARTA3 Sending 1 byte
 */
void UARTA3SendByte(uint8_t byte)
{
	while(!(UCA3IFG & UCTXIFG));
	UCA3TXBUF = byte;
}

/* Converts number to a string */
void itoa(int num, unsigned char* num_string)
{
	int i, sign;

	 if ((sign = num) < 0)  /* record sign */
		 num = -num;          /* make n positive */
	 i = 0;
	 do {       /* generate digits in reverse order */
		 num_string[i++] = num % 10 + '0';   /* get next digit */
	 } while ((num /= 10) > 0);     /* delete it */
	 if (sign < 0)
		 num_string[i++] = '-';
	 num_string[i] = '\0';
	 reverse(num_string);
}

/* Reverses a string */
void reverse(unsigned char* s)
{
    int i, j;
    char c;

    for (i = 0, j = strlen((const char*)s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


