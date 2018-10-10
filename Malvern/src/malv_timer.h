#ifndef SRC_MALV_TIMER_H_
#define SRC_MALV_TIMER_H_

#include <sys/neutrino.h>

//#include <errno.h>

#define MY_PULSE_CODE   _PULSE_CODE_MINAVAIL

typedef union
{
    struct _pulse   pulse;
} my_message_t;

typedef struct
{
	int chid;
    struct itimerspec *itime_current;
    struct itimerspec itime_green;
    struct itimerspec itime_yellow;
    struct itimerspec itime_red;
    timer_t           timer_id;
}timer_container;


void configure_timer(timer_container *tc);
int timer_done(int chid);

#endif /* SRC_MALV_TIMER_H_ */
