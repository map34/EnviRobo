#include "./oled.h"

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
	i2cOLEDSetPos(0, 0);
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
void i2cOLEDSendChar(char c)
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
 * Send a string to OLED via I2C
 */
void i2cOLEDSendString(char* str)
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
