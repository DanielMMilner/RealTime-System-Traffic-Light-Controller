/*
 * FreewayIntersection.c
 *
 *  Created on: 21Sep.,2018
 *      Author: Daniel
 */

#include "FreewayIntersection.h"

State *currentState;
State yellowState = {-1, "YYYYYYYY", 1, NULL};
int previousStateYellow = 0;

//use for the fixed sequence
State fixedStates[] = {
		{RRRRRRRR, "RRRRRRRR", 2, &fixedStates[GGGGRRRR]},
		{GGGGRRRR, "GGGGRRRR", 4, &fixedStates[RGRGRRGG]},
		{RGRGRRGG, "RGRGRRGG", 4, &fixedStates[RRRRGGRR]},
		{RRRRGGRR, "RRRRGGRR", 4, &fixedStates[GGGGRRRR]},
		{GRRGGRRR, "GRRGGRRR", 4, &fixedStates[RGGRRGRR]},
		{RGGRRGRR, "RGGRRGRR", 4, &fixedStates[GGRGRRGR]},
		{GGRGRRGR, "GGRGRRGR", 4, &fixedStates[RGGGRRRG]},
		{RGGGRRRG, "RGGGRRRG", 4, &fixedStates[RRRGGRRG]},
		{RRRGGRRG, "RRRGGRRG", 4, &fixedStates[RGRRRGGR]},
		{RGRRRGGR, "RGRRRGGR", 4, &fixedStates[GGGGRRRR]},
	};

//used for sensors
State sensorStates[] = {
		{RRRRRRRR, "RRRRRRRR", 2, &sensorStates[GGGGRRRR]},
		{GGGGRRRR, "GGGGRRRR", 4, &sensorStates[GGGGRRRR]},
		{RGRGRRGG, "RGRGRRGG", 4, &sensorStates[GGGGRRRR]},
		{RRRRGGRR, "RRRRGGRR", 4, &sensorStates[GGGGRRRR]},
		{GRRGGRRR, "GRRGGRRR", 4, &sensorStates[GGGGRRRR]},
		{RGGRRGRR, "RGGRRGRR", 4, &sensorStates[GGGGRRRR]},
		{GGRGRRGR, "GGRGRRGR", 4, &sensorStates[GGGGRRRR]},
		{RGGGRRRG, "RGGGRRRG", 4, &sensorStates[GGGGRRRR]},
		{RRRGGRRG, "RRRGGRRG", 4, &sensorStates[GGGGRRRR]},
		{RGRRRGGR, "RGRRRGGR", 4, &sensorStates[GGGGRRRR]},
	};

void changeTiming(int sensorsTimings, int stateID, int newTime){
	if(stateID > 0 && stateID < 10){
		if(sensorsTimings){
			sensorStates[stateID].length = (int)newTime;
		}else{
			fixedStates[stateID].length = (int)newTime;
		}
	}else{
		printf("Error! No state with that ID.");
	}
}

void changePattern(int stateID, int nextStateID){
	if(stateID > 0 && stateID < 10){
		fixedStates[stateID].nextState = &fixedStates[nextStateID];
	}else{
		printf("Error! No state with that ID.");
	}
}

char* getCurrentState(){
	if(previousStateYellow){
		return yellowState.stateName;
	}else{
		return currentState->stateName;
	}
}

