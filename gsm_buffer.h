#ifndef GSM_BUFFER_H
#define GSM_BUFFER_H

#include <avr/io.h>
#include <avr/interrupt.h>


//размер буфера макс. = 255
//#define GSM_SIZE_RX_BUF 100
#define GSM_SIZE_RX_BUF 250

unsigned char GSM_RxBuf_Count_Get (void);
unsigned char GSM_RxBuf_Char_Get (void);
unsigned char GSM_RxBuf_Char_Put (unsigned char c);
void GSM_RxBuf_Flush (void);

#endif // GSM_BUFFER_H
