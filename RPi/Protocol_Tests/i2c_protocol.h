/*
 * i2c_protocol.h
 *
 *  Created on: Nov 26, 2015
 *      Author: prananda0203
 */
#include "msp.h"
#include <stdint.h>
#ifndef I2C_PROTOCOL_H_
#define I2C_PROTOCOL_H_

// Functions
void i2c_init();
void i2c_write_reg(const uint8_t slaveAddr, const uint8_t byteAddr, const uint8_t data);
uint8_t i2c_read_reg(const uint8_t slaveAddr, const uint8_t byteAddr);
void i2c_read_mult_bytes(const uint8_t slaveAddr, const uint8_t byteAddr, const uint8_t n, uint8_t *arr);


#endif /* I2C_PROTOCOL_H_ */
