/*
 * manchester_encode.c
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#include "manchester_encode.h"
#include "tim.h"
#include "led.h"
#include "gpio.h"

#define DOWN_SCALE   1000
#define TIME_OUT     16000
#define TIME_OUT_CLK TIM2
#define MIN_TIM_GAP  12

static void configure_timeout_clk();
static void handle_edge_detection();
static void handle_timeout();

void manchester_init()
{
    enable_af_mode(A, 5, 1);
    configure_timeout_clk();
}

#define BUFF_SIZE 100

static volatile enum states state_buffer[BUFF_SIZE] = {IDLE};
static volatile int state_count = 0;
static volatile int push_index = 0;
static volatile int pop_index = 0;

void read_encode()
{
    while (1) // TODO: the terminate condition should be changed
    {
        enum states state = IDLE;
        // update the state buffer
        if (state_count > 0)
        {
            state = state_buffer[pop_index++];
            if (pop_index >= BUFF_SIZE)
            {
                pop_index = 0;
            }
            --state_count;
        }

        // TODO: implement the state machine
        switch (state)
        {
        case IDLE:
            break;
        case BUSY:
            break;
        case COLLISION:
            break;
        default:
            break;
        }
    }
}

volatile int tmp1 = 0;
volatile int tmp2 = 0;

void TIM2_IRQHandler(void)
{
    stop_counter(TIM2);
    if (TIM2_BASE->SR & (1 << UIF))
    {
        /***************** The time out is triggered ****************/
        clear_counter_mode_pending_flag(TIM2);
        handle_timeout();
    }
    else if (TIM2_BASE->SR & (1 << CC1IF))
    {
        /***************** The edge is detected ******************/
        clear_input_capture_mode_pending_flag(TIM2);
        handle_edge_detection();
    }
    TIM2_BASE->CNT = 0; // resets the count value to zero
    start_counter(TIM2);
}

static void configure_timeout_clk()
{
    enable_timer_clk(TIME_OUT_CLK);

    set_to_input_capture_mode(TIME_OUT_CLK);
    set_to_counter_mode(TIME_OUT_CLK);
    set_psc(TIME_OUT_CLK, DOWN_SCALE);
    set_arr(TIME_OUT_CLK, TIME_OUT);

    log_tim_interrupt(TIME_OUT_CLK);

    enable_input_capture_mode_interrupt(TIME_OUT_CLK);
    enable_counter_mode_interrupt(TIME_OUT_CLK);
    TIM2_BASE->CNT = 0;
    start_counter(TIME_OUT_CLK);
}

static void handle_edge_detection()
{
    // TODO: fill out the logic for entering state BUSY/COLLISION
    enum states state;
    if (TIM2_BASE->CCR1 > MIN_TIM_GAP)
    {
        // valid data
        state = BUSY;
    }
    else
    {
        // invalid data
        state = COLLISION;
    }

    // The LED light for showing the timer is triggered
    if (tmp2)
    {
        led_on(1);
        tmp2 = 0;
    }
    else
    {
        led_off(1);
        tmp2 = 1;
    }

    // update the state buffer
    if (state_count < BUFF_SIZE)
    {
        state_buffer[push_index++] = state;
        if (push_index >= BUFF_SIZE)
        {
            push_index = 0;
        }
        ++state_count;
    }
}

static void handle_timeout()
{
    // TODO: fill out the logic for entering state COLLISION
    enum states state;
    state = COLLISION;

    // The LED light for showing the timer is triggered
    if (tmp1)
    {
        led_on(0);
        tmp1 = 0;
    }
    else
    {
        led_off(0);
        tmp1 = 1;
    }

    // update the state buffer
    if (state_count < BUFF_SIZE)
    {
        state_buffer[push_index++] = state;
        if (push_index >= BUFF_SIZE)
        {
            push_index = 0;
        }
        ++state_count;
    }
}

