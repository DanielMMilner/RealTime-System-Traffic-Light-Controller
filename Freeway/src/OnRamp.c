/*
 * OnRamp.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#include "OnRamp.h"

int isActive(int rampID, enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime) {
	//checks if the ramps are still active. sets to off state if not active.
	if ((rampID == 0 && !getSensorValue(East_Onramp))
			|| (rampID == 1 && !getSensorValue(West_Onramp))) {
		*newState = OFF;
		setTime(timer_id, itime, 0.1);
		changeDisplayOnRamp('-', rampID);
		return 0;
	}
	return 1;
}

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime, int rampID) {
	switch (*newState) {
	case OFF:
		//checks if ramp is active
		if (isActive(rampID, newState, timer_id, itime)) {
			//ramp is active, changes state accordingly
			*newState = RED;
			setTime(timer_id, itime, 0.5);
			changeDisplayOnRamp('R', rampID);
		}
		break;
	case GREEN:
		if (isActive(rampID, newState, timer_id, itime)) {
			printf("GREEN\n");
			*newState = YELLOW;
			setTime(timer_id, itime, 0.5);
			changeDisplayOnRamp('G', rampID);
		}
		break;
	case YELLOW:
		printf("YELLOW\n");
		*newState = RED;
		setTime(timer_id, itime, 0.5);
		changeDisplayOnRamp('Y', rampID);
		break;
	case RED:
		if (isActive(rampID, newState, timer_id, itime)) {
			printf("RED\n");
			*newState = GREEN;
			setTime(timer_id, itime, 3);
			changeDisplayOnRamp('R', rampID);
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
