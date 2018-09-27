/*
 * main.c
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#include "led.h"
#include "delay.h"
#include "tim.h"
#include "manchester_encode.h"
#include "gpio.h"
#include <inttypes.h>

int main()
{
    init_GPIO(A);
    light_LED_init();
    manchester_init();
    // Keep looping in the end
    while (1);
}
