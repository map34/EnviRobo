#include "./i2c_protocol.h"
/*
 * i2c_protocol.c
 *
 *  Created on: Nov 26, 2015
 *      Author: prananda0203
 */

/*
 * I2C Intialization
 */

void i2c_init()
{
	// 1.7 SCL, 1.6 SDA
	P1SEL0 |= BIT6 | BIT7;                         			// Assign I2C pins to SCL/SDA
	P1SEL1 &= ~(BIT6 | BIT7);
															// Selection mode P1SEL1=0 and P1SEL0=1 for UCS_B0_I2C

	UCB0CTLW0 |= UCSWRST;                  					// Enable SW reset (reset state machine)

	UCB0CTLW0 |= (UCMST | UCMODE_3 | UCSYNC);     			// I2C Master, synchronous mode
	UCB0CTLW0 |= (UCSSEL__SMCLK | UCTR);            		// Use SMCLK, keep SW reset

	// fSCL = SMCLK/60, for example
	UCB0BRW = 60;
	UCB0CTLW0 &= ~UCSWRST;                    				// Clear SW reset, resume operation
}

/*
 * I2C Writing Process
 */
void i2c_write_reg(const uint8_t slaveAddr, const uint8_t byteAddr, const uint8_t data)
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
uint8_t i2c_read_reg(const uint8_t slaveAddr, const uint8_t byteAddr)
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

	while(UCB0CTLW0 & UCTXSTT);
	while (!(UCB0IFG & UCRXIFG0));							// wait for an rx character

	UCB0CTLW0 |= UCTXSTP;									// I2C stop condition

	dataToReturn = UCB0RXBUF;

	while (UCB0CTLW0 & UCTXSTP);

	return dataToReturn; 									// reading clears RXIFG flag
}

/*
* I2C Reading Multiple Bytes Consecutively
*/
void i2c_read_mult_bytes(const uint8_t slaveAddr, const uint8_t byteAddr, const uint8_t n, uint8_t *arr)
{

	int i;
	UCB0I2CSA = slaveAddr; 									// set slave address

	/** ACCESSING DATA **/
	UCB0CTLW0 |= UCTR + UCTXSTT;							// put in transmitter mode and send start bit
	while (!(UCB0IFG & UCTXIFG0) | (UCB0CTLW0 & UCTXSTT));  // wait for previous tx to complete

	UCB0TXBUF = byteAddr; 									// setting TXBUF clears the TXIFG flag
	while (!(UCB0IFG & UCTXIFG0) | (UCB0CTLW0 & UCTXSTT));  // wait for previous tx to complete

	/** SEND DATA BACK IN STREAM **/
	UCB0CTLW0 &= ~UCTR;										//change to receiver mode
	UCB0CTLW0 |= UCTXSTT;

	// send start bit
	while(UCB0CTLW0 & UCTXSTT);
	for (i = 0; i < n; i++)
	{

		while (!(UCB0IFG & UCRXIFG0)); 						// wait for rx buffer
		if(i == n - 1)
		{
			UCB0CTLW0 |= UCTXSTP; 							// I2C stop condition
		}
		arr[i] = UCB0RXBUF;
	}


	while (UCB0CTLW0 & UCTXSTP);

}
