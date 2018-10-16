#include "console.h"

extern Client_typedef * clients[CLIENT_COUNT];

int *console_thread()
{
	while(1)
	{

		printf("Nodes: \n");
		for (int i = 0; i < CLIENT_COUNT; i++)
		{
			printf("-> %s is %s\n", CLIENT_NAMES[clients[i]->id], CONN_STATE_STRS[clients[i]->conn_state]);
		}
		sleep(1);
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
