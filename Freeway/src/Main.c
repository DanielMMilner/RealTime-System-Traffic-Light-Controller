/*
 * Main.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include <stdlib.h>
#include <stdio.h>
#include "FreewayIntersection.h"
#include "Sensors.h"
#include "Client.h"
#include "LCDdisplay.h"
#include "MQueues.h"


int main(int argc, char *argv[]) {
	printf("Intersection running...\n");

	pthread_t input;
	pthread_create(&input, NULL, userInput, NULL);

	pthread_t client;
	pthread_create(&client, NULL, server_thread, NULL);

	pthread_t lcd;
	pthread_create(&lcd, NULL, LCDfunction, NULL);

	pthread_t mainIntersection;
	pthread_create(&mainIntersection, NULL, mainIntersectionStateMachine, NULL);

	pthread_t OnRampCommsSend;
	pthread_create(&OnRampCommsSend, NULL, OnRampCommunicationSend, NULL);

	pthread_t OnRampCommsRecieve;
	pthread_create(&OnRampCommsRecieve, NULL, OnRampCommunicationRecieve, NULL);

	keypadMethod();

	pthread_join(lcd, NULL);
	pthread_join(mainIntersection, NULL);
	pthread_join(input, NULL);
	pthread_join(client, NULL);
	pthread_join(OnRampCommsSend, NULL);
	pthread_join(OnRampCommsRecieve, NULL);

	printf("Main Terminated...\n");
	return EXIT_SUCCESS;
}
