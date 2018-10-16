#include "console.h"

extern Client_typedef * clients[CLIENT_COUNT];

void *console_thread() {
	Client_ID ids;
	Node_Commands command;

	int num;

	while (1) {
		// Get all states,
		printf(
				"\nTraffic light Controller Menu\n--> 1 - Update States of connected devices\n--> 2 - Use Command\n");
		num = getNumFromUser(1, 2);
		if (num == 1) {
			for (int i = 0; i < CLIENT_COUNT; i++) {
				if (clients[i]->conn_state == CONN_STATE_CONNECTED) {
					pthread_mutex_lock(&clients[i]->mutex);
					clients[i]->messageReady = 1;
					clients[i]->command = COMMAND_GET_STATE;
					pthread_mutex_unlock(&clients[i]->mutex);
				}
			}
		} else if (num == 2) {
			// Send a command

			printf("Select a client:\n");
			for (int i = 0; i < CLIENT_COUNT; i++) {
				printf("-> %d - %s (%s)\n", i, CLIENT_NAMES[clients[i]->id],
						CONN_STATE_STRS[clients[i]->conn_state]);
			}
			num = getNumFromUser(0, CLIENT_COUNT - 1);
			ids = clients[num]->id;

			if(clients[num]->conn_state == CONN_STATE_DISCONNECTED)
			{
				printf("ERROR : Node %s is disconnected\n", CLIENT_NAMES[ids]);
				continue;
			}

			printf("Select a command to send to the %s:\n", CLIENT_NAMES[ids]);
			printf("--> 1 - Toggle Sensor\n");

			if (ids == CID_FREEWAY) {
				printf("--> 2 - Change Light timings\n");
				printf("--> 3 - Change Light patterns\n");
			}

			command = (Node_Commands) getNumFromUser(0, CLIENT_COUNT - 1);

			for (int x = 0; x < CLIENT_COUNT; x++) {
				if (clients[x]->id == ids) {
					Client_typedef *cc = clients[x];
					pthread_mutex_lock(&clients[x]->mutex);
					cc->messageReady = 1;
					cc->command = command;

					switch (command) {
					case COMMAND_TOGGLE_SENSOR:
						printf("Enter the sensor number to trigger %s:\n",
								CLIENT_NAMES[ids]);
						cc->data1 = getNumFromUser(0, 100);
						break;
					case COMMAND_CHANGE_LIGHT_TIMING:
						for (int h = 0; h < FREEWAY_INTER_STATES; h++) {
							printf("--> %d - %s\n",h,
									FREEWAY_INTER_STATES_STRS[h]);
						}
						printf(
								"Enter the new light timing as {0=fixed, 1=sensors},{startState},{time} for %s:\n",
								CLIENT_NAMES[ids]);
						int fixedOrSensor, startState, time;
						while (1) {
							char buff[100];
							fgets(buff, 100, stdin);
							int res = sscanf(buff, "%d,%d,%d", &fixedOrSensor, &startState, &time);
							if (res <= 0) {
								printf(
										"{0=fixed, 1=sensors},{startState},{time}. Try again: \n");
							} else {
								break;
							}
						}
						cc->data1 = fixedOrSensor;
						cc->data2 = startState;
						cc->data3 = time;

						break;
					case COMMAND_CHANGE_LIGHT_PATTERN:

						for (int h = 0; h < FREEWAY_INTER_STATES; h++) {
							printf("--> %d - %s\n",h,
									FREEWAY_INTER_STATES_STRS[h]);
						}
						printf("Enter the new light pattern as {startState},{finshState} for %s:\n",
								CLIENT_NAMES[ids]);
						int ss, fs;
						while (1) {
							char buff[100];
							fgets(buff, 100, stdin);
							int res = sscanf(buff, "%d,%d", &ss, &fs);
							if (res <= 0) {
								printf(
										"{startState},{finshState}. Try again: \n");
							} else {
								break;
							}
						}
						cc->data1 = ss;
						cc->data2 = fs;
						break;
					}

					pthread_mutex_unlock(&clients[x]->mutex);
					break;
				}
			}
		}

	}

	// Select a node.

	// Select an action
	// Enter data for that action.

	// Set train time
	// Get state of each node.
	// Set light timings.
	// Alter light patterns for freeway.
	// Trigger sensor
}

int getNumFromUser(int low, int high) {
	int num;
	char buff[100];
	fgets(buff, 100, stdin);
	int res = sscanf(buff, "%d", &num);
	if (res <= 0 || num < low || num > high) {
		printf("Must enter a number between %d and %d. Try again: ", low, high);
		num = getNumFromUser(low, high);
	}
	return num;
}
