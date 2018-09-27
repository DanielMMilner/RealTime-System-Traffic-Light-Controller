/*
 * TimerHandler.h
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#ifndef SRC_TIMERHANDLER_H_
#define SRC_TIMERHANDLER_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <time.h>

#define BUF_SIZE 100
#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union
{
	struct _pulse   pulse;
    // your other message structures would go here too
} my_message_t;

typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	int ClientID; // our data (unique id from client)
	char data;     // our data
} my_data;

typedef struct {
	struct _pulse hdr;  // Our real data comes after this header
	char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} my_reply;

void setTime(timer_t *timer_id, struct itimerspec *itime, float length);

void setUpTimer(int *chid, timer_t *timer_id);

#endif /* SRC_TIMERHANDLER_H_ */
