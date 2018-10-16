#include "console.h"

extern Client_typedef * clients[CLIENT_COUNT];

void *console_thread() {
	Client_ID ids;
	Node_Commands command;

	int num;

	printf("Select a client:\n");
	for (int i = 0; i < CLIENT_COUNT; i++) {
		printf("-> %d - %s\n", i, CLIENT_NAMES[clients[i]->id]);
	}
	num = getNumFromUser(0, CLIENT_COUNT - 1);
	ids = clients[num]->id;

	printf("Select a command to send to the %s:\n", CLIENT_NAMES[ids]);
	for (int i = 0; i < COMMAND_COUNT; i++) {
		printf("-> %d - %s\n", i, COMMAND_STRS[i]);
	}
	command = (Node_Commands)getNumFromUser(0, CLIENT_COUNT - 1);

	for(int x = 0; x < CLIENT_COUNT; x++)
	{
		if(clients[x]->id == ids)
		{
			Client_typedef *cc = clients[x];
			pthread_mutex_lock(&clients[x]->mutex);
			cc->messageReady = 1;
			cc->command = command;
			switch(command)
			{
			case COMMAND_GET_STATE:

				break;
			case COMMAND_TOGGLE_SENSOR:
				printf("Enter the sensor number to trigger %s:\n", CLIENT_NAMES[ids]);
				cc->data1 = getNumFromUser(0, 100);
				break;
			case COMMAND_CHANGE_LIGHT_TIMING:
				printf("Enter the new light timing %s:\n", CLIENT_NAMES[ids]);
				cc->data1 = getNumFromUser(0, 100);
				break;

			case COMMAND_CHANGE_LIGHT_PATTERN:
				printf("Enter the new light pattern %s:\n", CLIENT_NAMES[ids]);
				cc->data1 = getNumFromUser(0, 100);
				break;
			}

			pthread_mutex_unlock(&clients[x]->mutex);
			break;
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
	fgets (buff, 100, stdin);
	int res = sscanf(buff, "%d", &num);
	if(res <= 0 || num < low || num > high)
	{
		printf("Must enter a number between %d and %d. Try again: ", low, high);
		num = getNumFromUser(low, high);
	}
	return num;
}
