#ifndef FIFO_BUFFER_H
#define FIFO_BUFFER_H

#include <avr/io.h>
#include <avr/interrupt.h>


/* size can be only 255 byte, because the type of size is unsigned char */ 
#define  BUF_SIZE_MAX    255

typedef struct FIFO_BUF {
	unsigned char *dataptr;			
	unsigned char size;			    
	unsigned char datacnt; 
	unsigned char tail;
    unsigned char head;    
} FIFO_BUFFER;


void FifoBufInit (FIFO_BUFFER *buffer, unsigned char *start, unsigned short size);

unsigned char FifoBufDataCnt (FIFO_BUFFER *buffer);

signed int FifoBufGet (FIFO_BUFFER *buffer);

signed int FifoBufPut (FIFO_BUFFER *buffer, unsigned char c);

void FifoBufFlush (FIFO_BUFFER *buffer);

#endif // FIFO_BUFFER_H
