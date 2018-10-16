#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include "network.h"
#include "client.h"

void *console_thread();
int getNumFromUser(int low, int high);

#define FREEWAY_INTER_STATES 10

static char *FREEWAY_INTER_STATES_STRS[FREEWAY_INTER_STATES] = { "RRRRRRRR" , "GGGGRRRR", "RGRGRRGG", "RRRRGGRR", "GRRGGRRR", "RGGRRGRR", "GGRGRRGR", "RGGGRRRG", "RRRGGRRG", "RGRRRGGR"};


typedef struct
{
	Client_ID clientID;
	Node_Commands command;
	int data1, data2;
	pthread_mutex_t mutex;
}Console_Command;


#endif /* SRC_CONSOLE_H_ */
