#include "ringbuffer.h"
#include <inttypes.h>
#include <stdio.h>

void put(volatile RingBuffer *buffer, char element)
{
    buffer->buffer[buffer->put] = element;
    buffer->put++;
    if (buffer->put == BUF_SIZE)
    {
        buffer->put = 0;
    }
}

char get(volatile RingBuffer *buffer)
{
    char tempChar = buffer->buffer[buffer->get];
    buffer->get++;
    if (buffer->get == BUF_SIZE)
    {
        buffer->get = 0;
    }
    return tempChar;
}

void clear_buf(volatile RingBuffer *buffer)
{
//    for (int i = 0; i < BUF_SIZE; ++i)
//    {
//        buffer->buffer[i] = 0;
//    }
    buffer->get = 0;
    buffer->put = 0;
}

int hasSpace(volatile RingBuffer *buffer)
{
    if (buffer->put != (BUF_SIZE - 1))
    {
        return 1;
    }
    return 0;
}

int hasElement(volatile RingBuffer *buffer)
{
    if (((buffer->put) - (buffer->get)) != 0)
    {
        return 1;
    }
    return 0;
}

int buffer_size(volatile RingBuffer *buffer)
{
    if (buffer->put >= buffer->get)
    {
        return (buffer->put - buffer->get);
    }
    else
    {
        return (BUF_SIZE - buffer->get + buffer->put);
    }
}
