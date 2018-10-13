/*
 * FreewayIntersection.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include "FreewayIntersection.h"

void changeStateUsingSensors(State *currentState, State sensorStates[10], int *sensorActivated) {
	//used to determine if the nextState has changed.
	int previousID = currentState->nextState->id;

	//find if a sensor has been activated and change state accordingly.
	if (getSensorValue(ES_Waiting) && getSensorValue(WN_Waiting)) {
		currentState->nextState = &sensorStates[RRRRGGRR];
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (getSensorValue(SW_Waiting) && getSensorValue(NE_Waiting)) {
		currentState->nextState = &sensorStates[RGRGRRGG];
		changeSensor(&turningSensors.SW_Waiting, 0);
		changeSensor(&turningSensors.NE_Waiting, 0);
	} else if (getSensorValue(ES_Waiting) && getSensorValue(SW_Waiting)) {
		currentState->nextState = &sensorStates[RRRGGRRG];
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.SW_Waiting, 0);
	} else if (getSensorValue(NE_Waiting) && getSensorValue(WN_Waiting)) {
		currentState->nextState = &sensorStates[RGRRRGGR];
		changeSensor(&turningSensors.NE_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (getSensorValue(ES_Waiting)) {
		currentState->nextState = &sensorStates[GRRGGRRR];
		changeSensor(&turningSensors.ES_Waiting, 0);
	} else if (getSensorValue(WN_Waiting)) {
		currentState->nextState = &sensorStates[RGGRRGRR];
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (getSensorValue(NE_Waiting)) {
		currentState->nextState = &sensorStates[GGRGRRGR];
		changeSensor(&turningSensors.NE_Waiting, 0);
	} else if (getSensorValue(SW_Waiting)) {
		currentState->nextState = &sensorStates[RGGGRRRG];
		changeSensor(&turningSensors.SW_Waiting, 0);
	} else {
		currentState->nextState = &sensorStates[GGGGRRRR];
	}

	if(previousID != currentState->nextState->id){
		*sensorActivated = 4;
	}
}

void getYellowLightState(State *currentState, State *nextState) {
	char yellowState[9] = "";

	//create yellow light state
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
	//determine if a yellow light is required between the next two states
	State *nextState = currentState->nextState;
	for (int i = 0; i <= 7; i++) {
		if (currentState->stateName[i] == 'G'
				&& nextState->stateName[i] == 'R') {
			return 1;
		}
	}
	return 0;
}

int isStateChangeSafe(State *currentState, int *yellowNeeded) {
	if (*yellowNeeded) {
		//A yellow light state is needed.
		getYellowLightState(currentState, currentState->nextState);
		*yellowNeeded = 0;
		return 1;
	} else {
		//No yellow light state is needed.
		printf("%s\n", currentState->stateName);
		*yellowNeeded = isYellowRequired(currentState);
		if (!*yellowNeeded)
			return 1; 		//Yellow light state is needed before next state change.
	}
	return 0;
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

	//use for the fixed sequence
	State fixedStates[] = {
			{RRRRRRRR, "RRRRRRRR", 1, &fixedStates[GGGGRRRR]},
			{GGGGRRRR, "GGGGRRRR", 1, &fixedStates[RGRGRRGG]},
			{RGRGRRGG, "RGRGRRGG", 1, &fixedStates[RRRRGGRR]},
			{RRRRGGRR, "RRRRGGRR", 1, &fixedStates[GGGGRRRR]},
			{GRRGGRRR, "GRRGGRRR", 1, &fixedStates[RGGRRGRR]},
			{RGGRRGRR, "RGGRRGRR", 1, &fixedStates[GGRGRRGR]},
			{GGRGRRGR, "GGRGRRGR", 1, &fixedStates[RGGGRRRG]},
			{RGGGRRRG, "RGGGRRRG", 1, &fixedStates[RRRGGRRG]},
			{RRRGGRRG, "RRRGGRRG", 1, &fixedStates[RGRRRGGR]},
			{RGRRRGGR, "RGRRRGGR", 1, &fixedStates[GGGGRRRR]},
		};

	//used for sensors
	State sensorStates[] = {
			{RRRRRRRR, "RRRRRRRR", 1, &sensorStates[GGGGRRRR]},
			{GGGGRRRR, "GGGGRRRR", 2, &sensorStates[GGGGRRRR]},
			{RGRGRRGG, "RGRGRRGG", 3, &sensorStates[GGGGRRRR]},
			{RRRRGGRR, "RRRRGGRR", 3, &sensorStates[GGGGRRRR]},
			{GRRGGRRR, "GRRGGRRR", 3, &sensorStates[GGGGRRRR]},
			{RGGRRGRR, "RGGRRGRR", 3, &sensorStates[GGGGRRRR]},
			{GGRGRRGR, "GGRGRRGR", 3, &sensorStates[GGGGRRRR]},
			{RGGGRRRG, "RGGGRRRG", 3, &sensorStates[GGGGRRRR]},
			{RRRGGRRG, "RRRGGRRG", 3, &sensorStates[GGGGRRRR]},
			{RGRRRGGR, "RGRRRGGR", 3, &sensorStates[GGGGRRRR]},
		};

	int yellowNeeded = 0;
	int sensorActivated = 0;
	int useSensors = getSensorValue(Use_Sensors);
	int useSensorsNext = useSensors;
	State *currentState;

	if(useSensors){
		currentState = &sensorStates[RRRRRRRR];
	}else{
		currentState = &fixedStates[RRRRRRRR];
	}

	setStateTime(&timer_id, &itime, currentState->length);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0 && msg.pulse.code == MY_PULSE_CODE) {
			useSensorsNext = getSensorValue(Use_Sensors);

			if (useSensors != useSensorsNext) {
				// sensors have been enabled/disabled. Go to RRRRRRRR state before enable/disabling sensors.
				getYellowLightState(currentState, &fixedStates[RRRRRRRR]);

				//reset to default state.
				currentState = useSensorsNext ? &sensorStates[RRRRRRRR] : &fixedStates[RRRRRRRR];

				setStateTime(&timer_id, &itime, 1);
			} else {
				if (useSensors) {
					if(sensorActivated == 0){
						//checks if a sensor has been activated and changes state accordingly.
						changeStateUsingSensors(currentState, sensorStates, &sensorActivated);
					}

					if(sensorActivated == 4){
						//A sensor was activated
						State *tmpState;
						//print the required yellow lights
						getYellowLightState(currentState, currentState->nextState);
						sensorActivated--;

						//set the current state to the next state and reset the current state to the default
						tmpState = currentState->nextState;
						currentState->nextState = &sensorStates[GGGGRRRR];
						currentState = tmpState;
						yellowNeeded = 1;
					}else if(sensorActivated == 3){
						//print the state that the sensor activated
						printf("%s\n", currentState->stateName);
						sensorActivated--;
						yellowNeeded = 0;
					}else if(sensorActivated == 2){
						//print the required yellow lights to return to the default
						getYellowLightState(currentState, currentState->nextState);
						sensorActivated--;
						//set the current state back to the default
						currentState = currentState->nextState;
						yellowNeeded = 1;
					}else if(sensorActivated == 1){
						//print the default state
						printf("%s\n", currentState->stateName);
						sensorActivated = 0;
						yellowNeeded = 0;
					}else{
						//no sensors activated
						printf("%s\n", currentState->stateName);
						currentState = currentState->nextState;
					}

				}else if(isStateChangeSafe(currentState, &yellowNeeded)){
					//no yellow light is needed. change to next state.
					currentState = currentState->nextState;
				}

				//set timer based on current state
				float length = yellowNeeded ? 1 : currentState->length;
				setStateTime(&timer_id, &itime, length);
			}
			useSensors = useSensorsNext;
		}
	}
	return EXIT_SUCCESS;
}
