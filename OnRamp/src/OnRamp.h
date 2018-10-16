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
#include "LocalController.h"

enum onRampStates {
	GREEN, RED, YELLOW, OFF
};

enum rampSensors {
	East_Onramp = 0, West_Onramp = 1
};

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime, int rampID);

void *onRampStateMachine(void *isEast);

#endif /* SRC_ONRAMP_H_ */
