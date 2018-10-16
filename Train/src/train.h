/*
 * main.h
 *
 *  Created on: 15Oct.,2018
 *      Author: s3542623
 */

#ifndef TRAIN_H_
#define TRAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/iofunc.h>
#include <sys/netmgr.h>
#include <sys/neutrino.h>

#include "stdint.h"
#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <unistd.h>
#include <sched.h>

#include "lcd.h"

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL

#define BUFFER_SIZE 100
#define MESSAGE_SIZE 2

#define TIMETABLE_FILE "train_times.info"

#define DATA_SEND 0x40  // sets the Rs value high
#define Co_Ctrl   0x00  // mode to tell LCD we are sending a single command

#define BD_DURATION 3
#define B_CHANGE_DURATION 2

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
	char sensor_data[MESSAGE_SIZE]; // ?
	int timetable[2][7][30];

	// LCD data
	int fd;
	uint8_t Address;
	uint8_t mode;

	pthread_mutex_t mutex;
} app_data;

typedef union {
	struct _pulse pulse;
} my_message_t;

#endif /* TRAIN_H_ */
