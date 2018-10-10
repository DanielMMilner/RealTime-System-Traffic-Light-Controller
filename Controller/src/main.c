#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/dispatch.h>
#include <fcntl.h>
#include <share.h>
#include "network.h"
#include "client.h"
#include "server.h"

// Private vars
Client_typedef freeway = { CID_FREEWAY, "/net/RMIT_BBB_v5_03/dev/name/local/freeway" };
//Client_typedef malvern = { CID_MALVERN, "/dev/name/local/controller" };
//Client_typedef pedestrain = {CID_PEDESTRAIN, "/net/Beagle01.net.intra/dev/name/local/pedestrain"};
//Client_typedef train = {CID_TRAIN, "/net/Beagle01.net.intra/dev/name/local/train"};

int main(void) {
	// Read the paths from file

	pthread_t clientTHD, c2, serverTHD;
	void *retval;

	pthread_create(&serverTHD, NULL, server_thread, NULL);
	sleep(1);
	pthread_create(&clientTHD, NULL, client_thread, &freeway);

	pthread_join(c2, &retval);
	pthread_join(clientTHD, &retval);
	pthread_join(serverTHD, &retval);

	printf("DONE");
	return EXIT_SUCCESS;
}
