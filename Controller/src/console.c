#include "console.h"

int *console_thread()
{
	printf("Connected nodes \n");
	for (int i = 0; i < CLIENT_COUNT; i++)
	{
		printf("  %d -- %s\n", i+1, CLIENT_NAMES[i]);
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
