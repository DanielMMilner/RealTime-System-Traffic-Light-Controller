/*
 * MQueues.h
 *
 *  Created on: Oct 16, 2018
 *      Author: s3542977
 */

#ifndef MQUEUES_H_
#define MQUEUES_H_

#include <stdlib.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>

#include "Sensors.h"
#include "LCDdisplay.h"

#define  MESSAGESIZE 1000

#define Q_FLAGS O_RDWR | O_CREAT
#define Q_Mode S_IRUSR | S_IWUSR

void *OnRampCommunicationSend();

void *OnRampCommunicationRecieve();


#endif /* MQUEUES_H_ */
