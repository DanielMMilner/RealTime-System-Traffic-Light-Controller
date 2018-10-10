#include "malv_timer.h"

#include <pthread.h>
#include <sys/netmgr.h>

void configure_timer(timer_container *tc)
{
    tc->chid = ChannelCreate(0);

    struct sigevent         event;

    event.sigev_notify = SIGEV_PULSE;

    // create a connection back to ourselves for the timer to send the pulse on
    event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, tc->chid, _NTO_SIDE_CHANNEL, 0);

    struct sched_param th_param;
    pthread_getschedparam(pthread_self(), NULL, &th_param);
    event.sigev_priority = th_param.sched_curpriority;

    event.sigev_code = MY_PULSE_CODE;

    // create the timer, binding it to the event
    timer_create(CLOCK_REALTIME, &event, &tc->timer_id);

    // setup the timer (5s initial delay value, 5s reload interval)
    tc->itime_green.it_value.tv_sec = 5;            // 1 second
    tc->itime_green.it_value.tv_nsec = 0;
    tc->itime_green.it_interval.tv_sec = 0;         // 1 second
    tc->itime_green.it_interval.tv_nsec = 0;

    //->setup the timer (1s initial delay value, 1s reload interval)
    tc->itime_yellow.it_value.tv_sec = 1;           // 1 seconds
    tc->itime_yellow.it_value.tv_nsec = 0;
    tc->itime_yellow.it_interval.tv_sec = 0;        // 1 seconds
    tc->itime_yellow.it_interval.tv_nsec = 0;

    //->setup the timer (2s initial delay value, 2s reload interval)
    tc->itime_red.it_value.tv_sec = 2;              // 2 seconds
    tc->itime_red.it_value.tv_nsec = 0;
    tc->itime_red.it_interval.tv_sec = 0;           // 2 seconds
    tc->itime_red.it_interval.tv_nsec = 0;

    tc->itime_current = &tc->itime_red;
    timer_settime(tc->timer_id, 0, tc->itime_current, NULL);
}

int timer_done(int chid)
{
    my_message_t msg;

    if(MsgReceive(chid, &msg, sizeof(msg), NULL) == 0)
    {
        if (msg.pulse.code == MY_PULSE_CODE)
        {
            return 1;
        }
    }
    return 0;
}
