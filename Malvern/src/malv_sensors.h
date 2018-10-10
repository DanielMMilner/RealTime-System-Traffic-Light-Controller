#ifndef SRC_MALV_SENSORS_H_
#define SRC_MALV_SENSORS_H_

#include <pthread.h>

typedef struct
{
    unsigned int sensor1:1;
    unsigned int sensor2:1;
    unsigned int sensor3:1;
    unsigned int sensor4:1;
    unsigned int sensor5:1;
    unsigned int sensor6:1;
    unsigned int sensor7:1;
    pthread_mutex_t mutex;
} sensor_state;

void configure_sensor(sensor_state *sen);
void *sensor_thr(void *data);

#endif /* SRC_MALV_SENSORS_H_ */
