#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "malv_timer.h"
#include "malv_sensors.h"
#include "malv_statemachine.h"
#include "server.h"
#include "client.h"
#include "LCDdisplay.h"
#include "Keypad.h"

int main(void) {
	// state variables
	light_data ld = {0};
    ld.state = State1;
    int boom_gate = 0;

    Client_typedef train = {CID_TRAIN, "/net/RMIT_BBB_v5_06/dev/name/local/train"};
    pthread_mutex_init(&train.mutex, NULL);

    // initialisation
    configure_timer(&ld.timer_cont);
    configure_sensor(&ld.sensor_data);
    configureLCD(&ld.lcd_data);
    configureKeypad(&ld.keypaddata);

    pthread_t th_server;
    pthread_create(&th_server, NULL, server_thread, &ld);

    pthread_t th_train;
	pthread_create(&th_train, NULL, client_thread, &train);

    pthread_t th_lcd;
    pthread_create(&th_lcd, NULL, LCDthread, &ld.lcd_data);

    pthread_t th_keypad;
    pthread_create(&th_keypad, NULL, KeypadThread, &ld.keypaddata);

    // main control loop
    while(1)
    {

        if(timer_done(&ld.timer_cont))
        {
        	decode_keypad(&ld.sensor_data, &ld.keypaddata);
            print_state(&ld);

            pthread_mutex_lock(&ld.sensor_data.mutex);                // block for sensor measurement
            light_state_machine(&ld.state, &ld.sensor_data, boom_gate, &ld.timer_cont);
            pthread_mutex_unlock(&ld.sensor_data.mutex);

            timer_settime(ld.timer_cont.timer_id, 0, ld.timer_cont.itime_current, NULL);
        }
    }

    pthread_join(th_lcd, NULL);

	return EXIT_SUCCESS;
}

