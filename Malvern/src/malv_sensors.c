#include <stdio.h>
#include <stdlib.h>

#include "malv_sensors.h"

void configure_sensor(sensor_state *sen)
{
	sen->sensor1 = 0;
	sen->sensor2 = 0;
	sen->sensor3 = 0;
	sen->sensor4 = 0;
	sen->sensor5 = 0;
	sen->sensor6 = 0;
	sen->sensor7 = 0;

	pthread_mutex_init(&sen->mutex, NULL);
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
