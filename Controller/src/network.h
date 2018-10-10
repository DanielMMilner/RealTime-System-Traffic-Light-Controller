#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#define BUF_SIZE 300
#define LOCAL_ATTACH_POINT "controller2"
#define MAX_CONNECT_ATTEMPT_COUNT 20

typedef enum {
	CID_CONTROLLER = 0, CID_FREEWAY = 1, CID_MALVERN = 2, CID_TRAIN = 3, CID_PEDESTRAIN = 4
} Client_ID;

static char *CLIENT_NAMES[5] = { "Controller Node", "Freeway Node", "Malvern Intersection Node", "Train Node",
        "Pedestrian Node" };

typedef enum
{
	COMMAND_GET_STATE,
	COMMAND_TOGGLE_SENSOR
}Node_Commands;

typedef struct {
	Client_ID id;
	char QNET_name[BUF_SIZE];
} Client_typedef;


static char *COMMAND_STRS[2] = {"Get State", "Set Sensor"};

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


#endif /* SRC_NETWORK_H_ */
