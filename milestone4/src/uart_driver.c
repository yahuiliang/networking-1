/*
 * uart_driver.c
 *
 *  Created on: Nov 8, 2016
 *      Author: barnekow
 */
#include "uart_driver.h"
#include "isr.h"
#include <inttypes.h>
#include <stdio.h>
#include "ringbuffer.h"

static RingBuffer printCharBuffer =
{ 0, 0 };
static RingBuffer inputCharBuffer =
{ 0, 0 };

int hasInput()
{
    return hasElement(&inputCharBuffer);
}

char usart2_getch()
{
    //Enable RXNEIE
    *(USART_CR1 ) |= (1 << 5);
    char c = get(&inputCharBuffer);  // Read character from usart
    return c;
}

void usart2_putch(char c)
{
    put(&printCharBuffer, c);
    //Turn on TXEIE
    *(USART_CR1 ) |= (1 << 7);
}

void init_usart2(uint32_t baud, uint32_t sysclk)
{
    // Enable clocks for GPIOA and USART2
    *(RCC_AHB1ENR) |= (1 << GPIOAEN);
    *(RCC_APB1ENR) |= (1 << USART2EN);

    // Function 7 of PORTA pins is USART
    *(GPIOA_AFRL ) &= (0xFFFF00FF); // Clear the bits associated with PA3 and PA2
    *(GPIOA_AFRL ) |= (0b01110111 << 8); // Choose function 7 for both PA3 and PA2
    *(GPIOA_MODER ) &= (0xFFFFFF0F); // Clear mode bits for PA3 and PA2
    *(GPIOA_MODER ) |= (0b1010 << 4); // Both PA3 and PA2 in alt function mode

    // Set up USART2
    //USART2_init(); //8n1 no flow control
    // over8 = 0..oversample by 16
    // M = 0..1 start bit, data size is 8, 1 stop bit
    // PCE= 0..Parity check not enabled
    // no interrupts... using polling
    *(USART_CR1 ) = (1 << UE) | (1 << TE) | (1 << RE); // Enable UART, Tx and Rx
    *(USART_CR2 ) = 0; // This is the default, but do it anyway
    *(USART_CR3 ) = 0; // This is the default, but do it anyway
    *(USART_BRR ) = sysclk / baud;

    //Enable Interrupts

    *(ISER1 ) = (1 << 6);

    //Enable RXNEIE
    *(USART_CR1 ) |= (1 << 5);

    /* I'm not sure if this is needed for standard IO*/
    //setvbuf(stderr, NULL, _IONBF, 0);
    //setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

/**
 * USART2 Interrupt Handler
 */
void USART2_IRQHandler(void)
{
    if (*(USART_CR1 ) & (1 << 7))
    {
        //If TXE
        //Get character from buffer and send it
        if (hasElement(&printCharBuffer) == 1)
        {
            *(USART_DR ) = get(&printCharBuffer);
        }
        // If the buffer is empty, just do nothing here
        else
        {
            // Turn off TXEIE
            *(USART_CR1 ) &= ~(1 << 7);
        }
    }
    else
    {
        //Put character in ring buffer
        char tmp = *USART_DR;
        put(&inputCharBuffer, tmp);
    }
}
