#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>

#define BUF_SIZE 300
#define LOCAL_ATTACH_POINT "controller"

typedef enum {
	CID_CONTROLLER = 0, CID_FREEWAY = 1, CID_MALVERN = 2, CID_TRAIN = 3, CID_PEDESTRAIN = 4
} Client_ID;

static char *CLIENT_NAMES[5] = { "Controller Node", "Freeway Node", "Malvern Intersection Node", "Train Node",
        "Pedestrain Node" };

typedef struct {
	Client_ID id;
	char QNET_name[BUF_SIZE];
} Client_typedef;

typedef struct {
	struct _pulse hdr;
	int ClientID; // node client ID
	char data[BUF_SIZE]; // Message Data
} Send_header;

typedef struct {
	struct _pulse hdr; // Our real data comes after this header
	char buf[BUF_SIZE]; // Message we send back to clients to tell them the messages was processed correctly.
} Response_header;

// Thread to read from console.
int *read_thread() {

}

// Thread for the server
int *server_thread() {
	name_attach_t *attach;

	// Create a local name (/dev/name/...)
	if ((attach = name_attach(NULL, LOCAL_ATTACH_POINT, 0)) == NULL) {
		printf("\nFailed to name_attach on ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);
		printf("\n Possibly another server with the same name is already running !\n");
		return EXIT_FAILURE;
	}

	printf("Server Listening for Clients on ATTACH_POINT: %s \n", LOCAL_ATTACH_POINT);

	Send_header msg;
	int rcvid = 0;
	int living = 0;

	Response_header replymsg; // replymsg structure for sending back to client
	replymsg.hdr.type = 0x01;
	replymsg.hdr.subtype = 0x00;

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
				printf("\nServer got some other pulse\n");
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

			printf("Server received data packet with value of '%s' from client (ID:%d), ", msg.data, msg.ClientID);
			fflush(stdout);
			printf("\n    -----> replying with: '%s'\n", replymsg.buf);
			MsgReply(rcvid, EOK, &replymsg, sizeof(replymsg));

		} else {
			printf("\nERROR: Server received something, but could not handle it correctly\n");
		}

	}

	// Remove the attach point name from the file system (i.e. /dev/name/local/<myname>)
	name_detach(attach, 0);

	return EXIT_SUCCESS;
}

// Thread for each client. Many of these are running
int *client_thread(void *data) {
	Client_typedef *client = (Client_typedef*) data;
	Send_header msg;
	Response_header reply;

	msg.ClientID = (int) client->id;

	int server_coid;

	// Connect to the server
	printf("Attempting to connect to: %s at %s\n", CLIENT_NAMES[(int) client->id], client->QNET_name);

	if ((server_coid = name_open(client->QNET_name, 0)) == -1) {
		printf("\n    ERROR, unable to connect to %s!\n\n", client->QNET_name);
		return EXIT_FAILURE;
	}

	printf("Connection established to: %s\n", CLIENT_NAMES[(int) client->id]);

	// We would have pre-defined data to stuff here
	msg.hdr.type = 0x00;
	msg.hdr.subtype = 0x00;

	sprintf(msg.data, "HELLO WORLD");

	MsgSend(server_coid, &msg, sizeof(msg), &reply, sizeof(reply));

	name_close(server_coid);
	return EXIT_SUCCESS;
}

// Private vars
Client_typedef freeway = { CID_FREEWAY, "/dev/name/local/controller" };
Client_typedef malvern = { CID_MALVERN, "/dev/name/local/controller" };
//Client_typedef pedestrain = {CID_PEDESTRAIN, "/net/Beagle01.net.intra/dev/name/local/pedestrain"};
//Client_typedef train = {CID_TRAIN, "/net/Beagle01.net.intra/dev/name/local/train"};

int main(void) {
	// Read the paths from file

	pthread_t clientTHD, c2, serverTHD;
	void *retval;

	pthread_create(&serverTHD, NULL, server_thread, NULL);
	sleep(1);
	pthread_create(&clientTHD, NULL, client_thread, &freeway);
	pthread_create(&c2, NULL, client_thread, &malvern);

	pthread_join(c2, &retval);
	pthread_join(clientTHD, &retval);
	pthread_join(serverTHD, &retval);

	printf("DONE");
	return EXIT_SUCCESS;
}
