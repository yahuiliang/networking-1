/*
 * gpio.h
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <inttypes.h>

enum GPIOs
{
    A, B, C, D
};

/* GPIO structure */
typedef struct
{
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFRL;
    uint32_t AFRH;
} GPIO;

#define GPIOA_EN 0
#define GPIOB_EN 1

// Base addresses
#define GPIOA_BASE ((volatile GPIO *) 0x40020000)
#define GPIOB_BASE ((volatile GPIO *) 0x40020400)

// IDR bits
#define IDR4 4
#define IDR5 5
#define IDR6 6
// ODR bits
#define ODR4 4
#define ODR5 5
// MODER bits
#define MODER5 10
#define MODER6 12
// MODER values
#define INPUT 0b00

void init_GPIO(enum GPIOs gpio);
void enable_output_mode(enum GPIOs gpio, int pin);
void enable_af_mode(enum GPIOs gpio, int pin, int af_num);

#endif /* GPIO_H_ */
