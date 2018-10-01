/*
 * Sensors.h
 *
 *  Created on: 28Sep.,2018
 *      Author: Daniel
 */

#ifndef SENSORS_H_
#define SENSORS_H_

//#include "FreewayIntersection.h"
#include <stdio.h>
#include <pthread.h>

typedef struct {
	int NE_Waiting;
	int SW_Waiting;
	int ES_Waiting;
	int WN_Waiting;
	pthread_mutex_t mutex;
} TurningSensors;

void changeSensor(int* sensor, int value);

void *userInput();

#endif /* SENSORS_H_ */
