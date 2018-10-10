#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "malv_timer.h"
#include "malv_sensors.h"
#include "malv_statemachine.h"


int main(void) {
	// state variables
    timer_container tc;
    sensor_state sen;
    light_state state = State1;

    int boom_gate = 0;

    // initialisation
    configure_timer(&tc);
    configure_sensor(&sen);

    pthread_t th_sensor;
    pthread_create(&th_sensor, NULL, sensor_thr, &sen);

    // main control loop
    while(1)
    {
        if(timer_done(tc.chid))
        {
            print_light_state(&state);
            light_state_machine(&state, sen, boom_gate, &tc);

            timer_settime(tc.timer_id, 0, tc.itime_current, NULL);
        }
    }

    pthread_join(th_sensor, NULL);

	return EXIT_SUCCESS;
}

