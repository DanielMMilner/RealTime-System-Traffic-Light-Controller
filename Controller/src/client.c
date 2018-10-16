#include "client.h"

// Thread for each client. Many of these are running
int *client_thread(void *data) {
	Client_typedef *client = (Client_typedef*) data;
	Send_header msg;
	Response_header reply;

	msg.ClientID = (int) CID_CONTROLLER;

	int server_coid;

	int connectAttemptCount;
	// Connect to the server

	while(1)
	{
		client->conn_state = CONN_STATE_DISCONNECTED;
		connectAttemptCount++;
		//printf("Atempt %d to connect to: %s at %s\n",connectAttemptCount, CLIENT_NAMES[(int) client->id], client->QNET_name);
		if ((server_coid = name_open(client->QNET_name, 0)) == -1) {
			sleep(1);
		}else{
			break;
		}
		if(connectAttemptCount >= MAX_CONNECT_ATTEMPT_COUNT)
		{
			client->conn_state = CONN_STATE_TIMEOUT;
			printf("Failed to connect to %s after %d attempts\n", client->QNET_name, connectAttemptCount);
			return EXIT_FAILURE;
		}
	}

	client->conn_state = CONN_STATE_CONNECTED;

	printf("Connection established to: %s\n", CLIENT_NAMES[(int) client->id]);

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	// Setup the command
	msg.command = COMMAND_TOGGLE_SENSOR;
	msg.data = 5;

	// Send the command
	MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply));

	// Process the response from the server/node
	printf("Res is: %d\n", reply.data);

	name_close(server_coid);
	return EXIT_SUCCESS;
}
