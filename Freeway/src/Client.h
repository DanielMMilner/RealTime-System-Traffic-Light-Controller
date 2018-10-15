/*
 * Client.h
 *
 *  Created on: Oct 11, 2018
 *      Author: s3542977
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#define LOCAL_ATTACH_POINT "freeway"

typedef enum {
	CID_CONTROLLER = 0, CID_FREEWAY = 1, CID_MALVERN = 2, CID_TRAIN = 3, CID_PEDESTRAIN = 4
} Client_ID;

typedef enum
{
	COMMAND_GET_STATE,
	COMMAND_TOGGLE_SENSOR
}Node_Commands;

typedef struct {
	Client_ID id;
	char QNET_name[300];
} Client_typedef;

typedef struct {
	struct _pulse hdr;
	int ClientID; // Id of the node sending the message
	Node_Commands command;
	int data;
} Send_header;

typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	int data;
} Response_header;


void *server_thread();

#endif /* CLIENT_H_ */
