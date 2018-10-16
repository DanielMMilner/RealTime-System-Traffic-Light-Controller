/*
 * LocalController.c
 *
 *  Created on: Oct 16, 2018
 *      Author: s3542977
 */

#include "LocalController.h"

char eastState = '-';
char westState = '-';

int eastRamp = 0;
int westRamp = 0;

int getSensorValue(int isWestRamp){
	if(isWestRamp){
		return westRamp;
	}else{
		return eastRamp;
	}
}

char getRampState(int isWestRamp){
	if(isWestRamp){
		return westState;
	}else{
		return eastState;
	}
}

void setRampState(int isWestRamp, char state){
	if(isWestRamp){
		westState = state;
	}else{
		eastState = state;
	}
}

void *localControllerFunctionRecieve(){
	printf("mqueue receive process\n");

	mqd_t	qd;
	char	buf[MESSAGESIZE];

	struct	mq_attr	attr;

	// example using the default path notation.
	const char * MqueueLocation = "/s3542977";	/* will be located /dev/mqueue/test_queue  */

	qd = mq_open(MqueueLocation, O_RDONLY);		//MqueueLocation should be opened on the node where the queue was established
    if (qd != -1)
	{
		mq_getattr(qd, &attr);
		printf ("max. %ld msgs, %ld bytes; waiting: %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

		while (mq_receive(qd, buf, MESSAGESIZE, NULL) > 0)  //wait for the messages
		{
			eastRamp = buf[0] - '0';
			westRamp = buf[1] - '0';
//			printf("dequeue: East Ramp: %c	West Ramp: %c\n", buf[0], buf[1]);					//print out the messages to this terminal
			if (!strcmp(buf, "done"))			//once we get this message we know not to expect any more mqueue data
				break;
		}
		mq_close(qd);
	}

	printf("mqueue receive process exited\n");
	return EXIT_SUCCESS;
}


void *localControllerFunctionSend(){
    mqd_t	qd;
    char	buf[MESSAGESIZE];

    struct  mq_attr  attr;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MESSAGESIZE;
    attr.mq_flags = 0;

	// example using the default path notation.
	const char * MqueueLocation = "/s3542977State";

    qd = mq_open(MqueueLocation, Q_FLAGS, Q_Mode, &attr);			 // full path will be: <host_name>/dev/mqueue/test_queue
    if (qd != -1)
    {
		while(1)
        {
			sprintf(buf, "%c%c", getRampState(0),getRampState(1));			//put the message in a char[] so it can be sent
//			printf("queue: '%s'\n", buf); 			//print the message to this processes terminal
			mq_send(qd, buf, MESSAGESIZE, 0);		//send the mqueue
			usleep(200000);
		}
		//mq_send(qd, "done", 5, 0);					// send last message so the receive process knows
													// not to expect any more messages. 5 char long because
													// of '/0' char at end of the "done" string
		printf("\nAll Messages sent to the queue\n");

		// as soon as this code executes the mqueue data will be deleted
		// from the /dev/mqueue/test_queue  file structure
		mq_close(qd);
		mq_unlink(MqueueLocation);
    }
    else
    {
    	printf("\nThere was an ERROR opening the message queue!");
    	printf("\nHave you started the 'mqueue' process on the VM target?\n");
    }

	printf("\nmqueue send process Exited\n");
	return EXIT_SUCCESS;
}
