#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include "network.h"
#include "train.h"

#define LOCAL_ATTACH_POINT "train"

int *server_thread(void *data);

#endif /* SRC_SERVER_H_ */
