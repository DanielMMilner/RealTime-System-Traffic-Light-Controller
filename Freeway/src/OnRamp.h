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

enum onRampStates {
	GREEN, RED, YELLOW, OFF
};

void changeOnRampState(enum onRampStates *newState, timer_t *timer_id,
		struct itimerspec *itime);

void *onRampStateMachine();

#endif /* SRC_ONRAMP_H_ */
