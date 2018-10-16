#ifndef SRC_NETWORK_H_
#define SRC_NETWORK_H_

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#define BUF_SIZE 300
#define MAX_CONNECT_ATTEMPT_COUNT 2

#define NODE_COUNT 5
#define COMMAND_COUNT 2
#define CONN_STATE_COUNT 3

typedef enum {
	CID_CONTROLLER = 0, CID_FREEWAY = 1, CID_MALVERN = 2, CID_TRAIN = 3, CID_PEDESTRAIN = 4
} Client_ID;

static char *CLIENT_NAMES[NODE_COUNT] = { "Controller Node", "Freeway Node", "Malvern Intersection Node", "Train Node",
        "Pedestrian Node" };

typedef enum
{
	COMMAND_GET_STATE,
	COMMAND_TOGGLE_SENSOR
}Node_Commands;

static char *COMMAND_STRS[COMMAND_COUNT] = {"Get State", "Set Sensor"};

typedef enum
{
	CONN_STATE_DISCONNECTED,
	CONN_STATE_CONNECTED,
	CONN_STATE_TIMEOUT
}Connection_State;

static char *CONN_STATE_STRS[CONN_STATE_COUNT] = {"Disconnected", "Connected", "Timeout"};

typedef struct {
	Client_ID id;
	char QNET_name[BUF_SIZE];
	Connection_State conn_state;
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

typedef struct
{
	char current_state[20];
	char next_state[20];
	// Sensor states,
	// Current timing
}Freeway_typedef;


#endif /* SRC_NETWORK_H_ */
