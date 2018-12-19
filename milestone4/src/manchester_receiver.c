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
#include "msg_frame.h"
#include "CRC.h"
#include "delay.h"
#include <inttypes.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static const volatile uint16_t lower_bound = 8;
static const volatile uint16_t upper_bound = 2;
static volatile RingBuffer bitArray =
{ 0, 0 };
volatile int firstEdge = 1;

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

static struct PACKET pkt;
static int byteIndex = 0;
static int msgByteCount = 0;

void handle_received_byte()
{
    while (buffer_size(&bitArray) >= 8)
    {
        char byte = 0;
        for (int i = 0, offset = 7; i < 8; i++, offset--)
        {
            int bit = get(&bitArray);
            byte |= bit << offset;
            if (state == COLLISION)
            {
                dropReceiveData();
                return;
            }
        }
        // Obtained the display char!!!! Yahoo!!!!!
        // Handle the byte
        switch (byteIndex)
        {
        //Sync
        case 0:
            pkt.synch = byte;
            byteIndex++;
            break;
            //Version
        case 1:
            pkt.version = byte;
            byteIndex++;
            break;
            //Source
        case 2:
            pkt.source = byte;
            byteIndex++;
            break;
            //Destination
        case 3:
            pkt.destination = byte;
            byteIndex++;
            break;
            //Length
        case 4:
            pkt.length = byte;
            byteIndex++;
            break;
            //CRC Flag
        case 5:
            pkt.CRCFlag = byte;
            byteIndex++;
            break;
        default:
            if (msgByteCount < pkt.length)
            {        // Message
                pkt.msg[msgByteCount++] = byte;
            }
            else
            {        // The checksum
                pkt.trailer = byte;
                // Handle the packet here since all bytes have been received
                if (pkt.destination == '\x14' || pkt.destination == '\x00')
                {
                    pkt.msg[msgByteCount] = 0;
                    char crc = Compute_CRC8(pkt.msg, pkt.length);
                    if (pkt.trailer == crc)
                    {
                        printf(pkt.msg);
                    }
                    else
                    {
                        printf("The message might be invalid!\n");
                    }
                }
                // Reset indexes
                dropReceiveData();
            }
            break;
        }
    }
}

void dropReceiveData()
{
    byteIndex = 0;
    msgByteCount = 0;
    clear_buf(&bitArray);
}

void TIM4_IRQHandler(void)
{
    stop_counter(TIM4);
    clear_input_capture_mode_pending_flag(TIM4);

    printf("T");

    if (state != COLLISION)
    {
        uint16_t cnt = TIM4_BASE->CNT;
        // If the timer auto-loads and within the range, two bits are received
        if (firstEdge || (cnt >= lower_bound) || (cnt <= upper_bound))
        {
            int logic = (GPIOB_BASE->IDR & (1 << IDR6)) != 0;
            put(&bitArray, logic);
            firstEdge = 0;
            clear_cnt(TIM4);
        }
    }

    start_counter(TIM4);
}

void resetReceiving()
{
    firstEdge = 1;
    dropReceiveData();
}
