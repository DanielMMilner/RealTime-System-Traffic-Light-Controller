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
#include "Client.h"
#include "LCDdisplay.h"


int main(int argc, char *argv[]) {
	printf("Intersection running...\n");

    int *east = malloc(sizeof(*east));
    int *west = malloc(sizeof(*west));
    *east = 0;
    *west = 1;

	pthread_t onRamp1;
	pthread_create(&onRamp1, NULL, onRampStateMachine, (void *) east);

	pthread_t onRamp2;
	pthread_create(&onRamp2, NULL, onRampStateMachine, (void *) west);

	pthread_t mainIntersection;
	pthread_create(&mainIntersection, NULL, mainIntersectionStateMachine, NULL);

	pthread_t client;
	pthread_create(&client, NULL, server_thread, NULL);

//	pthread_t lcd;
//	pthread_create(&lcd, NULL, LCDfunction, NULL);

	keypadMethod();

	pthread_join(onRamp1, NULL);
	pthread_join(onRamp2, NULL);
	pthread_join(mainIntersection, NULL);
	pthread_join(client, NULL);
//	pthread_join(lcd, NULL);


	printf("Main Terminated...\n");
	return EXIT_SUCCESS;
}
