/*
 * msg_frame.h
 *
 *  Created on: Oct 29, 2018
 *      Author: liangy
 */

#ifndef MSG_FRAME_H_
#define MSG_FRAME_H_

#include "ringbuffer.h"

struct PACKET
{
    char synch;
    char version;
    char source;
    char destination;
    char length;
    char CRCFlag;
    char msg[5048];
    char trailer;
};

struct PACKET generate_pkt(char sync, char version, char source,
        char destination, char length, char CRCFlag, RingBuffer *msg);

#endif /* MSG_FRAME_H_ */
