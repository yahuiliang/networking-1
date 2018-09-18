/*
 * main.c
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#include "led.h"
#include "delay.h"
#include <inttypes.h>

int main()
{
    light_LED_init();
    uint16_t number = 1;
    // Light up lights by shifting bits.
    while (1)
    {
        for (int i = 0; i < 9; i++)
        {
            light_LED(number);
            delay_ms(200);
            number = number << 1;
        }
        for (int i = 0; i < 9; i++)
        {
            light_LED(number);
            delay_ms(200);
            number = number >> 1;
        }
    }
}
