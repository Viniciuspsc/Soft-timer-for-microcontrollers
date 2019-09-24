//Implementation of Software Timer.
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soft_timer.h"
#include "hmcu_timer.h"

/*****************************************************************************
 * Global variables.
 *****************************************************************************/
int num_timer; //number of active timers

long long tick; //incremented every 1ms by interruption of physical timer

struct soft_timer{
    uint16_t        period; //period in ms
    bool            rpt;    //repeat mode, 1- on; 0-off
    bool            active; //soft timer is running
    soft_timer_callback_t callback;
};//soft_timer_t    type

soft_timer_t timer_buffer[SOFT_TIMER_MAX_INSTANCES];

/*****************************************************************************
 * Bodies of public functions.
 *****************************************************************************/
void soft_timer_init(void){
    unsigned int ask_1, timer_n, period, rpt;
    TIMER_CTRL_REG_ADDR = 0b0000000000000011; //Timer on, and enable timer interrupts
                                                    //Presscaler 00
    printf("Press 1 to create a new soft timer\n");
    scanf("%u", &ask_1);
    if(ask_1 == 1){
        printf("Enter the period value in milliseconds\n");
        scanf("%u", &period);

        printf("Enable repeat mode? No - 0   Yes - 1\n");
        scanf("%b", &rpt);

        soft_timer_t new_timer;
        new_timer.period = period;
        new_timer.rpt = rpt;

        soft_timer_t *p_timer = &new_timer;

        soft_timer_create(&p_timer);
    }
}


void soft_timer_create(soft_timer_t**pp_timer){
    int i;

    soft_timer_t newTimer;
    newTimer.period = (**pp_timer).period +tick; //plus the current tick value to adjust the comparison in main()
    newTimer.rpt = (**pp_timer).rpt;
    soft_timer_t *p_timer_in_the_buffer;
    void timeout_cb();
    if(num_timer < SOFT_TIMER_MAX_INSTANCES){//check if there is space to add a timer
        for(i=0; i<10;i++){
            if(timer_buffer[i].period = NULL){ //Find a empty space in the buffer to create the timer
                timer_buffer[i] = newTimer;//write the new soft timer in the buffer
                num_timer++;

                p_timer_in_the_buffer = &timer_buffer[i];
                soft_timer_set(p_timer_in_the_buffer, timeout_cb, newTimer.period, newTimer.rpt);
                soft_timer_start(p_timer_in_the_buffer);
            }
        }
    }else printf("There isn't space to add the timer");
}

soft_timer_status_t soft_timer_set(soft_timer_t *p_timer, soft_timer_callback_t  timeout_cb, uint32_t reload_ms, bool repeat){
    if(reload_ms > 0 && reload_ms < SOFT_TIMER_MAX_RELOAD_MS){
        return 0; //success
     } else return 1;
}


soft_timer_status_t soft_timer_start(soft_timer_t *p_timer){
    if((*p_timer).active == 0){
        (*p_timer).active = 1;
        return 0; //success
     } else return 2; //invalid state, the timer is already active
}


soft_timer_status_t soft_timer_stop(soft_timer_t *p_timer){
    if((*p_timer).active == 1){
        (*p_timer).active = 0;
        return 0; //success
     } else return 2;//invalid state, the timer is already stopped
}


void soft_timer_destroy(soft_timer_t **pp_timer){
    if((**pp_timer).period != NULL){
        (**pp_timer).period = NULL;
        num_timer--;
        return 0; //success
     } else return 2; //invalid state, the timer is already destroyed
}

/*****************************************************************************
 * Bodies of private functions.
 *****************************************************************************/
void main(){ //running soft timers
    for(;;){
        //This function will be called when timer countdown reaches zero
        void hmcu_timer_irq_handler(void){//Reset timer (every 1ms)
            if(timer_cnt == 0){//if timer reaches zero
                timer_rld= 1;
                timer_cnt = TIMER_RLD_REG_ADDR;//fill the countdown value with 1
                tick ++;//every interrupt, tick is incremented by 1
            }
        }

        //"Tick" is incremented by 1 every 1ms
        //TIMER 0
        if(timer_buffer[0].active == 1){//if soft timer is active
            if(timer_buffer[0].period != NULL && timer_buffer[0].period == tick){//when tick = period, it will have passed the desired time in milliseconds
                //do_something()...
                //do something...
                if(timer_buffer[0].rpt == 1){ //if repeat mode is on
                    timer_buffer[0].period = timer_buffer[0].period + tick;
                }
            }
        }
        //TIMER 1
        if(timer_buffer[1].active == 1){
            if(timer_buffer[1].period != NULL && timer_buffer[1].period == tick){
                //do_something()...
                //do something...
                if(timer_buffer[1].rpt == 1){ //To repeat
                    timer_buffer[1].period = timer_buffer[1].period + tick;
                }
            }
        }
        //And so on...
        }
}
