#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int sensor1;
    int sensor2;
    int sensor3;
    int sensor4;
    int sensor5;
    int sensor6;
    int sensor7;
} sensor_state;

typedef enum
{
    State1,
    State2,
    State3,
    State4,
    State5, State5b,
    State6, State6b,
    State7,
    State8, State8b,
    State9, State9b,
    State10,
    State11,
    State12
} light_state;

void print_light_state(light_state *state) {
    switch (*state) {
    case State1:
        printf("RRRRRRR\n");
        break;
    case State2:
        printf("RRRRRGG\n");
        break;
    case State3:
        printf("RRRRRYG\n");
        break;
    case State4:
        printf("RRRRRRG\n");
        break;
    case State5:
        printf("GGRRRRG\n");
        break;
    case State5b:
        printf("RGRRRRG\n");
        break;
    case State6:
        printf("YYRRRRY\n");
        break;
    case State6b:
        printf("RYRRRRY\n");
        break;
    case State7:
        printf("RRRRRRR\n");
        break;
    case State8:
        printf("GRRGGRR\n");
        break;
    case State8b:
        printf("RRRGGRR\n");
        break;
    case State9:
        printf("YRRYGRR\n");
        break;
    case State9b:
        printf("RRRYGRR\n");
        break;
    case State10:
        printf("RRRRGRR\n");
        break;
    case State11:
        printf("RRGRGRR\n");
        break;
    case State12:
        printf("RRYRYRR\n");
        break;
    }
}

void light_state_machine(light_state *state, sensor_state sen, int boom_gate) {
    switch (*state) {
    case State1:
        if(sen.sensor6 || sen.sensor7) {
            *state = State2;
        }
        break;
    case State2:
        *state = State3;
        break;
    case State3:
        *state = State4;
        break;
    case State4:
        if(boom_gate) {
            *state = State5b;
        }
        else {
            *state = State5;
        }
        break;
    case State5:
        if(sen.sensor1 || sen.sensor3 || sen.sensor4 || sen.sensor5 || sen.sensor6) {
            *state = State6;
        }
        break;
    case State5b:
        if(sen.sensor1 || sen.sensor3 || sen.sensor4 || sen.sensor5 || sen.sensor6) {
            *state = State6b;
        }
        break;
    case State6:
        *state = State7;
        break;
    case State6b:
        *state = State7;
        break;
    case State7:
        if(boom_gate) {
            *state = State8b;
        }
        else {
            *state = State8;
        }
        break;
    case State8:
        *state = State9;
        break;
    case State8b:
        *state = State9b;
        break;
    case State9:
        if(sen.sensor2 || sen.sensor3 || sen.sensor6 || sen.sensor7) {
            *state = State10;
        }
        break;
    case State9b:
        if(sen.sensor1 || sen.sensor2 || sen.sensor3 || sen.sensor6 || sen.sensor7) {
            *state = State10;
        }
        break;
    case State10:
        *state = State11;
        break;
    case State11:
        *state = State12;
        break;
    case State12:
        if(sen.sensor1 || sen.sensor2 || sen.sensor4 || sen.sensor6 || sen.sensor7) {
            *state = State1;
        }
        break;
    }
}


int main(void) {
    light_state state = State1;
    sensor_state sen = {0};
    int boom_gate = 0;
    light_state_machine(&state, sen, boom_gate);
    print_light_state(&state);
	return EXIT_SUCCESS;
}
