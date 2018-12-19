/*
 * manchester_encode.c
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#include "manchester.h"
#include "tim.h"
#include "led.h"
#include "gpio.h"
#include <stdio.h>

#define DOWN_SCALE              0
#define DEFAULT_TIME_OUT        17760
#define TIME_OUT_COLLISION      17120
#define TIME_OUT_IDLE           17120
#define TIME_OUT_CLK            TIM2

static void configure_input_capture_clk();
static void handle_edge_detection();
static void handle_timeout();
static void idle_light_on();
static void collision_light_on();
static void busy_light_on();
static void lights_off();
static void enter_IDLE();
static void enter_BUSY();
static void enter_COLLISION();

void init_channel_monitor()
{
    enter_IDLE();
    enable_af_mode(A, 5, 1);
    configure_input_capture_clk();
}

volatile int prev_logic = 0;

void TIM2_IRQHandler(void)
{
    stop_counter(TIM2);

//    clear_counter_mode_pending_flag(TIM2);
//
//    if (state == IDLE)
//    {
//        state = COLLISION;
//        led_on(0);
//    }
//    else if (state == COLLISION)
//    {
//        state = IDLE;
//        led_off(0);
//    }

    if (TIM2_BASE->SR & (1 << UIF))
    {
        /***************** The time out is triggered ****************/
        clear_counter_mode_pending_flag(TIM2);
        handle_timeout();
    }
    else if (TIM2_BASE->SR & (1 << CC1IF))
    {
        /***************** The edge is detected *********************/
        clear_input_capture_mode_pending_flag(TIM2);
        handle_edge_detection();
    }

    TIM2_BASE->CNT = 0;
    TIM2_BASE->CCR1 = 0;
    start_counter(TIM2);
    prev_logic = GPIOA_BASE->IDR & (1 << IDR5);
}

static void configure_input_capture_clk()
{
    enable_timer_clk(TIME_OUT_CLK);

    set_to_input_capture_mode(TIME_OUT_CLK);
    set_psc(TIME_OUT_CLK, DOWN_SCALE);
    set_arr(TIME_OUT_CLK, DEFAULT_TIME_OUT);

    log_tim_interrupt(TIME_OUT_CLK);

    enable_input_capture_mode_interrupt(TIME_OUT_CLK);
//    enable_counter_mode_interrupt(TIME_OUT_CLK);
    TIM2_BASE->CNT = 0;
    start_counter(TIME_OUT_CLK);
}

volatile int tmp1 = 0;
volatile int tmp2 = 0;

volatile enum states state = IDLE;

static void handle_edge_detection()
{
    switch (state)
    {
    case IDLE:
        enter_BUSY();
        break;
    default:
        enter_BUSY();
        break;
    }
    // The LED light for showing the timer is triggered
//    if (tmp2)
//    {
//        led_on(1);
//        tmp2 = 0;
//    }
//    else
//    {
//        led_off(1);
//        tmp2 = 1;
//    }
}

static void handle_timeout()
{
    switch (state)
    {
    case BUSY:
        if (prev_logic) // When the voltage is high
        {
            enter_IDLE();
        }
        else            // When the voltage is low
        {
            enter_COLLISION();
        }
        break;
    default:
        break;
    }
    // The LED light for showing the timer is triggered
//    if (tmp1)
//    {
//        led_on(0);
//        tmp1 = 0;
//    }
//    else
//    {
//        led_off(0);
//        tmp1 = 1;
//    }
}

static void lights_off()
{
    for (int i = 0; i < 10; ++i)
    {
        led_off(i);
    }
}

static void enter_IDLE()
{
    state = IDLE;
    printf("I\n");
    firstEdge = 1;
    disable_counter_mode_interrupt(TIME_OUT_CLK);
    lights_off();
    idle_light_on();
}

static void enter_BUSY()
{
    if (state == COLLISION)
    {
        resetReceiving();
    }
    state = BUSY;
    if (prev_logic) // When the voltage is high
    {
        // IDLE
        set_arr(TIM2, TIME_OUT_IDLE);
    }
    else // When the voltage is low
    {
        set_arr(TIM2, TIME_OUT_COLLISION);
    }
    set_to_counter_mode(TIME_OUT_CLK);
    enable_counter_mode_interrupt(TIME_OUT_CLK);
    lights_off();
    busy_light_on();
}

static void enter_COLLISION()
{
    state = COLLISION;
    printf("C\n");
    resetReceiving();
    disable_counter_mode_interrupt(TIME_OUT_CLK);
    lights_off();
    collision_light_on();
}

static void collision_light_on()
{
    led_on(5);
}

static void busy_light_on()
{
    led_on(9);
}

static void idle_light_on()
{
    led_on(0);
}

