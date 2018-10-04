/*
 * Main.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include <stdlib.h>
#include <stdio.h>
#include "FreewayIntersection.h"
#include "OnRamp.h"
#include "Sensors.h"

int main(int argc, char *argv[]) {
	printf("Intersection running\n");

	pthread_t sensors;
	pthread_create(&sensors, NULL, userInput, NULL);

	pthread_t onRamp1;
	pthread_create(&onRamp1, NULL, onRampStateMachine, NULL);

	pthread_t onRamp2;
	pthread_create(&onRamp2, NULL, onRampStateMachine, NULL);

	pthread_t mainIntersection;
	pthread_create(&mainIntersection, NULL, mainIntersectionStateMachine, NULL);

	keypadMethod();

	pthread_join(sensors, NULL);
	pthread_join(onRamp1, NULL);
	pthread_join(onRamp2, NULL);
	pthread_join(mainIntersection, NULL);

	printf("Main Terminated...\n");
	return EXIT_SUCCESS;
}
