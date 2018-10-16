#include "console.h"

extern Client_typedef * clients[CLIENT_COUNT];

int *console_thread()
{
	printf("CONSOLE");
//	pthread_mutex_lock(&clients[2]->mutex);
//	// Check if a msg is ready
//	clients[2]->command = COMMAND_GET_STATE;
//	clients[2]->data1 = 100;
//	clients[2]->data2 = 200;
//
//	clients[2]->messageReady = 1;
//	pthread_mutex_unlock(&clients[2]->mutex);

	// Select a node.
		// Select an action
			// Enter data for that action.

	// Set train time
	// Get state of each node.
	// Set light timings.
	// Alter light patterns for freeway.
	// Trigger sensor

}
