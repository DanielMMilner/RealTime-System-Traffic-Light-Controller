#include "server.h"

// Thread for the server
void *server_thread() {
	name_attach_t *attach;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, LOCAL_ATTACH_POINT, 0)) == NULL) {
		printf("\nFailed to name_attach on ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);
		printf("\n Possibly another server with the same name is already running !\n");
		return EXIT_FAILURE;
	}

	//printf("Server Listening for Clients on ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);

	Send_header msg;
	int rcvid = 0;
	int living = 0;

	Response_header replymsg; // replymsg structure for sending back to client

	living = 1;
	while (living) {
		// Wait for msg to be received.
		rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), NULL);

		if (rcvid == -1)  // Error condition, exit
		        {
			printf("\nFailed to MsgReceive\n");
			break;
		}

		// Determine if a pulse or message has been received.
		if (rcvid == 0) {
			switch (msg.hdr.code) {
			case _PULSE_CODE_DISCONNECT:
				// A client disconnected
				ConnectDetach(msg.hdr.scoid);
				printf("\nServer was told to Detach from ClientID:%d ...\n", msg.ClientID);
				break;

			default:
				// Some other pulse sent by one of your processes or the kernel
				//printf("\nServer got some other pulse\n");
				break;

			}
			continue; // go back to top of while loop
		}

		// for messages:
		if (rcvid > 0) // if true then A message was received
		        {

			// If the Global Name Service (gns) is running, name_open() sends a connect message. The server must EOK it.
			if (msg.hdr.type == _IO_CONNECT) {
				MsgReply(rcvid, EOK, NULL, 0);
				printf("\n gns service is running....");
				continue;	// go back to top of while loop
			}

			// Some other I/O message was received; reject it
			if (msg.hdr.type > _IO_BASE && msg.hdr.type <= _IO_MAX) {
				MsgError(rcvid, ENOSYS);
				printf("\n Server received and IO message and rejected it....");
				continue;	// go back to top of while loop
			}

			printf("Message received from: %s with command '%s' and data '%d' and '%d' and '%d'\n", CLIENT_NAMES[msg.ClientID], COMMAND_STRS[(int)msg.command], msg.data1, msg.data2, msg.data3);

			// Process the data and res given the command
			switch(msg.command)
			{
			case COMMAND_GET_STATE:

				break;
			case COMMAND_TOGGLE_SENSOR:

				// PROCESS SENSOR REQUEST HERE
				// msg.data contains the sensor number
			case COMMAND_CHANGE_LIGHT_TIMING:

				break;

			case COMMAND_CHANGE_LIGHT_PATTERN:

				break;
			}

			// Server responds to msg.
			MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));

		} else {
			printf("\nERROR: Server received something, but could not handle it correctly\n");
		}

	}

	name_detach(attach, 0);

	return EXIT_SUCCESS;
}
