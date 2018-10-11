/*
 * FreewayIntersection.h
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#ifndef SRC_FREEWAYINTERSECTION_H_
#define SRC_FREEWAYINTERSECTION_H_

#include <stdlib.h>
#include <stdio.h>
#include "TimerHandler.h"
#include "Sensors.h"

enum mainIntersectionStates {
	RRRRRRRR,
	GGGGRRRR,
	YGYGRRRR,
	RGRGRRGG,
	RGRGRRYY,
	RYRYRRRR,
	RRRRGGRR,
	RRRRYYRR,
	RYRGGRRR,
	RRRGGRRR,
	RRRGYRRR,
	RGRYRRRR,
	RGRRGRRR,
	RGRRRYRR
};

enum freewaySensors {
	NE_Waiting = 1,
	SW_Waiting = 2,
	ES_Waiting = 3,
	WN_Waiting = 4,
	Use_Sensors = 5
};

enum mainIntersectionStates CurState;

extern TurningSensors turningSensors;

void changeMainIntersectionState(enum mainIntersectionStates *newState,
		timer_t *timer_id, struct itimerspec *itime);

void *mainIntersectionStateMachine();

void *userInput();

int FreewayIntersection();

#endif /* SRC_FREEWAYINTERSECTION_H_ */
