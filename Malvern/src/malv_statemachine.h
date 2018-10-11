#ifndef SRC_MALV_STATEMACHINE_H_
#define SRC_MALV_STATEMACHINE_H_

#include "malv_sensors.h"
#include "malv_timer.h"

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

void print_state(light_state *state, sensor_state *sen);
void light_state_machine(light_state *state, sensor_state sen, int boom_gate, timer_container *tc);

#endif /* SRC_MALV_STATEMACHINE_H_ */
