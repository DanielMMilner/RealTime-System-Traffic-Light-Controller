#ifndef SRC_CONSOLE_H_
#define SRC_CONSOLE_H_

#include "network.h"
#include "client.h"

int *console_thread();

typedef struct
{
	Client_ID clientID;
	Node_Commands command;
	int data1, data2;
	pthread_mutex_t mutex;
}Console_Command;


#endif /* SRC_CONSOLE_H_ */
