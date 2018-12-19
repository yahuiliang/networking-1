/*
 * CRC.c
 *
 *  Created on: Nov 6, 2018
 *      Author: liangy
 */

static char crctable[256] =
{ 0 };

void CalulateTable_CRC8()
{
    const char generator = 0x07;
    /* iterate over all byte values 0 - 255 */
    for (int divident = 0; divident < 256; divident++)
    {
        char currByte = (char) divident;
        /* calculate the CRC-8 value for current byte */
        for (char bit = 0; bit < 8; bit++)
        {
            if ((currByte & 0x80) != 0)
            {
                currByte <<= 1;
                currByte ^= generator;
            }
            else
            {
                currByte <<= 1;
            }
        }
        /* store CRC value in lookup table */
        crctable[divident] = currByte;
    }
}

char Compute_CRC8(char bytes[], int length)
{
    char crc = 0;
    for (int i = 0; i < length; ++i)
    {
        char b = bytes[i];
        /* XOR-in next input byte */
        int data = (b ^ crc);
        /* get current CRC value = remainder */
        crc = crctable[data];
    }
    return crc;
}
