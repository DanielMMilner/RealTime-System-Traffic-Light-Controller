/*
 * Sensors.c
 *
 *  Created on: 28Sep.,2018
 *      Author: Daniel
 */

#include "Sensors.h"

TurningSensors turningSensors = { 0, 0, 0, 0 };

void changeSensor(int* sensor, int value) {
	pthread_mutex_lock(&turningSensors.mutex);
	*sensor = value;
	pthread_mutex_unlock(&turningSensors.mutex);
}

//To be replaced with keypad input
void *userInput() {
	int temp;
	while (1) {
		temp = getchar();
		if (temp == '1')
			changeSensor(&turningSensors.NE_Waiting, 1);
		if (temp == '2')
			changeSensor(&turningSensors.SW_Waiting, 1);
		if (temp == '3')
			changeSensor(&turningSensors.ES_Waiting, 1);
		if (temp == '4')
			changeSensor(&turningSensors.WN_Waiting, 1);
	}
	return 0;
}
