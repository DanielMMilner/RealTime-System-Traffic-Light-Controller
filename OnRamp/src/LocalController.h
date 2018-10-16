/*
 * LocalController.h
 *
 *  Created on: Oct 16, 2018
 *      Author: s3542977
 */

#ifndef LOCALCONTROLLER_H_
#define LOCALCONTROLLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>

#define  MESSAGESIZE 1000
#define Q_FLAGS O_RDWR | O_CREAT
#define Q_Mode S_IRUSR | S_IWUSR

int getSensorValue(int rampID);

void* localControllerFunctionRecieve();

void* localControllerFunctionSend();

void setRampState(int isWestRamp, char state);

#endif /* LOCALCONTROLLER_H_ */
