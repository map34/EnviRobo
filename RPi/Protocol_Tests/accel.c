#include "./accel.h"
#include "./i2c_protocol.h"

void accelTurnOn()
{
	i2c_write_reg(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x00);
	i2c_write_reg(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x10);
	// Measuremen mode
	i2c_write_reg(ADXL345_ADDRESS, ADXL345_POWER_CTL, 0x08);
}

void accelRead(int* x, int* y, int* z)
{
	i2c_read_mult_bytes(ADXL345_ADDRESS, ADXL345_DATAX0, 6, _buffer);
	//uint8_t readx_0 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAX0);
	//uint8_t readx_1 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAX1);
	//uint8_t ready_0 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAY0);
	//uint8_t ready_1 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAY1);
	//uint8_t readz_0 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAZ0);
	//uint8_t readz_1 = i2c_read_reg(ADXL345_ADDRESS, ADXL345_DATAZ1);


	*x = (((int)_buffer[1]) << 8) | _buffer[0];
	*z = (((int)_buffer[3]) << 8) | _buffer[2];
	*y = (((int)_buffer[5]) << 8) | _buffer[4];
	//*z = _buffer[4];
	//*x = readx_0;
	//*y = ready_0;
	//*z = readz_0;
}






