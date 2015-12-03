//*****************************************************************************
//
// MSP432 main.c template - Empty main
//
//****************************************************************************

#include "msp.h"
#include "./oled.h"
#include <stdint.h>
#define OLED_SLAVE_ADDR 0x3C

void i2cOLEDStartBit(uint8_t address);
void i2cOLEDStopBit();
void i2cOLEDSendByte(uint8_t byte);
void i2cOLEDStartCommand();
void i2cOLEDStartData();
void i2cOLEDSendChar(unsigned char c);
void i2cOLEDSendString(unsigned char* str);
void i2cOLEDInitialize();
void i2c_init();
void i2cOLEDWriteCommand(const uint8_t command);
void i2cOLEDWriteData(const uint8_t data);
void i2cOLEDSetPos(uint8_t x, uint8_t y);
void i2cOLEDSendChar(unsigned char c);
void i2cOLEDFillScreen(uint8_t p1, uint8_t p2);
void i2cOLEDClear();
void SMCLKSetUp(const uint8_t freq);

/*
 * MAIN FUNCTION LOL
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // Stop watchdog timer
    i2c_init();
    SMCLKSetUp(24);
    i2cOLEDInitialize();
    i2cOLEDClear();
    i2cOLEDSetPos(0,0);
    i2cOLEDSendString("ABC Haha this is working!");
	while(1){}
}

/*
 * OLED Initialization
 */
void i2cOLEDInitialize()
{
	int init_length = sizeof(oled_init_sequence)/sizeof(oled_init_sequence[0]);
	int i;
	i2cOLEDStartCommand();
	for (i = 0; i < init_length; i++)
	{
		i2cOLEDSendByte(oled_init_sequence[i]);
	}
	i2cOLEDStopBit();
}

/*
 *	I2C OLED Clear
 */
void i2cOLEDClear()
{
	i2cOLEDFillScreen(0x00, 0x00);
}

/*
 * I2C OLED Setting Cursor Position
 */
void i2cOLEDSetPos(uint8_t x, uint8_t y)
{
	i2cOLEDStartCommand();
	i2cOLEDSendByte(0xB0 + y);
	i2cOLEDSendByte(((x & 0xF0) >> 4) | 0x10); // | 0x10
// TODO: Verify correctness
	i2cOLEDSendByte((x & 0x0F)); // | 0x01
	i2cOLEDStopBit();
}

/*
 * I2C OLED Send Char
 */
void i2cOLEDSendChar(unsigned char c)
{
	unsigned char cToPrint = c - 32;
	i2cOLEDStartData();
	int i;
	//int ascii_size = sizeof(font6x8)/sizeof(font6x8[0]);
	for (i=0; i < 6; i++)
	{
		i2cOLEDSendByte(font6x8[cToPrint*6 + i]);
	}
	i2cOLEDStopBit();
}

/*
 * I2C Fill Screen
 */
void i2cOLEDFillScreen(uint8_t p1, uint8_t p2)
{


	unsigned char x,y;

	i2cOLEDWriteCommand(0x02);    /*set lower column address*/
	i2cOLEDWriteCommand(0x10);    /*set higher column address*/
	i2cOLEDWriteCommand(0xB0);    /*set page address*/
	for(y=0;y<8;y++)
	{
		i2cOLEDWriteCommand(0xB0+y);    /*set page address*/
		i2cOLEDWriteCommand(0x02);    /*set lower column address*/
		i2cOLEDWriteCommand(0x10);    /*set higher column address*/
		for(x=0;x<64;x++)
		{
			i2cOLEDWriteData(p2);
			i2cOLEDWriteData(p2);
		}
	}
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


/*
 * Send a string to OLED via I2C
 */
void i2cOLEDSendString(unsigned char* str)
{
	while(*str)
	{
		i2cOLEDSendChar(*str++);
	}
}

/*
 * I2C OLED Start Command
 */
void i2cOLEDStartCommand()
{
	i2cOLEDStartBit(OLED_SLAVE_ADDR);
	i2cOLEDSendByte(0x00);
}

/*
 * I2C OLED Start Data
 */
void i2cOLEDStartData()
{
	i2cOLEDStartBit(OLED_SLAVE_ADDR);
	i2cOLEDSendByte(0x40);
}

/*
 * I2C OLED Send Byte
 */
void i2cOLEDSendByte(uint8_t byte)
{
	UCB0TXBUF = byte; 								// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0)); 						// wait for previous tx to complete

}


/*
 * i2cOLED Start Bit
 */
void i2cOLEDStartBit(uint8_t address)
{
	UCB0I2CSA = address; 						// set slave address
	while (UCB0CTLW0 & UCTXSTP);
	UCB0CTLW0 |= UCTR | UCTXSTT;						// put in transmitter mode and send start bit

	while (UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCTXIFG0)); 						// wait for previous tx to complete
}

