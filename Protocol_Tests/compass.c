/*
 * compass.c
 *
 *  Created on: Dec 2, 2015
 *      Author: prananda0203
 */
#include "./i2c_protocol.h"
#include "./compass.h"

/*
 * Reading raw values from sensor
 */
void readCompassRaw(int* x, int* y, int* z)
{
	i2c_read_mult_bytes(HMC5883L_Address, DataRegisterBegin, 6, _buffer_compass);
	*x = (_buffer_compass[1] << 8) | _buffer_compass[0];
	*y = (_buffer_compass[3] << 8) | _buffer_compass[2];
	*z = (_buffer_compass[5] << 8) | _buffer_compass[4];
}

/*
 * Reading scaled values from sensor
 */
void readCompassScaled(float* x, float* y, float *z)
{
	int x_dec = 0;
	int y_dec = 0;
	int z_dec = 0;

	readCompassRaw(&x_dec, &y_dec, &z_dec);

	*x = (float)x_dec * m_Scale;
	*y = (float)y_dec * m_Scale;
	*z = (float)z_dec * m_Scale;
	// TO DO
}

/*
 * Set the gaussian scale for sensor
 */
uint8_t setCompassScale(unsigned char gauss)
{
	uint8_t regValue = 0x00;
	if(gauss == '0')
	{
		regValue = 0x00;
		m_Scale = 0.73;
	}
	else if(gauss == '1')
	{
		regValue = 0x01;
		m_Scale = 0.92;
	}
	else if(gauss == '2')
	{
		regValue = 0x02;
		m_Scale = 1.22;
	}
	else if(gauss == '3')
	{
		regValue = 0x03;
		m_Scale = 1.52;
	}
	else if(gauss == '4')
	{
		regValue = 0x04;
		m_Scale = 2.27;
	}
	else if(gauss == '5')
	{
		regValue = 0x05;
		m_Scale = 2.56;
	}
	else if(gauss == '6')
	{
		regValue = 0x06;
		m_Scale = 3.03;
	}
	else if(gauss == '7')
	{
		regValue = 0x07;
		m_Scale = 4.35;
	}
	else
		return 3;

	regValue = regValue << 5;
	i2c_write_reg(HMC5883L_Address, ConfigurationRegisterB, regValue);
	return 0;
}

// Set Measurement Mode
void setCompassMeasurementMode(uint8_t mode)
{
	i2c_write_reg(HMC5883L_Address, ModeRegister, mode);
}





