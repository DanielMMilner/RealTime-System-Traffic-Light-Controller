/*
 * OnRamp.c
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#include "OnRamp.h"

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime, int rampID) {
	switch (*newState) {
	case OFF:
		if (rampID == 0) {
			if (getSensorValue(East_Onramp)) {
				*newState = GREEN;
				setTime(timer_id, itime, 2);
			} else {
				printf("East ramp if Off\n");
				setTime(timer_id, itime, 3);
			}
		} else {
			if (getSensorValue(West_Onramp)) {
				*newState = GREEN;
				setTime(timer_id, itime, 2);
			} else {
				printf("West ramp if Off\n");
				setTime(timer_id, itime, 3);
			}
		}
		break;
	case GREEN:
		printf("GREEN\n");
		*newState = YELLOW;
		setTime(timer_id, itime, 0.5);
		break;
	case YELLOW:
		printf("YELLOW\n");
		*newState = RED;
		setTime(timer_id, itime, 0.5);
		break;
	case RED:
		if (!getSensorValue(East_Onramp) && rampID == 0) {
			*newState = OFF;
			setTime(timer_id, itime, 3);
		} else if (!getSensorValue(West_Onramp) && rampID == 1) {
			*newState = OFF;
			setTime(timer_id, itime, 3);
		} else {
			printf("RED\n");
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
