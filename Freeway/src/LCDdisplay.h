/*
 * LCDdisplay.h
 *
 *  Created on: Oct 11, 2018
 *      Author: s3542977
 */

#ifndef LCDDISPLAY_H_
#define LCDDISPLAY_H_

#include <stdlib.h>
#include <stdio.h>
#include "stdint.h"
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <string.h>
#include "Sensors.h"


#define DATA_SEND 0x40  // sets the Rs value high
#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command

typedef struct
{
	int fd;
	uint8_t Address;
	uint8_t mode;
	pthread_mutex_t mutex;
} LCD_connect;

typedef struct
{
	char intersectionState[9];
	char OnRamps[2];
	int useSensors;
	pthread_mutex_t mutex;
} LCD_Data;

// Function prototypes
void changeDisplayUseSensors(int useSensors);
void changeDisplayIntersection(char* state);
void changeDisplayOnRamp(char state, int ramp);
int *LCDfunction();
int  I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData);
void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column);
void Initialise_LCD (int fd, _Uint32t LCDi2cAdd);

#endif /* LCDDISPLAY_H_ */
