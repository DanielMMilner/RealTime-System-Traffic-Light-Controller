/*
 * OnRamp.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#include "OnRamp.h"

//struct itimerspec       itime_3_sec;
//struct itimerspec       itime_half_sec;
struct itimerspec       itime;

timer_t                 timer_id;

void changeOnRampState(enum onRampStates *newState){
	switch (*newState) {
		case OFF:
			printf("OFF\n");
			*newState = GREEN;
			setTime(timer_id, &itime, 2);
//			timer_settime(timer_id, 0, &itime_3_sec, NULL);
			break;
		case GREEN:
			printf("GREEN\n");
			*newState = YELLOW;
			setTime(timer_id, &itime, 0.5);
//			timer_settime(timer_id, 0, &itime_half_sec, NULL);
			break;
		case YELLOW:
			printf("YELLOW\n");
			*newState = RED;
			setTime(timer_id, &itime, 0.5);
//			timer_settime(timer_id, 0, &itime_half_sec, NULL);
			break;
		case RED:
			printf("RED\n");
			*newState = GREEN;
			setTime(timer_id, &itime, 3);
//			timer_settime(timer_id, 0, &itime_3_sec, NULL);
			break;
		}
}

void *onRampStateMachine() {
	enum onRampStates newState = OFF;
	my_message_t            msg;
	int                     chid;
	int                     rcvid;

	setUpTimer(&chid, &timer_id);
//	setUpTimer(&chid, &itime_half_sec, &timer_id, 0.5);

	changeOnRampState(&newState);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0){
		   if (msg.pulse.code == MY_PULSE_CODE){
			   changeOnRampState(&newState);
		   }
		}
	}

	return 0;
}
