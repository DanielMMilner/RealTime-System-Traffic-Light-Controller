/*
 * TimerHandler.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */
#include "TimerHandler.h"

void setTime(timer_t *timer_id, struct itimerspec *itime, float length) {
	int seconds = (int) length;
	double decimal = length - seconds;

	// setup the timer
	itime->it_value.tv_sec = seconds;
	itime->it_value.tv_nsec = decimal * 1000000000; //converts decimal to nano seconds
	itime->it_interval.tv_sec = 0;
	itime->it_interval.tv_nsec = 0;

	timer_settime(*timer_id, 0, itime, NULL);
}

void setUpTimer(int *chid, timer_t *timer_id) {
	struct sigevent event;
	*chid = ChannelCreate(0); // Create a communications channel

	event.sigev_notify = SIGEV_PULSE;

	// create a connection back to ourselves for the timer to send the pulse on
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, *chid, _NTO_SIDE_CHANNEL,
			0);
	if (event.sigev_coid == -1) {
		printf("couldn't ConnectAttach to self!\n");
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;
	event.sigev_code = MY_PULSE_CODE;

	// create the timer, binding it to the event
	if (timer_create(CLOCK_REALTIME, &event, timer_id) == -1) {
		printf("couldn't create a timer, errno %d\n", errno);
		perror(NULL);
		exit(EXIT_FAILURE);
	}
}
