#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

#define BUFFER_SIZE 100
#define MESSAGE_SIZE 2

enum states {
	BD, BR, BU, BL, ERR
};

typedef struct {
	struct _pulse header;
	int ClientID;
	char data[MESSAGE_SIZE];
} my_data;

typedef struct {
	struct _pulse header;
	char buffer[BUFFER_SIZE];
} my_reply;

typedef struct {
	char sensor_data[MESSAGE_SIZE];
} app_data;

typedef union {
	struct _pulse pulse;
} my_message_t;

struct itimerspec itime_default;
timer_t timer_id;

void boom_gate_sm(enum states *curr_state) {

	switch (*curr_state) {

	case BD:

		printf("Current State: BD(0)\n");

		*curr_state = BR;

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	case BR:

		printf("Current State: BR(1)\n");

		*curr_state = BU;

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	case BU:

		printf("Current State: BU(2)\n");

		*curr_state = BL;

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	case BL:

		printf("Current State: BL(3)\n");

		*curr_state = BD;

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	case ERR:

		printf("Current State: ERR(4)\n");

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	}
}

int main(void) {

	printf("Boom Gate Node Running.\n");

	enum states curr_state = BD;

	char *prog_name = "boom_gate.c";

	struct sigevent event;
	my_message_t msg;
	int chid;
	int rcvid;

	chid = ChannelCreate(0);

	event.sigev_notify = SIGEV_PULSE;

	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	if (event.sigev_coid == -1) {
		printf(stderr, "%s: couldn't ConnectAttach to self!\n", prog_name);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	struct sched_param th_param;
	pthread_getschedparam(pthread_self(), NULL, &th_param);
	event.sigev_priority = th_param.sched_curpriority;

	event.sigev_code = MY_PULSE_CODE;

	if (timer_create(CLOCK_REALTIME, &event, &timer_id) == -1) {
		printf(stderr, "%s: couldn't create a timer, errno %d\n", prog_name, errno);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	itime_default.it_value.tv_sec = 1;
	itime_default.it_value.tv_nsec = 0;
	itime_default.it_interval.tv_sec = 0;
	itime_default.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &itime_default, NULL);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				boom_gate_sm(&curr_state);
			}
		}
	}

	return EXIT_SUCCESS;
}
