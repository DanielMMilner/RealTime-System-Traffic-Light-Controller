/*
 * Main.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#include <stdio.h>
#include <stdlib.h>

#include "OnRamp.h"

int main(void) {
    int *east = malloc(sizeof(*east));
    int *west = malloc(sizeof(*west));
    *east = 0;
    *west = 1;

	pthread_t onRamp1;
	pthread_create(&onRamp1, NULL, onRampStateMachine, (void *) east);

	pthread_t onRamp2;
	pthread_create(&onRamp2, NULL, onRampStateMachine, (void *) west);

	pthread_t localControllerRecieve;
	pthread_create(&localControllerRecieve, NULL, localControllerFunctionRecieve, NULL);

	pthread_t localControllerSend;
	pthread_create(&localControllerSend, NULL, localControllerFunctionSend, NULL);

	pthread_join(onRamp1, NULL);
	pthread_join(onRamp2, NULL);
	pthread_join(localControllerRecieve, NULL);
	pthread_join(localControllerSend, NULL);

	return EXIT_SUCCESS;
}