/*
 * i2cOLED Stop Bit
 */
void i2cOLEDStopBit()
{
	UCB0CTLW0 |= UCTXSTP; 									// I2C stop condition
	while (UCB0CTLW0 & UCTXSTP);
}


/*
 * I2C Intialization
 */
void i2c_init()
{
	P1SEL0 |= BIT6 | BIT7;                         			// Assign I2C pins to SCL/SDA
	P1SEL1 &= ~(BIT6 | BIT7);
															// Selection mode P1SEL1=0 and P1SEL0=1 for UCS_B0_I2C

	UCB0CTLW0 |= UCSWRST;                  					// Enable SW reset (reset state machine)

	UCB0CTLW0 |= (UCMST | UCMODE_3 | UCSYNC);     			// I2C Master, synchronous mode
	UCB0CTLW0 |= (UCSSEL__SMCLK | UCTR);            		// Use SMCLK, keep SW reset

	// fSCL = SMCLK/120, for example
	UCB0BRW = 120;
	UCB0CTLW0 &= ~UCSWRST;                    				// Clear SW reset, resume operation
}

/*
 * OLED_commands
 */
void i2cOLEDWriteCommand(const uint8_t command)
{
	UCB0I2CSA = OLED_SLAVE_ADDR; 									// set slave address
	while (UCB0CTLW0 & UCTXSTP);
	UCB0CTLW0 |= UCTR | UCTXSTT;							// put in transmitter mode and send start bit

	while(UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0TXBUF = 0x00;	 								// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0));

	UCB0TXBUF = command; 										// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0CTLW0 |= UCTXSTP; 									// I2C stop condition
	while (UCB0CTLW0 & UCTXSTP);
}

/*
 * OLED Write Data
 */
void i2cOLEDWriteData(const uint8_t data)
{
	UCB0I2CSA = OLED_SLAVE_ADDR; 									// set slave address
	while (UCB0CTLW0 & UCTXSTP);
	UCB0CTLW0 |= UCTR | UCTXSTT;							// put in transmitter mode and send start bit

	while(UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0TXBUF = 0x40;	 								// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0));

	UCB0TXBUF = data; 										// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0CTLW0 |= UCTXSTP; 									// I2C stop condition
	while (UCB0CTLW0 & UCTXSTP);
}

/*
 * I2C Writing Process
 */
void i2c_write(const uint8_t slaveAddr, const uint8_t byteAddr, const uint8_t data)
{
	UCB0I2CSA = slaveAddr; 									// set slave address
	while (UCB0CTLW0 & UCTXSTP);
	UCB0CTLW0 |= UCTR | UCTXSTT;							// put in transmitter mode and send start bit

	while(UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0TXBUF = byteAddr;	 								// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0));

	UCB0TXBUF = data; 										// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	UCB0CTLW0 |= UCTXSTP; 									// I2C stop condition
	while (UCB0CTLW0 & UCTXSTP);
}

/*
 * I2C Reading Process
 */
uint8_t i2c_read(const uint8_t slaveAddr, const uint8_t byteAddr)
{
	uint8_t dataToReturn;

	/** ACCESSING DATA **/
	UCB0I2CSA = slaveAddr;									// set slave address
	UCB0CTLW0 |= UCTR | UCTXSTT;							// put in transmitter mode and send start bit

	while (UCB0CTLW0 & UCTXSTT); 							// wait for previous tx to complete
	while (!(UCB0IFG & UCTXIFG0));

	UCB0TXBUF = byteAddr; 									// setting TXBUF clears the TXIFG flag

	while(UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCTXIFG0)); 							// wait for previous tx to complete

	/*** SEND BACK DATA **/
	UCB0CTLW0 &= ~UCTR;										// change to receiver mode
	UCB0CTLW0 |= UCTXSTT;									// send start bit

	while (!(UCB0IFG & UCRXIFG0));							// wait for an rx character

	UCB0CTLW0 |= UCTXSTP;									// I2C stop condition

	dataToReturn = UCB0RXBUF;

	while (UCB0CTLW0 & UCTXSTP);

	return dataToReturn; 									// reading clears RXIFG flag
}

