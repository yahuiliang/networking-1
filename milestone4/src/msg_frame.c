/*
 * msg_frame.c
 *
 *  Created on: Oct 29, 2018
 *      Author: liangy
 */

#include "msg_frame.h"
#include "ringbuffer.h"
#include "CRC.h"
#include <string.h>
#include <stdio.h>

struct PACKET generate_pkt(char sync, char version, char source,
        char destination, char length, char CRCFlag, RingBuffer *msg)
{
    struct PACKET returnObj;
    returnObj.synch = sync;
    returnObj.version = version;
    returnObj.source = source;
    returnObj.destination = destination;
    returnObj.length = length;
    returnObj.CRCFlag = CRCFlag;
    int index = 0;
    while (hasElement(msg))
    {
        //Transmits all characters in buffer till its empty
        char ch = get(msg);
        returnObj.msg[index++] = ch;
    }
    returnObj.msg[index++] = 0;
    returnObj.trailer = Compute_CRC8(returnObj.msg, returnObj.length);
    return returnObj;
}
