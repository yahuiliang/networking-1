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
#include "manchester.h"
#include <inttypes.h>
#include <stdio.h>

static RingBuffer msg_buffer =
{ 0, 0 };

int main()
{
    light_LED_init();
    init_channel_monitor();
    init_transmitter();
    init_receiver();
    init_usart2(19200, F_CPU);

    while (1)
    {
        display_received_char();
        if (hasInput())
        {
            char ch = usart2_getch();
            switch (ch)
            {
            case '\r':
                printf("\n");
                if (state == IDLE)
                {
                    put(&msg_buffer, '\n');
                    while (hasElement(&msg_buffer))
                    { //Transmits all characters in buffer till its empty
                        ch = get(&msg_buffer);
                        transmitCharacter(ch);
                    }
                    transmit_IDLE();
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
}
