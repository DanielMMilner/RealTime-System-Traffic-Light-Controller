/*
 * lcd.h
 *
 *  Created on: 15Oct.,2018
 *  Author: s3542623
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdlib.h>
#include <stdio.h>
#include "stdint.h"
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sched.h>

#include <time.h>
#include <sys/netmgr.h>

#define DATA_SEND 0x40  // sets the Rs value high
#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL


// Function prototypes
int init_LCD();
int I2cWrite_(int fd, uint8_t Address, uint8_t mode, uint8_t *pBuffer, uint32_t NbData);
void SetCursor(int fd, uint8_t LCDi2cAdd, uint8_t row, uint8_t column);
void Initialise_LCD (int fd, _Uint32t LCDi2cAdd);

#endif /* LCD_H_ */
