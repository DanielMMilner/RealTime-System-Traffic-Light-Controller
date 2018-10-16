#ifndef SRC_SERVER_H_
#define SRC_SERVER_H_

#include "network.h"
#include "pedestrian.h"

#define LOCAL_ATTACH_POINT "pedestrian"

void *server_thread(void *data);

#endif /* SRC_SERVER_H_ */
