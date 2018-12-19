/*
 * manchester_encode.h
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#ifndef MANCHESTERH_
#define MANCHESTERH_

enum states
{
    IDLE, BUSY, COLLISION
};

extern volatile enum states state;

void init_channel_monitor();
void init_transmitter();
void init_receiver();
void start_receiver();
void transmit_IDLE();
void transmit_COLLISION();
void transmitCharacter(char c);
void display_received_char();

#endif /* MANCHESTERH_ */
