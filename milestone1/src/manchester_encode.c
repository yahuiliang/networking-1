/*
 * manchester_encode.c
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#include "tim.h"

#define DOWN_SCALE   2          // TODO: put scale here
#define TIME_OUT     3          // TODO: put the time out value here
#define TIME_OUT_CLK TIM2       // TODO: make it to be the timer that is used

void configure_timeout_clk()
{
    enable_timer_clk(TIME_OUT_CLK);
    log_tim_interrupt(TIME_OUT_CLK);
    set_to_counter_mode(TIME_OUT_CLK);
    enable_counter_mode_interrupt(TIME_OUT_CLK);
    set_psc(TIME_OUT_CLK, DOWN_SCALE);
    set_arr(TIME_OUT_CLK, TIME_OUT);
    start_counter(TIME_OUT_CLK);
}

void TIM2_IRQHandler(void)
{
    // TODO: implement code to handle COLLISION state here
}

