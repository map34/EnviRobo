/*
 * compass.h
 *
 *  Created on: Dec 2, 2015
 *      Author: prananda0203
 */

#ifndef COMPASS_H_
#define COMPASS_H_

#include "msp.h"
#include <stdint.h>

#define HMC5883L_Address 0x1E
#define ConfigurationRegisterA 0x00
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define DataRegisterBegin 0x03

#define Measurement_Continuous 0x00
#define Measurement_SingleShot 0x01
#define Measurement_Idle 0x03

static uint8_t _buffer_compass[6] = {0x00};
static float m_Scale = 1.0;

void readCompassRaw(int* x, int* y, int* z);
void readCompassScaled(float* x, float* y, float *z);
uint8_t setCompassScale(unsigned char gauss);
void setCompassMeasurementMode(uint8_t mode);

#endif /* COMPASS_H_ */
