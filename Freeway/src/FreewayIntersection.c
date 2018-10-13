/*
 * FreewayIntersection.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include "FreewayIntersection.h"

void changeStateUsingSensors(State *currentState, State sensorStates[10]) {
	if (getSensorValue(ES_Waiting) && getSensorValue(WN_Waiting)) {
		*currentState = sensorStates[RRRRGGRR];
	} else if (getSensorValue(SW_Waiting) && getSensorValue(NE_Waiting)) {
		*currentState = sensorStates[RGRGRRGG];
	} else if (getSensorValue(ES_Waiting) && getSensorValue(SW_Waiting)) {
		*currentState = sensorStates[RRRGGRRG];
	} else if (getSensorValue(NE_Waiting) && getSensorValue(WN_Waiting)) {
		*currentState = sensorStates[RGRRRGGR];
	} else if (getSensorValue(ES_Waiting)) {
		*currentState = sensorStates[GRRGGRRR];
	} else if (getSensorValue(WN_Waiting)) {
		*currentState = sensorStates[RGGRRGRR];
	} else if (getSensorValue(NE_Waiting)) {
		*currentState = sensorStates[GGRGRRGR];
	} else if (getSensorValue(SW_Waiting)) {
		*currentState = sensorStates[RGGGRRRG];
	} else {
		*currentState = sensorStates[GGGGRRRR];
	}
}

void getYellowLightState(State *currentState, State *nextState) {
	char yellowState[9] = "";

	for (int i = 0; i <= 7; i++) {
		if (currentState->stateName[i] == 'R') {
			yellowState[i] = 'R';
		} else if (currentState->stateName[i] == 'G'
				&& nextState->stateName[i] == 'R') {
			yellowState[i] = 'Y';
		} else {
			yellowState[i] = 'G';
		}
	}
	printf("%s\n", yellowState);
}

int isYellowRequired(State *currentState) {
	State *nextState = currentState->nextState;
	for (int i = 0; i <= 7; i++) {
		if (currentState->stateName[i] == 'G'
				&& nextState->stateName[i] == 'R') {
			return 1;
		}
	}
	return 0;
}

void nextStateSensor(State *currentState, int *yellowNeeded,
		State sensorStates[10]) {
	if (*yellowNeeded) {
		getYellowLightState(currentState, currentState->nextState);
		changeStateUsingSensors(currentState, sensorStates);
		*yellowNeeded = 0;
	} else {
		printf("%s\n", currentState->stateName);
		*yellowNeeded = isYellowRequired(currentState);
		if (!*yellowNeeded)
			changeStateUsingSensors(currentState, sensorStates);
	}
}

void nextStateFixed(State *currentState, int *yellowNeeded) {
	if (*yellowNeeded) {
		getYellowLightState(currentState, currentState->nextState);
		*currentState = *currentState->nextState;
		*yellowNeeded = 0;
	} else {
		printf("%s\n", currentState->stateName);
		*yellowNeeded = isYellowRequired(currentState);
		if (!*yellowNeeded)
			*currentState = *currentState->nextState;
	}
}

void setStateTime(timer_t *timer_id, struct itimerspec *itime, float length) {
	setTime(timer_id, itime, length);
}

void *mainIntersectionStateMachine() {
	my_message_t msg;
	int chid;
	int rcvid;
	struct itimerspec itime;
	timer_t timer_id;

	setUpTimer(&chid, &timer_id);

	State fixedStates[] = {
			{"RRRRRRRR", 1, &fixedStates[GGGGRRRR]}, //
			{"GGGGRRRR", 1, &fixedStates[RGRGRRGG]}, //
			{"RGRGRRGG", 1, &fixedStates[RRRRGGRR]}, //
			{"RRRRGGRR", 1, &fixedStates[GGGGRRRR]}, //
			{"GRRGGRRR", 1, &fixedStates[RGGRRGRR]},
			{"RGGRRGRR", 1, &fixedStates[GGRGRRGR]},
			{"GGRGRRGR", 1, &fixedStates[RGGGRRRG]},
			{"RGGGRRRG", 1, &fixedStates[RRRGGRRG]},
			{"RRRGGRRG", 1, &fixedStates[RGRRRGGR]},
			{"RGRRRGGR", 1, &fixedStates[GGGGRRRR]},
		};

	State sensorStates[] = {
			{"RRRRRRRR", 1, &sensorStates[GGGGRRRR]}, //
			{"GGGGRRRR", 1, &sensorStates[GGGGRRRR]}, //
			{"RGRGRRGG", 1, &sensorStates[GGGGRRRR]}, //
			{"RRRRGGRR", 1, &sensorStates[GGGGRRRR]}, //
			{"GRRGGRRR", 1, &sensorStates[GGGGRRRR]},
			{"RGGRRGRR", 1, &sensorStates[GGGGRRRR]},
			{"GGRGRRGR", 1, &sensorStates[GGGGRRRR]},
			{"RGGGRRRG", 1, &sensorStates[GGGGRRRR]},
			{"RRRGGRRG", 1, &sensorStates[GGGGRRRR]},
			{"RGRRRGGR", 1, &sensorStates[GGGGRRRR]},
		};

	State currentState = fixedStates[RRRRRRRR];
	State currentSensorState = sensorStates[RRRRRRRR];

	int yellowNeeded = 0;

	setStateTime(&timer_id, &itime, currentState.length);
	int useSensors = getSensorValue(Use_Sensors);
	int useSensorsNext = useSensors;

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				useSensorsNext = getSensorValue(Use_Sensors);

				if (useSensors != useSensorsNext) {
					// sensors have been toggled go to RRRRRRRR state before enable/disabling sensors.
					if (useSensors) {
						getYellowLightState(&currentSensorState,
								&fixedStates[RRRRRRRR]);
						currentSensorState = sensorStates[RRRRRRRR];
					} else {
						getYellowLightState(&currentState,
								&fixedStates[RRRRRRRR]);
						currentState = fixedStates[RRRRRRRR];
					}
					setStateTime(&timer_id, &itime, 1);
				} else {
					if (yellowNeeded) {
						setStateTime(&timer_id, &itime, 1);
					} else {
						setStateTime(&timer_id, &itime, currentState.length);
					}

					if (useSensors) {
						nextStateSensor(&currentSensorState, &yellowNeeded,
								sensorStates);
					} else {
						nextStateFixed(&currentState, &yellowNeeded);
					}
				}
				useSensors = useSensorsNext;
			}
		}
	}
	return EXIT_SUCCESS;
}
