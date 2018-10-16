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
#include "LCDdisplay.h"

#define YELLOW_STATE_LENGTH		2
#define YELLOW_STATE_ID			-1

enum mainIntersectionStates {
	RRRRRRRR,
	GGGGRRRR,
	RGRGRRGG,
	RRRRGGRR,
	GRRGGRRR,
	RGGRRGRR,
	GGRGRRGR,
	RGGGRRRG,
	RRRGGRRG,
	RGRRRGGR,
};

enum boolean {
	False = 0,
	True = 1,
};

enum freewaySensors {
	NE_Waiting = 1,
	SW_Waiting = 2,
	ES_Waiting = 3,
	WN_Waiting = 4,
	Use_Sensors = 5
};

typedef struct State State;

struct State {
	int id;
	char stateName[9];
	float length;
	State *nextState;
};

extern TurningSensors turningSensors;

void changeTiming(int sensorsTimings, int stateID, int newTime);

void changePattern(int stateID, int nextStateID);

char* getCurrentState();

void changeStateUsingSensors(State *currentState, State sensorStates[]);

void getYellowLightState(State *currentState, State *nextState, State *yellowState);

int isYellowRequired(State *currentState);

void setStateTime(timer_t *timer_id, struct itimerspec *itime, float length);

void *mainIntersectionStateMachine();

#endif /* SRC_FREEWAYINTERSECTION_H_ */
