/*
 * manchester_receiver.c
 *
 *  Created on: Oct 8, 2018
 *      Author: liangy
 */

#include "manchester.h"
#include "gpio.h"
#include "delay.h"
#include <stdio.h>

static void transmitBit(int bit);

void init_transmitter()
{
    init_GPIO(B);
    enable_output_mode(B, MODER5);
    transmit_IDLE();
}

void transmit_IDLE()
{
    GPIOB_BASE->ODR |= (1 << ODR5);
}

void transmit_COLLISION()
{
    GPIOB_BASE->ODR &= ~(1 << ODR5);
}

void transmitCharacter(char c)
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
        else
        {
            break;
        }
    }
}

static void transmitBit(int bit)
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
