#include "train.h"

struct itimerspec itime_default;
struct itimerspec itime_boom_down;
struct itimerspec itime_boom_changing;
timer_t timer_id;

int train1 = 0;
int train2 = 0;

void boom_gate_sm(enum states *curr_state, app_data *data) {

	app_data *appdata = (app_data*) data;
	uint8_t	LCDdata[10] = {};

	switch (*curr_state) {

	case BD:

		printf("Current State: BD(0)\n");

		pthread_mutex_lock(&appdata->mutex);

		sprintf(appdata->state, "%s", "BD(0)");

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"DOWN");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		if (!train1 && !train2) {
			*curr_state = BR;
		}

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(timer_id, 0, &itime_boom_down, NULL);

		break;
	case BR:

		printf("Current State: BR(1)\n");

		sprintf(appdata->state, "%s", "BR(1)");

		pthread_mutex_lock(&appdata->mutex);
		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"RISING");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
		pthread_mutex_unlock(&appdata->mutex);

		*curr_state = BU;

		timer_settime(timer_id, 0, &itime_boom_changing, NULL);

		break;
	case BU:

		printf("Current State: BU(2)\n");

		sprintf(appdata->state, "%s", "BU(2)");

		pthread_mutex_lock(&appdata->mutex);

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"UP");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		// check if there is a train,
		// if so, change to BL
		// otherwise don't change state and continue.

		if (train1 || train2) {
			*curr_state = BL;
		}

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	case BL:

		printf("Current State: BL(3)\n");

		sprintf(appdata->state, "%s", "BL(3)");

		pthread_mutex_lock(&appdata->mutex);

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"LOWERING");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		pthread_mutex_unlock(&appdata->mutex);

		*curr_state = BD;

		// stay in current state for set time
		timer_settime(timer_id, 0, &itime_boom_changing, NULL);

		break;
	case ERR:

		printf("Current State: ERR(4)\n");

		pthread_mutex_lock(&appdata->mutex);

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"ERR");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(timer_id, 0, &itime_default, NULL);

		break;
	}
}

void *east_west_train_sensor(void *data) {

	app_data *appdata = (app_data*) data;
	uint8_t	LCDdata[10] = {};

	// day stuff
	int day = 0;

	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	int chid;
	int rcvid;
	my_message_t msg;
	struct sched_param scheduling_params;
	int prio;

	chid = ChannelCreate(0);

	/* Get our priority. */
	if (SchedGet(0, 0, &scheduling_params) != -1) {
		prio = scheduling_params.sched_priority;
	} else {
		prio = 10;
	}

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid,
	_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = prio;
	event.sigev_code = MY_PULSE_CODE;
	timer_create(CLOCK_MONOTONIC, &event, &timer_id);

	itime.it_value.tv_sec = 1;
	itime.it_value.tv_nsec = 0;
	itime.it_interval.tv_sec = 1;
	itime.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &itime, NULL);

	struct timespec mono_time;
	time_t time_of_day;

	time_of_day = time(NULL);

	int next_train_index = 0;
	int timer_count = 0;

	int leave_time = 0;
	int next_train = 0;

	for (;;) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) { /* we got a pulse */
			if (msg.pulse.code == MY_PULSE_CODE) {
				if (clock_gettime( CLOCK_REALTIME, &mono_time) == -1) {
					perror("clock gettime");
					pthread_exit(EXIT_FAILURE);
				}

				next_train = appdata->timetable[0][day][next_train_index] - timer_count;

				pthread_mutex_lock(&appdata->mutex);
				SetCursor(appdata->fd, appdata->Address,0,10); // set cursor on LCD to first position first line
				sprintf(LCDdata,"%d mins", next_train);
				I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
				pthread_mutex_unlock(&appdata->mutex);

				if (timer_count == leave_time) {
					printf("Leave time: %d\n", leave_time);
					pthread_mutex_lock(&appdata->mutex);
					train1 = 0;
					pthread_mutex_unlock(&appdata->mutex);
				}

				timer_count = mono_time.tv_sec - time_of_day;

				if (timer_count == appdata->timetable[0][day][next_train_index]) {
					printf("Train here!\n");

					leave_time = timer_count + BD_DURATION + B_CHANGE_DURATION;

					pthread_mutex_lock(&appdata->mutex);
					train1 = 1;
					pthread_mutex_unlock(&appdata->mutex);

					next_train_index++;
				}
			}
		}
	}

	pthread_exit(EXIT_SUCCESS);
}

