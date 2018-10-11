#include "malv_statemachine.h"
#include <stdio.h>
#include <stdlib.h>

void print_state(light_state *state, sensor_state *sen) {
    switch (*state) {
    case State1:
        printf("RRRRRRR (State 1)");
        break;
    case State2:
        printf("RRRRRGG (State 2)");
        break;
    case State3:
        printf("RRRRRYG (State 3)");
        break;
    case State4:
        printf("RRRRRRG (State 4)");
        break;
    case State5:
        printf("GGRRRRG (State 5)");
        break;
    case State5b:
        printf("RGRRRRG (State5b)");
        break;
    case State6:
        printf("YYRRRRY (State 6)");
        break;
    case State6b:
        printf("RYRRRRY (State6b)");
        break;
    case State7:
        printf("RRRRRRR (State 7)");
        break;
    case State8:
        printf("GRRGGRR (State 8)");
        break;
    case State8b:
        printf("RRRGGRR (State8b)");
        break;
    case State9:
        printf("YRRYGRR (State 9)");
        break;
    case State9b:
        printf("RRRYGRR (State9b)");
        break;
    case State10:
        printf("RRRRGRR (State10)");
        break;
    case State11:
        printf("RRGRGRR (State11)");
        break;
    case State12:
        printf("RRYRYRR (State12)");
        break;
    }

    printf(" Sensors: ");
    printf("%u", sen->sensor1);
    printf("%u", sen->sensor2);
    printf("%u", sen->sensor3);
    printf("%u", sen->sensor4);
    printf("%u", sen->sensor5);
    printf("%u", sen->sensor6);
    printf("%u", sen->sensor7);

    printf("\n");
}

void light_state_machine(light_state *state, sensor_state sen, int boom_gate, timer_container *tc) {
    switch (*state) {
    case State1:
        if(sen.sensor6 || sen.sensor7) {
            *state = State2;
            tc->itime_current = &tc->itime_red;
        }
        tc->itime_current = &tc->itime_red;
        break;
    case State2:
        *state = State3;
        tc->itime_current = &tc->itime_green;
        break;
    case State3:
        *state = State4;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State4:
        if(boom_gate) {
            *state = State5b;
        }
        else {
            *state = State5;
        }
        tc->itime_current = &tc->itime_green;
        break;
    case State5:
        if(sen.sensor1 || sen.sensor3 || sen.sensor4 || sen.sensor5 || sen.sensor6) {
            *state = State6;
            tc->itime_current = &tc->itime_green;
        }
        break;
    case State5b:
        if(sen.sensor1 || sen.sensor3 || sen.sensor4 || sen.sensor5 || sen.sensor6) {
            *state = State6b;
            tc->itime_current = &tc->itime_green;
        }
        break;
    case State6:
        *state = State7;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State6b:
        *state = State7;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State7:
        if(boom_gate) {
            *state = State8b;
        }
        else {
            *state = State8;
        }
        tc->itime_current = &tc->itime_red;
        break;
    case State8:
        *state = State9;
        tc->itime_current = &tc->itime_green;
        break;
    case State8b:
        *state = State9b;
        tc->itime_current = &tc->itime_green;
        break;
    case State9:
        if(sen.sensor2 || sen.sensor3 || sen.sensor6 || sen.sensor7) {
            *state = State10;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    case State9b:
        if(sen.sensor1 || sen.sensor2 || sen.sensor3 || sen.sensor6 || sen.sensor7) {
            *state = State10;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    case State10:
        *state = State11;
        tc->itime_current = &tc->itime_green;
        break;
    case State11:
        *state = State12;
        tc->itime_current = &tc->itime_green;
        break;
    case State12:
        if(sen.sensor1 || sen.sensor2 || sen.sensor4 || sen.sensor6 || sen.sensor7) {
            *state = State1;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    }
}
