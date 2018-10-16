/*
 * pedestrian.h
 *
 *  Created on: 16Oct.,2018
 *      Author: s3542623
 */

#ifndef SRC_PEDESTRIAN_H_
#define SRC_PEDESTRIAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include "lcd.h"
#include "server.h"
#include "Sensors.h"
#include "client.h"

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

enum states {
	RED, FLASH_RED, GREEN, WAITING, ERR
};

typedef union {
	struct _pulse pulse;
} my_message_t;

typedef struct {
	// state machine data
	int state;
	int sensor;
	timer_t timer_id;

	// LCD data
	int fd;
	uint8_t Address;
	uint8_t mode;

	pthread_mutex_t mutex;
} app_data;

void changeState(void);

#endif /* SRC_PEDESTRIAN_H_ */
