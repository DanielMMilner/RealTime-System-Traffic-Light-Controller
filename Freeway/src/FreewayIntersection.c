/*
 * FreewayIntersection.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include "FreewayIntersection.h"

void changeMainIntersectionState(enum mainIntersectionStates *newState,
		timer_t *timer_id, struct itimerspec *itime) {
	switch (*newState) {
	case RRRRRRRR:
		printf("RRRRRRRR 1\n");
		setTime(timer_id, itime, 1);
		*newState = GGGGRRRR;
		break;
	case GGGGRRRR:
		printf("GGGGRRRR 2\n");
		setTime(timer_id, itime, 6);
		if (getSensorValue(NE_Waiting) || getSensorValue(SW_Waiting)
				|| getSensorValue(ES_Waiting) || getSensorValue(WN_Waiting) || !getSensorValue(Use_Sensors)) {
			*newState = YGYGRRRR;
		} else {
			*newState = GGGGRRRR;
		}
		break;
	case YGYGRRRR:
		printf("YGYGRRRR 3\n");
		setTime(timer_id, itime, 2);
		*newState = RGRGRRGG;
		break;
	case RGRGRRGG:
		printf("RGRGRRGG 4\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.NE_Waiting, 0);
		changeSensor(&turningSensors.SW_Waiting, 0);
		*newState = RGRGRRYY;
		break;
	case RGRGRRYY:
		printf("RGRGRRYY 5\n");
		setTime(timer_id, itime, 2);
		if(!getSensorValue(Use_Sensors)){
			*newState = RYRYRRRR;
		}else if (!getSensorValue(ES_Waiting) && getSensorValue(WN_Waiting)) {
			*newState = RGRYRRRR;
		} else if (getSensorValue(ES_Waiting) && getSensorValue(WN_Waiting)) {
			*newState = RYRYRRRR;
		} else if (getSensorValue(ES_Waiting) && !getSensorValue(WN_Waiting)) {
			*newState = RYRGGRRR;
		} else {
			*newState = GGGGRRRR;
		}
		break;
	case RYRYRRRR:
		printf("RYRYRRRR 6\n");
		setTime(timer_id, itime, 2);
		*newState = RRRRGGRR;
		break;
	case RRRRGGRR:
		printf("RRRRGGRR 7\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
		*newState = RRRRYYRR;
		break;
	case RRRRYYRR:
		printf("RRRRYYRR 8\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	case RYRGGRRR:
		printf("RYRGGRRR 9\n");
		setTime(timer_id, itime, 2);
		*newState = RRRGGRRR;
		break;
	case RRRGGRRR:
		printf("RRRGGRRR 10\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.ES_Waiting, 0);
		*newState = RRRGYRRR;
		break;
	case RRRGYRRR:
		printf("RRRGYRRR 11\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	case RGRYRRRR:
		printf("RGRYRRRR 12\n");
		setTime(timer_id, itime, 2);
		*newState = RGRRGRRR;
		break;
	case RGRRGRRR:
		printf("RGRRGRRR 13\n");
		setTime(timer_id, itime, 3);
		changeSensor(&turningSensors.WN_Waiting, 0);
		*newState = RGRRRYRR;
		break;
	case RGRRRYRR:
		printf("RGRRRYRR 14\n");
		setTime(timer_id, itime, 2);
		*newState = GGGGRRRR;
		break;
	}
}

void *mainIntersectionStateMachine() {
	enum mainIntersectionStates newState = RRRRRRRR;
	my_message_t msg;
	int chid;
	int rcvid;
	struct itimerspec itime;
	timer_t timer_id;

	setUpTimer(&chid, &timer_id);

	changeMainIntersectionState(&newState, &timer_id, &itime);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				changeMainIntersectionState(&newState, &timer_id, &itime);
			}
		}
	}
	return EXIT_SUCCESS;
}
