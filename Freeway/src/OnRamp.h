/*
 * OnRamp.h
 *
 *  Created on: 22Sep.,2018
 *      Author: Daniel
 */

#ifndef SRC_ONRAMP_H_
#define SRC_ONRAMP_H_

#include <stdlib.h>
#include <stdio.h>

#include "TimerHandler.h"
#include "Sensors.h"

extern TurningSensors turningSensors;

enum onRampStates {
	GREEN, RED, YELLOW, OFF
};

enum rampSensors {
	East_Onramp = 6, West_Onramp = 7
};

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime, int rampID);

void *onRampStateMachine(void *isEast);

#endif /* SRC_ONRAMP_H_ */
