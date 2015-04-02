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

//���������� ����������� �������� ����������� � �������� ������
unsigned char FifoBufDataCnt (FIFO_BUFFER * bufer) {

    return bufer->datacnt;  
}

//"�������" �������� �����
void FifoBufFlush (FIFO_BUFFER * bufer) {

    CRITICAL_SECTION_START;
    
    bufer->tail = 0;
    bufer->head = 0;
    bufer->datacnt = 0;
    
	CRITICAL_SECTION_END;
}

//������ ������
signed int FifoBufGet (FIFO_BUFFER *bufer) {

    unsigned char c;
	CRITICAL_SECTION_START;
    if (bufer->datacnt > 0) {                                          //���� �������� ����� �� ������  
        c = bufer->dataptr[bufer->head];                               //��������� �� ���� ������    
        bufer->datacnt--;                                              //��������� ������� ��������
        bufer->head++;                                                 //���������������� ������ ������ ������  

        if (bufer->head == bufer->size) {
		    bufer->head = 0;
        }
    
        CRITICAL_SECTION_END;
        return c;                                                       //������� ����������� ������
    }
    CRITICAL_SECTION_END;

    return (-1);
}


//������ � �����
signed int FifoBufPut (FIFO_BUFFER *bufer, unsigned char c) {

//    CRITICAL_SECTION_START;   
	if (bufer->datacnt < bufer->size) {                               //���� � ������ ��� ���� �����                     

        bufer->dataptr[bufer->tail] = c;                                //������� ������ �� UDR � �����
        bufer->tail++;                                             //��������� ������ ������ ��������� ������ 

        if (bufer->tail == bufer->size) {
	        bufer->tail = 0;  
        }

    	bufer->datacnt++;                                               //��������� ������� �������� ��������
//	    CRITICAL_SECTION_END;
        return (0); 
	}
    
//    CRITICAL_SECTION_END;
    return (-1);
} 
