#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>
#include "network.h"
#include "client.h"
#include "server.h"
#include "console.h"

Client_typedef freeway = { CID_FREEWAY, "/net/RMIT_BBB_v5_03/dev/name/local/freeway" };
Client_typedef malvern = { CID_MALVERN, "/dev/name/local/controller" };
Client_typedef pedestrian = {CID_PEDESTRAIN, "/dev/name/local/controller"};
Client_typedef train = {CID_TRAIN, "/net/Beagle01.net.intra/dev/name/local/train"};

// Private vars
Client_typedef * clients[4];

int main(void) {
	// Populate the client struct

	clients[0] = &freeway;
	clients[1] = &malvern;
	clients[2] = &pedestrian;
	clients[3] = &train;

	pthread_t freewayThread, malvernThread, pedThread, trainThread;
	pthread_t serverThread;
	pthread_t consoleThread;

	void *retval;

	pthread_create(&consoleThread, NULL, console_thread, NULL);
	pthread_create(&serverThread, NULL, server_thread, NULL);
	sleep(1);

	pthread_create(&freewayThread, NULL, client_thread, (void*)&freeway);
	pthread_create(&malvernThread, NULL, client_thread, &malvern);
	pthread_create(&pedThread, NULL, client_thread, &pedestrian);
	pthread_create(&trainThread, NULL, client_thread, &train);

	pthread_join(serverThread, &retval);
	pthread_join(freewayThread, &retval);
	pthread_join(malvernThread, &retval);
	pthread_join(pedThread, &retval);
	pthread_join(trainThread, &retval);

	printf("DONE");
	return EXIT_SUCCESS;
}
