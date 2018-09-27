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
    A,
    B,
    C,
    D
};

/* GPIO structure */
typedef struct {
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

// Base addresses
#define GPIOA_BASE ((volatile GPIO *) 0x40020000)
#define GPIOB_BASE ((volatile GPIO *) 0x40020400)

void init_GPIO(enum GPIOs gpio);
void enable_af_mode(enum GPIOs gpio, int pin, int af_num);

#endif /* GPIO_H_ */
