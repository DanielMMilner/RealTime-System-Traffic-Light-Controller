#include "malv_statemachine.h"
#include <stdio.h>
#include <stdlib.h>
#include "LCDdisplay.h"

void print_state(light_data *ld)
{
	LCD_connect *lcd = &ld->lcd_data;

	sensor_state *sen = &ld->sensor_data;

	pthread_mutex_lock(&lcd->mutex);

    switch (ld->state)
    {
    case State1:
        printf("RRRRRRR (State 1)");
        sprintf((char*)&lcd->LCDdata1, "RRRRRRR");
        break;
    case State2:
        printf("RRRRRGG (State 2)");
        sprintf((char*)&lcd->LCDdata1, "RRRRRGG");
        break;
    case State3:
        printf("RRRRRYG (State 3)");
        sprintf((char*)&lcd->LCDdata1, "RRRRRYG");
        break;
    case State4:
        printf("RRRRRRG (State 4)");
        sprintf((char*)&lcd->LCDdata1, "RRRRRRG");
        break;
    case State5:
        printf("GGRRRRG (State 5)");
        sprintf((char*)&lcd->LCDdata1, "GGRRRRG");
        break;
    case State5b:
        printf("RGRRRRG (State5b)");
        sprintf((char*)&lcd->LCDdata1, "RGRRRRG");
        break;
    case State6:
        printf("YYRRRRY (State 6)");
        sprintf((char*)&lcd->LCDdata1, "YYRRRRY");
        break;
    case State6b:
        printf("RYRRRRY (State6b)");
        sprintf((char*)&lcd->LCDdata1, "RYRRRRY");
        break;
    case State7:
        printf("RRRRRRR (State 7)");
        sprintf((char*)&lcd->LCDdata1, "RRRRRRR");
        break;
    case State8:
        printf("GRRGGRR (State 8)");
        sprintf((char*)&lcd->LCDdata1, "GRRGGRR");
        break;
    case State8b:
        printf("RRRGGRR (State8b)");
        sprintf((char*)&lcd->LCDdata1, "RRRGGRR");
        break;
    case State9:
        printf("YRRYGRR (State 9)");
        sprintf((char*)&lcd->LCDdata1, "YRRYGRR");
        break;
    case State9b:
        printf("RRRYGRR (State9b)");
        sprintf((char*)&lcd->LCDdata1, "RRRYGRR");
        break;
    case State10:
        printf("RRRRGRR (State10)");
        sprintf((char*)&lcd->LCDdata1, "RRRRGRR");
        break;
    case State11:
        printf("RRGRGRR (State11)");
        sprintf((char*)&lcd->LCDdata1, "RRGRGRR");
        break;
    case State12:
        printf("RRYRYRR (State12)");
        sprintf((char*)&lcd->LCDdata1, "RRYRYRR");
        break;
    }

    printf("Sensors: ");
    printf("%u%u%u%u%u%u%u",
    		sen->sensor1,
			sen->sensor2,
			sen->sensor3,
			sen->sensor4,
			sen->sensor5,
			sen->sensor6,
			sen->sensor7);
    printf("\n");

    sprintf((char*)&lcd->LCDdata2, "Sensors: ");
    sprintf((char*)&lcd->LCDdata2, "%u%u%u%u%u%u%u",
    		sen->sensor1,
			sen->sensor2,
			sen->sensor3,
			sen->sensor4,
			sen->sensor5,
			sen->sensor6,
			sen->sensor7);

    pthread_mutex_unlock(&lcd->mutex);
}

void light_state_machine(light_state *state, sensor_state *sen, int boom_gate, timer_container *tc)
{
    switch (*state)
    {
    case State1:
        if(sen->sensor6 || sen->sensor7)
        {
            *state = State2;
            sen->sensor6 = 0;
            sen->sensor7 = 0;
            tc->itime_current = &tc->itime_red;
        }
        tc->itime_current = &tc->itime_red;
        break;
    case State2:
        *state = State3;
        tc->itime_current = &tc->itime_green;
        break;
    case State3:
        *state = State4;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State4:
        if(boom_gate)
        {
            *state = State5b;
        }
        else
        {
            *state = State5;
        }
        tc->itime_current = &tc->itime_green;
        break;
    case State5:
        if(sen->sensor1 || sen->sensor3 || sen->sensor4 || sen->sensor5 || sen->sensor6)
        {
            *state = State6;
            sen->sensor1 = 0;
            sen->sensor3 = 0;
            sen->sensor4  = 0;
            sen->sensor5 = 0;
            sen->sensor6 = 0;
            tc->itime_current = &tc->itime_green;
        }
        break;
    case State5b:
        if(sen->sensor1 || sen->sensor3 || sen->sensor4 || sen->sensor5 || sen->sensor6)
        {
            *state = State6b;
            sen->sensor1 = 0;
            sen->sensor3 = 0;
            sen->sensor4 = 0;
            sen->sensor5 = 0;
            sen->sensor6 = 0;
            tc->itime_current = &tc->itime_green;
        }
        break;
    case State6:
        *state = State7;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State6b:
        *state = State7;
        tc->itime_current = &tc->itime_yellow;
        break;
    case State7:
        if(boom_gate)
        {
            *state = State8b;
        }
        else
        {
            *state = State8;
        }
        tc->itime_current = &tc->itime_red;
        break;
    case State8:
        *state = State9;
        tc->itime_current = &tc->itime_green;
        break;
    case State8b:
        *state = State9b;
        tc->itime_current = &tc->itime_green;
        break;
    case State9:
        if(sen->sensor2 || sen->sensor3 || sen->sensor6 || sen->sensor7)
        {
        	sen->sensor2 = 0;
        	sen->sensor3 = 0;
        	sen->sensor6 = 0;
        	sen->sensor7 = 0;
            *state = State10;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    case State9b:
        if(sen->sensor1 || sen->sensor2 || sen->sensor3 || sen->sensor6 || sen->sensor7)
        {
            *state = State10;
            sen->sensor1 = 0;
            sen->sensor2 = 0;
            sen->sensor3 = 0;
            sen->sensor6 = 0;
            sen->sensor7 = 0;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    case State10:
        *state = State11;
        tc->itime_current = &tc->itime_green;
        break;
    case State11:
        *state = State12;
        tc->itime_current = &tc->itime_green;
        break;
    case State12:
        if(sen->sensor1 || sen->sensor2 || sen->sensor4 || sen->sensor6 || sen->sensor7)
        {
            *state = State1;
            sen->sensor1 = 0;
            sen->sensor2 = 0;
            sen->sensor4 = 0;
            sen->sensor6 = 0;
            sen->sensor7 = 0;
        }
        tc->itime_current = &tc->itime_yellow;
        break;
    }
}
