/*
 * main.c
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#include "led.h"
#include "delay.h"
#include "tim.h"
#include "gpio.h"
#include "uart_driver.h"
#include "ringbuffer.h"
#include <inttypes.h>
#include <manchester.h>
#include <stdio.h>

static RingBuffer msg_buffer = {0, 0};

int main()
{
    light_LED_init();
    init_transmitter();
    init_receiver();
    init_usart2(19200, F_CPU);
    while (1)
    {
        char ch = usart2_getch();
        switch (ch)
        {
        case '\r':
            printf("\n");
            if (state == IDLE)
            {
//                put(&msg_buffer, '\r');
//                put(&msg_buffer, '\n');
                while (hasElement(&msg_buffer))
                { //Transmits all characters in buffer till its empty
                    ch = get(&msg_buffer);
                    transmitCharacter(ch);
                }
            }
            clear_buf(&msg_buffer); // clear the message in the memory
            break;
        default:
            if (state == IDLE)
            {
                printf("%c", ch);
                put(&msg_buffer, ch);
            }
            break;
        }
    }
}