void *west_east_train_sensor(void *data) {

	app_data *appdata = (app_data*) data;
	uint8_t	LCDdata[10] = {};

	// day stuff
	int day = 0; // TODO: make dynamic

	struct sigevent event;
	struct itimerspec itime;
	timer_t timer_id;
	int chid;
	int rcvid;
	my_message_t msg;
	struct sched_param scheduling_params;
	int prio;

	chid = ChannelCreate(0);

	/* Get our priority. */
	if (SchedGet(0, 0, &scheduling_params) != -1) {
		prio = scheduling_params.sched_priority;
	} else {
		prio = 10;
	}

	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid,
	_NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = prio;
	event.sigev_code = MY_PULSE_CODE;
	timer_create(CLOCK_MONOTONIC, &event, &timer_id);

	itime.it_value.tv_sec = 1;
	itime.it_value.tv_nsec = 0;
	itime.it_interval.tv_sec = 1;
	itime.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &itime, NULL);

	struct timespec mono_time;
	time_t time_of_day;

	time_of_day = time(NULL);

	int next_train_index = 0;
	int timer_count = 0;

	int leave_time = 0;
	int next_train = 0;

	for (;;) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) { /* we got a pulse */
			if (msg.pulse.code == MY_PULSE_CODE) {
				if (clock_gettime( CLOCK_REALTIME, &mono_time) == -1) {
					perror("clock gettime");
					pthread_exit(EXIT_FAILURE);
				}

				next_train = appdata->timetable[1][day][next_train_index] - timer_count;

				pthread_mutex_lock(&appdata->mutex);
				SetCursor(appdata->fd, appdata->Address,1,10); // set cursor on LCD to first position first line
				sprintf(LCDdata,"%d mins", next_train);
				I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C
				pthread_mutex_unlock(&appdata->mutex);

				if (timer_count == leave_time) {
					printf("Leave time: %d\n", leave_time);
					pthread_mutex_lock(&appdata->mutex);
					train2 = 0;
					pthread_mutex_unlock(&appdata->mutex);
				}

				timer_count = mono_time.tv_sec - time_of_day;

				if (timer_count == appdata->timetable[1][day][next_train_index]) {
					printf("Train here!\n");

					leave_time = timer_count + BD_DURATION + B_CHANGE_DURATION;

					pthread_mutex_lock(&appdata->mutex);
					train2 = 1;
					pthread_mutex_unlock(&appdata->mutex);

					next_train_index++;
				}
			}
		}
	}

	pthread_exit(EXIT_SUCCESS);
}

int load_timetable(app_data *data) {
	FILE *fp;
	char buffer[80];
	char *ret;

	char *p;
	char *delim = { "," };

	int line_num = 0;
	int train_counter = 0;
	int day_counter = 0;
	int time_counter = 0;

	fp = fopen(TIMETABLE_FILE, "r");
	if (fp == NULL) {
		perror("fopen()");
		printf("Unable to load timetable file, exiting...");
		return -1;
	}

	while (1) {
		ret = fgets(buffer, 80, fp);

		if (ret == NULL) {
			if (feof(fp)) {
				// We're at the end of the file.
				break;
			} else {
				// An error occurred.
				perror("fgets()");
				printf("Unable to load timetable file, exiting...");
				return -1;
			}
		}

		p = strtok(buffer, delim);
		while (p != NULL) {
			data->timetable[train_counter][day_counter][time_counter] = strtol(p, NULL, 10);
			p = strtok(NULL, delim);
			time_counter++;
		}

		line_num++;

		time_counter = 0;
		if (line_num % 2 == 0) {
			day_counter++;
		}
		train_counter = train_counter == 0 ? 1 : 0;
	}
}

int main(void) {

	printf("Boom Gate Node Running.\n");

	enum states curr_state = BU;

	char *prog_name = "boom_gate.c";

	app_data data;

	sprintf(data.state, "%s", "BU(2)");

	// Create the mutex
	pthread_mutex_init(&data.mutex, NULL); // pass NULL as the attr parameter to use the default attributes for the mutex

	if (load_timetable(&data) == -1) {
		return EXIT_FAILURE;
	}

	init_LCD(&data);

	pthread_t east_west_train_thread;
	pthread_t west_east_train_thread;
	pthread_t s_thread;

	pthread_create(&s_thread, NULL, server_thread, &data);
	pthread_create(&east_west_train_thread, NULL, east_west_train_sensor,
			&data);
	pthread_create(&west_east_train_thread, NULL, west_east_train_sensor,
			&data);

	struct sigevent event;
	my_message_t msg;
	int chid;
	int rcvid;

	chid = ChannelCreate(0);

	event.sigev_notify = SIGEV_PULSE;

	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL,
			0);
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
		printf(stderr, "%s: couldn't create a timer, errno %d\n", prog_name,
		errno);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	itime_default.it_value.tv_sec = 1;
	itime_default.it_value.tv_nsec = 0;
	itime_default.it_interval.tv_sec = 0;
	itime_default.it_interval.tv_nsec = 0;

	itime_boom_down.it_value.tv_sec = BD_DURATION;
	itime_boom_down.it_value.tv_nsec = 0;
	itime_boom_down.it_interval.tv_sec = 0;
	itime_boom_down.it_interval.tv_nsec = 0;

	itime_boom_changing.it_value.tv_sec = B_CHANGE_DURATION;
	itime_boom_changing.it_value.tv_nsec = 0;
	itime_boom_changing.it_interval.tv_sec = 0;
	itime_boom_changing.it_interval.tv_nsec = 0;

	timer_settime(timer_id, 0, &itime_default, NULL);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				boom_gate_sm(&curr_state, &data);
			}
		}
	}

	return EXIT_SUCCESS;
}
