/*
 * gpio.h
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#ifndef GPIO_H_
#define GPIO_H_

#define GPIOA_MODER (volatile uint32_t*) 0x40020000
#define GPIOB_MODER (volatile uint32_t*) 0x40020400
#define GPIOA_ODR (volatile uint32_t*) 0x40020014
#define GPIOB_ODR (volatile uint32_t*) 0x40020414

#endif /* GPIO_H_ */
