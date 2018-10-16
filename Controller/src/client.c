#include "client.h"
#include "console.h"

extern Console_Command console_command;

// Thread for each client. Many of these are running
void *client_thread(void *data) {
	Client_typedef *client = (Client_typedef*) data;
	Send_header msg;
	Response_header reply;

	msg.ClientID = (int) CID_CONTROLLER;

	int server_coid;

	//printf("Attempting to connect to: %s at %s\n", CLIENT_NAMES[(int) client->id], client->QNET_name);

	// Connect to the server

	while (1) {
		client->conn_state = CONN_STATE_DISCONNECTED;
		// Try to connect to the server on the node
		if ((server_coid = name_open(client->QNET_name, 0)) == -1) {
			sleep(1); // Wait before reconnecting again
		} else {
			//printf("Connection established to: %s\n", CLIENT_NAMES[(int) client->id]);
			client->conn_state = CONN_STATE_CONNECTED;
		}

		// Once connected complete send/reply
		while (client->conn_state == CONN_STATE_CONNECTED) {
			pthread_mutex_lock(&client->mutex);
			// Check if a msg is ready
			if(client->messageReady)
			{
				msg.command = client->command;
				msg.data1 = client->data1;
				msg.data2 = client->data2;
				msg.data3 = client->data3;
				client->messageReady = 0;
			}else{
				pthread_mutex_unlock(&client->mutex);
				continue;
			}
			pthread_mutex_unlock(&client->mutex);

			if (MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply))
					== -1) {
				client->conn_state = CONN_STATE_DISCONNECTED;
			}

			if(msg.command == COMMAND_GET_STATE)
			{
				printf("State of %s is '%s'\n", CLIENT_NAMES[client->id], reply.data);
			}else{
				printf("Command Send\n");
			}
		}
	}
	name_close(server_coid);
	return EXIT_SUCCESS;
}
