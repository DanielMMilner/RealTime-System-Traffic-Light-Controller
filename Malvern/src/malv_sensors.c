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

void decode_keypad(sensor_state *ss, keypadData *kd)
{
	int key = 0;

	pthread_mutex_lock(&kd->mutex);
	if(kd->new_press)
	{
		printf("Key Pressed!! %u\n", kd->key);
		kd->new_press = 0;
		key = kd->key;
	}
	pthread_mutex_unlock(&kd->mutex);

	pthread_mutex_lock(&ss->mutex);
	// toggle sensor data
	switch(key)
	{
	case 1:
		ss->sensor1 = 1;
		break;
	case 2:
		ss->sensor2 = 1;
		break;
	case 4:
		ss->sensor3 = 1;
		break;
	case 8:
		ss->sensor4 = 1;
		break;
	case 16:
		ss->sensor5 = 1;
		break;
	case 32:
		ss->sensor6 = 1;
		break;
	case 64:
		ss->sensor7 = 1;
		break;
	default:
		break;
		// ignore bad values
	}

	pthread_mutex_unlock(&ss->mutex);
}