void changeStateUsingSensors(State *currentState, State sensorStates[10]) {
	//current sensor states
	int ES_Sensor = getSensorEnabled(ES_Waiting);
	int WN_Sensor = getSensorEnabled(WN_Waiting);
	int SW_Sensor = getSensorEnabled(SW_Waiting);
	int NE_Sensor = getSensorEnabled(NE_Waiting);

	//find if a sensor has been activated and change state accordingly.
	if (ES_Sensor && WN_Sensor) {
		currentState->nextState = &sensorStates[RRRRGGRR];
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (SW_Sensor && NE_Sensor) {
		currentState->nextState = &sensorStates[RGRGRRGG];
		changeSensor(&turningSensors.SW_Waiting, 0);
		changeSensor(&turningSensors.NE_Waiting, 0);
	} else if (ES_Sensor && SW_Sensor) {
		currentState->nextState = &sensorStates[RRRGGRRG];
		changeSensor(&turningSensors.ES_Waiting, 0);
		changeSensor(&turningSensors.SW_Waiting, 0);
	} else if (NE_Sensor && WN_Sensor) {
		currentState->nextState = &sensorStates[RGRRRGGR];
		changeSensor(&turningSensors.NE_Waiting, 0);
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (ES_Sensor) {
		currentState->nextState = &sensorStates[GRRGGRRR];
		changeSensor(&turningSensors.ES_Waiting, 0);
	} else if (WN_Sensor) {
		currentState->nextState = &sensorStates[RGGRRGRR];
		changeSensor(&turningSensors.WN_Waiting, 0);
	} else if (NE_Sensor) {
		currentState->nextState = &sensorStates[GGRGRRGR];
		changeSensor(&turningSensors.NE_Waiting, 0);
	} else if (SW_Sensor) {
		currentState->nextState = &sensorStates[RGGGRRRG];
		changeSensor(&turningSensors.SW_Waiting, 0);
	} else {
		currentState->nextState = &sensorStates[GGGGRRRR];
	}
}

void getYellowLightState(State *currentState, State *nextState, State *yellowState) {
	char yellowStateName[9] = "";

	//create yellow light state
	for (int i = 0; i <= 7; i++) {
		if (currentState->stateName[i] == 'R') {
			yellowStateName[i] = 'R';
		} else if (currentState->stateName[i] == 'G'
				&& nextState->stateName[i] == 'R') {
			yellowStateName[i] = 'Y';
		} else {
			yellowStateName[i] = 'G';
		}
	}

	yellowState->nextState = nextState;
	strcpy(yellowState->stateName, yellowStateName);
}

int isYellowRequired(State *currentState) {
	if(currentState->id == currentState->nextState->id)
		return 0;
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

	int isRedState = 1;
	int useSensors = getSensorEnabled(Use_Sensors);
	int useSensorsNext = useSensors;
	int change = 0;

	if(useSensors){
		currentState = &sensorStates[RRRRRRRR];
	}else{
		currentState = &fixedStates[RRRRRRRR];
	}

	setStateTime(&timer_id, &itime, currentState->length);

	changeDisplayUseSensors(getSensorEnabled(Use_Sensors));

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0 && msg.pulse.code == MY_PULSE_CODE) {
			useSensorsNext = getSensorEnabled(Use_Sensors);

			if (useSensors != useSensorsNext || change) {
				if(previousStateYellow){
					previousStateYellow = 0;
					printf("%s %2.2f\n", currentState->stateName, currentState->length);
					changeDisplayIntersection(currentState->stateName);
					if(!isYellowRequired(currentState)){
						currentState = currentState->nextState;
					}
					change = 1;
					//set timer based on current state
					float length = previousStateYellow ? 1 : currentState->length;
					setStateTime(&timer_id, &itime, length);
				}else{
					// sensors have been enabled/disabled. Go to RRRRRRRR state before enable/disabling sensors.
					getYellowLightState(currentState, &fixedStates[RRRRRRRR], &yellowState);
					printf("%s %2.2f\n", yellowState.stateName, yellowState.length);

					changeDisplayIntersection(yellowState.stateName);

					//reset to default state.
					currentState = useSensorsNext ? &sensorStates[RRRRRRRR] : &fixedStates[RRRRRRRR];
					isRedState = 1;
					change = 0;

					changeDisplayUseSensors(useSensorsNext);
				}
			} else {
				if (useSensors) {
					//checks if a sensor has been activated and changes state accordingly.
					if(currentState->id == GGGGRRRR && !previousStateYellow)
						changeStateUsingSensors(currentState, sensorStates);
				}

				if(!previousStateYellow && isYellowRequired(currentState)){
					getYellowLightState(currentState, currentState->nextState, &yellowState);
					currentState = &yellowState;
					printf("%s %2.2f\n", currentState->stateName, currentState->length);
					changeDisplayIntersection(currentState->stateName);
					previousStateYellow = 1;
					currentState = currentState->nextState;
				}else{
					previousStateYellow = 0;
					printf("%s %2.2f\n", currentState->stateName, currentState->length);
					changeDisplayIntersection(currentState->stateName);
					if(!isYellowRequired(currentState)){
						currentState = currentState->nextState;
					}
				}

				if(isRedState){
					previousStateYellow = 1;
					isRedState = 0;
				}
			}

			//set timer based on current state
			float length = previousStateYellow ? 2 : currentState->length;
			setStateTime(&timer_id, &itime, length);

			useSensors = useSensorsNext;
		}
	}
	return EXIT_SUCCESS;
}
