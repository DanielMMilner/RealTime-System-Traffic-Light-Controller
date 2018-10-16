/*
 * OnRamp.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#include "OnRamp.h"
#include "LocalController.h"

int isActive(int rampID, enum onRampStates *newState, timer_t *timer_id, struct itimerspec *itime) {
	if ((rampID == 0 && !getSensorValue(East_Onramp)) || (rampID == 1 && !getSensorValue(West_Onramp))) {
		*newState = OFF;
		setRampState(rampID, '-');
		setTime(timer_id, itime, 0.1);
		return 0;
	}
	return 1;
}

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id, struct itimerspec *itime, int rampID) {
	switch (*newState) {
	case OFF:
		if (isActive(rampID, newState, timer_id, itime)) {
			*newState = RED;
			setRampState(rampID, 'R');
			setTime(timer_id, itime, 0.5);
		}
		break;
	case GREEN:
		if (isActive(rampID, newState, timer_id, itime)) {
			printf("GREEN\n");
			setRampState(rampID, 'G');
			*newState = YELLOW;
			setTime(timer_id, itime, 0.5);
		}
		break;
	case YELLOW:
		printf("YELLOW\n");
		setRampState(rampID, 'Y');
		*newState = RED;
		setTime(timer_id, itime, 0.5);
		break;
	case RED:
		if (isActive(rampID, newState, timer_id, itime)) {
			printf("RED\n");
			setRampState(rampID, 'R');
			*newState = GREEN;
			setTime(timer_id, itime, 3);
		}
		break;
	}
}

void *onRampStateMachine(void *isEast) {
	enum onRampStates newState = OFF;
	my_message_t msg;
	int chid;
	int rcvid;
	struct itimerspec itime;
	timer_t timer_id;
	int rampID = *((int *) isEast);

	setUpTimer(&chid, &timer_id);

	changeOnRampState(&newState, &timer_id, &itime, rampID);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				changeOnRampState(&newState, &timer_id, &itime, rampID);
			}
		}
	}

	return 0;
}
