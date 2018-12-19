/*
 * manchester_encode.h
 *
 *  Created on: Sep 25, 2018
 *      Author: liangy
 */

#ifndef MANCHESTERH_
#define MANCHESTERH_

#include "ringbuffer.h"
#include "msg_frame.h"

enum states
{
    IDLE, BUSY, COLLISION
};

extern volatile enum states state;
extern volatile int firstEdge;

void init_channel_monitor();
void init_transmitter();
void init_receiver();
void handle_received_byte();
void handle_transmit_byte();
void transmit_IDLE();
void transmit_COLLISION();
int transmitCharacter(char c);
int transmit(struct PACKET *pkt);
int retransmit();
void dropTransmitData();
void dropReceiveData();
void resetReceiving();

#endif /* MANCHESTERH_ */
