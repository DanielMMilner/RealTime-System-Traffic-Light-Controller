#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>
#include <pthread.h>
#include <errno.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union
{
    struct _pulse   pulse;
} my_message_t;

typedef struct
{
    struct itimerspec *itime_current;
    struct itimerspec itime_green;
    struct itimerspec itime_yellow;
    struct itimerspec itime_red;
    timer_t           timer_id;
}timer_container;

void configure_timer(int chid, timer_container *tc)
{
    struct sigevent         event;

    event.sigev_notify = SIGEV_PULSE;

    // create a connection back to ourselves for the timer to send the pulse on
    event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);

    struct sched_param th_param;
    pthread_getschedparam(pthread_self(), NULL, &th_param);
    event.sigev_priority = th_param.sched_curpriority;

    event.sigev_code = MY_PULSE_CODE;

    // create the timer, binding it to the event
    timer_create(CLOCK_REALTIME, &event, &tc->timer_id);

    // setup the timer (5s initial delay value, 5s reload interval)
    tc->itime_green.it_value.tv_sec = 5;            // 1 second
    tc->itime_green.it_value.tv_nsec = 0;
    tc->itime_green.it_interval.tv_sec = 0;         // 1 second
    tc->itime_green.it_interval.tv_nsec = 0;

    //->setup the timer (1s initial delay value, 1s reload interval)
    tc->itime_yellow.it_value.tv_sec = 1;           // 1 seconds
    tc->itime_yellow.it_value.tv_nsec = 0;
    tc->itime_yellow.it_interval.tv_sec = 0;        // 1 seconds
    tc->itime_yellow.it_interval.tv_nsec = 0;

    //->setup the timer (2s initial delay value, 2s reload interval)
    tc->itime_red.it_value.tv_sec = 2;              // 2 seconds
    tc->itime_red.it_value.tv_nsec = 0;
    tc->itime_red.it_interval.tv_sec = 0;           // 2 seconds
    tc->itime_red.it_interval.tv_nsec = 0;
}

int timer_done(int chid)
{
    my_message_t msg;

    if(MsgReceive(chid, &msg, sizeof(msg), NULL) == 0)
    {
        if (msg.pulse.code == MY_PULSE_CODE)
        {
            return 1;
        }
    }
    return 0;
}

typedef struct
{
    int sensor1:1;
    int sensor2:1;
    int sensor3:1;
    int sensor4:1;
    int sensor5:1;
    int sensor6:1;
    int sensor7:1;
    pthread_mutex_t mutex;
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
        printf("RRRRRRR (State1)\n");
        break;
    case State2:
        printf("RRRRRGG (State2)\n");
        break;
    case State3:
        printf("RRRRRYG (State3)\n");
        break;
    case State4:
        printf("RRRRRRG (State4)\n");
        break;
    case State5:
        printf("GGRRRRG (State5)\n");
        break;
    case State5b:
        printf("RGRRRRG (State5b)\n");
        break;
    case State6:
        printf("YYRRRRY (State6)\n");
        break;
    case State6b:
        printf("RYRRRRY (State6b)\n");
        break;
    case State7:
        printf("RRRRRRR (State7)\n");
        break;
    case State8:
        printf("GRRGGRR (State8)\n");
        break;
    case State8b:
        printf("RRRGGRR (State8b)\n");
        break;
    case State9:
        printf("YRRYGRR (State9)\n");
        break;
    case State9b:
        printf("RRRYGRR (State9b)\n");
        break;
    case State10:
        printf("RRRRGRR (State10)\n");
        break;
    case State11:
        printf("RRGRGRR (State11)\n");
        break;
    case State12:
        printf("RRYRYRR (State12)\n");
        break;
    }
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

void *sensor_thr(void *data) {
	sensor_state *ss = (sensor_state*)data;

	while(1) {
		int sensor_sel = 0;
		scanf("%d", &sensor_sel);

		pthread_mutex_lock(&ss->mutex);

		// toggle sensor data.
		switch(sensor_sel) {
		case 1:
			ss->sensor1 = !ss->sensor1;
			break;
		case 2:
			ss->sensor2 = !ss->sensor2;
			break;
		case 3:
			ss->sensor3 = !ss->sensor3;
			break;
		case 4:
			ss->sensor4 = !ss->sensor4;
			break;
		case 5:
			ss->sensor5 = !ss->sensor5;
			break;
		case 6:
			ss->sensor6 = !ss->sensor6;
			break;
		case 7:
			ss->sensor7 = !ss->sensor7;
			break;
		default:
			break;
			// ignore bad values
		}

		pthread_mutex_unlock(&ss->mutex);
	}
}

int main(void) {
    timer_container tc;
    int chid = ChannelCreate(0);
    tc.itime_current = &tc.itime_red;

    light_state state = State1;
    sensor_state sen = {0};
    int boom_gate = 0;

    configure_timer(chid, &tc);

    timer_settime(tc.timer_id, 0, tc.itime_current, NULL);

    while(1)
    {
        if(timer_done(chid))
        {
            print_light_state(&state);
            light_state_machine(&state, sen, boom_gate, &tc);

            timer_settime(tc.timer_id, 0, tc.itime_current, NULL);
        }
    }

	return EXIT_SUCCESS;
}

