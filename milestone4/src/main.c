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
#include "msg_frame.h"
#include "CRC.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

static int attempt = 0;

int main()
{
    light_LED_init();
    init_channel_monitor();
    init_transmitter();
    init_receiver();
    init_usart2(19200, F_CPU);

    CalulateTable_CRC8();

    while (1)
    {
        if (state == COLLISION && attempt < 10)
        {
            attempt++;
            printf("Try reconnecting(%d)...\n", attempt);
            int sleepms = ((rand() % (200 + 1 - 1)) + 1) * 5;
            delay_ms(sleepms);
        }
        else if (state == COLLISION && attempt == 10)
        {
            attempt++;
            dropTransmitData();
            printf("Data dropped!\n");
        }
        else
        {
            attempt = 0;
            retransmit();
            handle_transmit_byte();
            handle_received_byte();
        }
    }
}
