#include <stdio.h>
#include <stdlib.h>

#include "pedestrian.h"

struct itimerspec itime_default;
struct itimerspec itime_red;
struct itimerspec itime_green;
struct itimerspec itime_flash;

void pedestrian_sm(enum states *curr_state, app_data *data) {

	app_data *appdata = (app_data*) data;
	uint8_t	LCDdata[10] = {};

	switch (*curr_state) {

	case RED:

		printf("Current State: RED(0)\n");

		pthread_mutex_lock(&appdata->mutex);

		appdata->state = curr_state;

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"RED");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		if (appdata->sensor) {
			*curr_state = GREEN;
		}

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(appdata->timer_id, 0, &itime_red, NULL);
		break;
	case GREEN:

		printf("Current State: GREEN(1)\n");

		pthread_mutex_lock(&appdata->mutex);

		appdata->state = curr_state;

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"GREEN");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		*curr_state = FLASH_RED;

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(appdata->timer_id, 0, &itime_green, NULL);
		break;

		break;

	case FLASH_RED:

		printf("Current State: FLASHING_RED(2)\n");

		pthread_mutex_lock(&appdata->mutex);

		appdata->state = curr_state;

		SetCursor(appdata->fd, appdata->Address,0,0); // set cursor on LCD to first position first line
		sprintf(LCDdata,"FLASH RED");
		I2cWrite_(appdata->fd, appdata->Address, DATA_SEND, &LCDdata[0], sizeof(LCDdata));		// write new data to I2C

		*curr_state = RED;

		pthread_mutex_unlock(&appdata->mutex);

		timer_settime(appdata->timer_id, 0, &itime_flash, NULL);
		break;

	case ERR:

		break;
	}

}

int main(void) {

	printf("Boom Gate Node Running.\n");

	enum states curr_state = RED;

	char *prog_name = "pedestrian.c";

	app_data data;

	data.state = curr_state;
	data.sensor = 0;

	pthread_mutex_init(&data.mutex, NULL);

	init_LCD(&data);

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

	if (timer_create(CLOCK_REALTIME, &event, &data.timer_id) == -1) {
		printf(stderr, "%s: couldn't create a timer, errno %d\n", prog_name,
		errno);
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	itime_default.it_value.tv_sec = 1;
	itime_default.it_value.tv_nsec = 0;
	itime_default.it_interval.tv_sec = 0;
	itime_default.it_interval.tv_nsec = 0;

	itime_red.it_value.tv_sec = 5;
	itime_red.it_value.tv_nsec = 0;
	itime_red.it_interval.tv_sec = 0;
	itime_red.it_interval.tv_nsec = 0;

	itime_green.it_value.tv_sec = 3;
	itime_green.it_value.tv_nsec = 0;
	itime_green.it_interval.tv_sec = 0;
	itime_green.it_interval.tv_nsec = 0;

	itime_flash.it_value.tv_sec = 3;
	itime_flash.it_value.tv_nsec = 0;
	itime_flash.it_interval.tv_sec = 0;
	itime_flash.it_interval.tv_nsec = 0;

	timer_settime(data.timer_id, 0, &itime_default, NULL);

	while (1) {
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);

		if (rcvid == 0) {
			if (msg.pulse.code == MY_PULSE_CODE) {
				pedestrian_sm(&curr_state, &data);
			}
		}
	}

	return EXIT_SUCCESS;
}
