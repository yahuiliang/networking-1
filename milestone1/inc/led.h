/*
 * led.h
 *
 *  Created on: Sep 18, 2018
 *      Author: liangy
 */

#ifndef LED_H_
#define LED_H_

#include <inttypes.h>

/**
 * Initializes all LEDs used in this program.
 */
void light_LED_init();

/**
 * Lights up the corresponding LED based on the number passed in.
 */
void light_LED(uint16_t number);

#endif /* LED_H_ */
