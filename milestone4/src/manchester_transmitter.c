/*
 * manchester_receiver.c
 *
 *  Created on: Oct 8, 2018
 *      Author: liangy
 */

#include "manchester.h"
#include "gpio.h"
#include "delay.h"
#include "msg_frame.h"
#include "uart_driver.h"
#include <stdio.h>
#include <string.h>

static void transmitBit(char bit);

void init_transmitter()
{
    init_GPIO(B);
    enable_output_mode(B, MODER5);
    transmit_IDLE();
}

void transmit_IDLE()
{
    GPIOB_BASE->ODR |= (1 << ODR5);
    delay_us(500);
    delay_us(500);
    delay_us(500);
}

void transmit_COLLISION()
{
    GPIOB_BASE->ODR &= ~(1 << ODR5);
    delay_us(500);
    delay_us(500);
    delay_us(500);
}

int transmitCharacter(char c)
{
    //Convert char to binary array
    int output[8];
    for (int i = 0; i < 8; ++i)
    {
        output[i] = (c >> i) & 1;
    }
    //Transmit each bit
    for (int i = 7; i >= 0; i--)
    {
        if (state == IDLE)
        {
            transmitBit(output[i]);
            //If collision is detected while transmitting stop the transmission
        }
        else if (state == COLLISION)
        {
            return -1;
        }
    }
    return 0;
}

static RingBuffer msg_buffer =
{ 0, 0 };

void handle_transmit_byte()
{
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
                //Transmit header
                char dataLength = buffer_size(&msg_buffer);

                // The code section below sends the whole data frame body
                struct PACKET pkt = generate_pkt('\x55', '\x01', '\x13', '\x00',
                        dataLength, '\x00', &msg_buffer);

                // Resets the signal and the buffer
                clear_buf(&msg_buffer); // clear the message in the memory

                if (transmit(&pkt) == -1)
                {
                    printf("Stop transmitting...\n");
                }
            }
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

static void transmitBit(char bit)
{
    if (state == IDLE)
    {
        if (bit == 0)
        {
            //Set ODR to 1
            GPIOB_BASE->ODR |= (1 << ODR5);
            delay_us(500); //Delay for half a millisecond
            //Set ODR to 0
            GPIOB_BASE->ODR &= ~(1 << ODR5);
        }
        else
        { //If bit is a 1
          //Set ODR to 0
            GPIOB_BASE->ODR &= ~(1 << ODR5);
            delay_us(500); //Delay for half a millisecond
            //Set ODR to 1
            GPIOB_BASE->ODR |= 1 << ODR5;
        }
        delay_us(500); //Delay for half a millisecond
    }
}

static struct PACKET backup =
{ '\x55', '\x01', '\x13', '\x00', 12, 0, "Hello World\n", '\xC5' };
static int needRetransmit = 0;

static int transmitR(struct PACKET *pkt)
{
    // Copy frame data
    backup.synch = pkt->synch;
    backup.version = pkt->version;
    backup.source = pkt->source;
    backup.destination = pkt->destination;
    backup.length = pkt->length;
    backup.CRCFlag = pkt->CRCFlag;
    memcpy(&backup.msg[0], &pkt->msg[0], pkt->length);
    backup.trailer = pkt->trailer;

    // Transmit header
    if (transmitCharacter(pkt->synch) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    if (transmitCharacter(pkt->version) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    if (transmitCharacter(pkt->source) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    if (transmitCharacter(pkt->destination) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    if (transmitCharacter(pkt->length) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    if (transmitCharacter(pkt->CRCFlag) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    // Transmit message
    for (int i = 0; i < pkt->length; ++i)
    {
        if (transmitCharacter(pkt->msg[i]) == -1)
        {
            needRetransmit = 1;
            return -1;
        }
    }
    // Transmit CRC Value
    if (transmitCharacter(pkt->trailer) == -1)
    {
        needRetransmit = 1;
        return -1;
    }
    return 0;
}

int transmit(struct PACKET *pkt)
{
    int returnVal;
    // Transmit IDLE first
//    transmit_IDLE();
    if (transmitR(pkt) == 0)
    {
        returnVal = 0;
    }
    else
    {
        returnVal = -1;
    }
    // Transmit IDLE in the end since no message gets sent
//    transmit_IDLE();
    return returnVal;
}

int retransmit()
{
    if (needRetransmit)
    {
        printf("Retransmitting...\n");
        if (transmit(&backup) == -1)
        {
            return -1;
        }
        printf("Retransmitting Successful!!!\n");
        needRetransmit = 0;
    }
    return 0;
}

void dropTransmitData()
{
    needRetransmit = 0;
}
