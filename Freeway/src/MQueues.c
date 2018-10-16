/*
 * MQueues.c
 *
 *  Created on: Oct 16, 2018
 *      Author: s3542977
 */

#include "MQueues.h"

void *OnRampCommunicationSend(){
    mqd_t	qd;
    char	buf[MESSAGESIZE];

    struct  mq_attr  attr;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MESSAGESIZE;
    attr.mq_flags = 0;

	// example using the default path notation.
	const char * MqueueLocation = "/s3542977";

    qd = mq_open(MqueueLocation, Q_FLAGS, Q_Mode, &attr);			 // full path will be: <host_name>/dev/mqueue/test_queue
    if (qd != -1)
    {
		while(1)
        {
			sprintf(buf, "%d%d", getSensorEnabled(6),getSensorEnabled(7));			//put the message in a char[] so it can be sent
			//printf("queue: '%s'\n", buf); 			//print the message to this processes terminal
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

void *OnRampCommunicationRecieve(){
	printf("mqueue receive process\n");

	mqd_t	qd;
	char	buf[MESSAGESIZE];

	struct	mq_attr	attr;

	// example using the default path notation.
	const char * MqueueLocation = "/s3542977State";	/* will be located /dev/mqueue/test_queue  */

	qd = mq_open(MqueueLocation, O_RDONLY);		//MqueueLocation should be opened on the node where the queue was established
    if (qd != -1)
	{
		mq_getattr(qd, &attr);
		printf ("max. %ld msgs, %ld bytes; waiting: %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

		while (mq_receive(qd, buf, MESSAGESIZE, NULL) > 0)  //wait for the messages
		{
			if(getSensorEnabled(6)){
				changeDisplayOnRamp(buf[0], 0);
			}else{
				changeDisplayOnRamp('-', 0);
			}

			if(getSensorEnabled(7)){
				changeDisplayOnRamp(buf[1], 1);
			}else{
				changeDisplayOnRamp('-', 1);
			}

//			printf("dequeue: East Ramp: %c	West Ramp: %c\n", buf[0], buf[1]);					//print out the messages to this terminal
			if (!strcmp(buf, "done"))			//once we get this message we know not to expect any more mqueue data
				break;
		}
		mq_close(qd);
	}

	printf("mqueue receive process exited\n");
	return EXIT_SUCCESS;
}

