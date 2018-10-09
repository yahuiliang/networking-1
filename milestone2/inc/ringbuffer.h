#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#define BUF_SIZE 255

//Ring Buffer Structure
typedef struct
{
    unsigned int put;
    unsigned int get;
    char buffer[BUF_SIZE];
} RingBuffer;

//Function Declaration
void put(volatile RingBuffer* buffer, char element);
char get(volatile RingBuffer* buffer);
void clear_buf(volatile RingBuffer *buffer);
int hasSpace(volatile RingBuffer*);
int hasElement(volatile RingBuffer*);

#endif /* RINGBUFFER_H_ */
