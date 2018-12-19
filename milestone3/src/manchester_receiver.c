/*
 * manchester_receiver.c
 *
 *  Created on: Oct 18, 2018
 *      Author: liangy
 */

#include "manchester.h"
#include "gpio.h"
#include "tim.h"
#include "led.h"
#include "ringbuffer.h"
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

static const volatile uint16_t lower_bound = 8;
static const volatile uint16_t upper_bound = 2;
static volatile int bitCount = 0;
static volatile RingBuffer bitArray =
{ 0, 0 };
static volatile int firstEdge = 1;
static volatile int last = 0;

// Please use PB6 for the receiver

void init_receiver()
{
    init_GPIO(B);

    enable_af_mode(B, 6, 2);

    enable_timer_clk(TIM4);

    set_to_input_capture_mode(TIM4);
    set_to_counter_mode(TIM4);

    set_psc(TIM4, 1600);
    uint16_t ticks = 10000 * (500e-6 / 1) * 2;
    set_arr(TIM4, ticks);

    log_tim_interrupt(TIM4);

    enable_input_capture_mode_interrupt(TIM4);

    TIM4_BASE->CNT = 0;
    start_counter(TIM4);
}

void display_received_char()
{
    // Display the character
    if (buffer_size(&bitArray) >= 8)
    {
        char displayChar = 0;
        for (int i = 0, offset = 7; i < 8; i++, offset--)
        {
            int bit = get(&bitArray);
            displayChar |= bit << offset;
        }
        // Obtained the display char!!!! Yahoo!!!!!
        printf("%X", displayChar);
    }
}

void TIM4_IRQHandler(void)
{
    stop_counter(TIM4);
    clear_input_capture_mode_pending_flag(TIM4);

    uint16_t cnt = TIM4_BASE->CNT;
    // If the timer auto-loads and within the range, two bits are received
    int lastLogic = 0;
    if (firstEdge || (cnt >= lower_bound) || (cnt <= upper_bound))
    {
        int logic = (GPIOB_BASE->IDR & (1 << IDR6)) != 0;
        if (logic)
        {
            put(&bitArray, 1);
//            printf("logic:1\n");
            lastLogic = 1;
            bitCount++;
        }
        else
        {
            put(&bitArray, 0);
//            printf("logic:0\n");
            lastLogic = 0;
            bitCount++;
        }
        firstEdge = 0;
        clear_cnt(TIM4);
    }
    // Determine if the last edge is received when the last logic is 0
    if (last)
    {
        firstEdge = 1;
        last = 0;
    }
    if (bitCount == 8) // If have received all bits
    {
        if (lastLogic) // if the last logic is 1
        {
            firstEdge = 1;
            last = 0;
        }
        else // if the last logic is 0
        {
            last = 1;
        }
        bitCount = 0;
    }

    start_counter(TIM4);
}
