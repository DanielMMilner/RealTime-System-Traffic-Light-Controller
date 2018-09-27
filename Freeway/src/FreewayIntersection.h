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

typedef struct {
	int NE_Waiting;
	int SW_Waiting;
	int ES_Waiting;
	int WN_Waiting;
} TurningSensors;

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

enum mainIntersectionStates CurState;

void changeMainIntersectionState(enum mainIntersectionStates *newState, timer_t *timer_id,struct itimerspec *itime);

void *mainIntersectionStateMachine();

void *userInput();

int FreewayIntersection();

#endif /* SRC_FREEWAYINTERSECTION_H_ */
