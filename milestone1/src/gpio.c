/*
 * gpio.c
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#include "gpio.h"
#include "clock.h"

void init_GPIO(enum GPIOs gpio)
{
    switch (gpio)
    {
    case A:
        // Enable the clock for the GPIO port
        *RCC_AHB1ENR |= 1 << GPIOA_EN;
        break;
    default:
        break;
    }
}

void enable_af_mode(enum GPIOs gpio, int pin, int af_num)
{
    volatile GPIO *gpio_ptr = 0;
    switch (gpio)
    {
    case A:
        gpio_ptr = GPIOA_BASE;
        break;
    default:
        break;
    }
    //Setup GPIOA PIN 5 to Alternate Mode
    gpio_ptr->MODER &= 0xFFFFFBFF;
    gpio_ptr->MODER |= 0x00000800;
    //Setup PIN 5 to AF1
    if (pin <= 7)
    {
        gpio_ptr->AFRL &= ~(0xF << (4 * pin));
        gpio_ptr->AFRL |= (af_num << (4 * pin));
    }
    else if (pin > 7 && pin <= 15)
    {
        gpio_ptr->AFRH &= ~(0xF << (4 * (pin - 8)));
        gpio_ptr->AFRH |= (af_num << (4 * (pin - 8)));
    }
}

