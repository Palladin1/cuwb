#include "fifo_buffer.h"


#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif


void FifoBufInit (FIFO_BUFFER *bufer, unsigned char *start, unsigned short size)
{
	// begin critical section
	CRITICAL_SECTION_START;
	// set start pointer of the buffer
	bufer->dataptr = start;
	bufer->size = size;
	// initialize index and length
	bufer->tail = 0;
    bufer->head = 0;
	bufer->datacnt = 0;
	// end critical section
	CRITICAL_SECTION_END;
}

//возвращает колличество символов находящихся в приемном буфере
unsigned char FifoBufDataCnt (FIFO_BUFFER * bufer) {

    return bufer->datacnt;  
}

//"очищает" приемный буфер
void FifoBufFlush (FIFO_BUFFER * bufer) {

    CRITICAL_SECTION_START;
    
    bufer->tail = 0;
    bufer->head = 0;
    bufer->datacnt = 0;
    
	CRITICAL_SECTION_END;
}

//чтение буфера
signed int FifoBufGet (FIFO_BUFFER *bufer) {

    unsigned char c;
	CRITICAL_SECTION_START;
    if (bufer->datacnt > 0) {                                          //если приемный буфер не пустой  
        c = bufer->dataptr[bufer->head];                               //прочитать из него символ    
        bufer->datacnt--;                                              //уменьшить счетчик символов
        bufer->head++;                                                 //инкрементировать индекс головы буфера  

        if (bufer->head == bufer->size) {
		    bufer->head = 0;
        }
    
        CRITICAL_SECTION_END;
        return c;                                                       //вернуть прочитанный символ
    }
    CRITICAL_SECTION_END;

    return (-1);
}


//Запись в буфер
signed int FifoBufPut (FIFO_BUFFER *bufer, unsigned char c) {

//    CRITICAL_SECTION_START;   
	if (bufer->datacnt < bufer->size) {                               //если в буфере еще есть место                     

        bufer->dataptr[bufer->tail] = c;                                //считать символ из UDR в буфер
        bufer->tail++;                                             //увеличить индекс хвоста приемного буфера 

        if (bufer->tail == bufer->size) {
	        bufer->tail = 0;  
        }

    	bufer->datacnt++;                                               //увеличить счетчик принятых символов
//	    CRITICAL_SECTION_END;
        return (0); 
	}
    
//    CRITICAL_SECTION_END;
    return (-1);
} 
