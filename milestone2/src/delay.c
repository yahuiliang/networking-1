/*
 * delay.c
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#include "clock.h"
#include <inttypes.h>

/**
 * The delay subroutine which delays the number of milliseconds based
 * on the argument passed in.
 */
void delay_ms(uint32_t theDelay)
{
    volatile uint32_t *systick;
    systick = (uint32_t *) 0xE000E010;
    *systick = 0;
    systick[2] = 0;
    systick[0] = 0;
    systick[1] = theDelay * (F_CPU / 8000);
    systick[0] = 1; // enable the clock.
    while (!(systick[0] & (1 << 16)))
    {
        // nothing to do.
    }
    systick[0] = 0;
    return;
}

