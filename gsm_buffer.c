#include "gsm_buffer.h"
#include "portmacro.h"



//unsigned char GSM_TxBuf[GSM_SIZE_TX_BUF];
//unsigned char GSM_TxBuf_tail  = 0;
//unsigned char GSM_TxBuf_head  = 0;
//unsigned char GSM_TxBuf_count = 0;

volatile unsigned char GSM_RxBuf[GSM_SIZE_RX_BUF];
volatile unsigned char GSM_RxBuf_tail  = 0;
volatile unsigned char GSM_RxBuf_head  = 0;
volatile unsigned char GSM_RxBuf_count = 0;

//���������� ����������� �������� ����������� � �������� ������
unsigned char GSM_RxBuf_Count_Get (void) {

    return GSM_RxBuf_count;  
}

//"�������" �������� �����
void GSM_RxBuf_Flush (void) {

//    portENTER_CRITICAL();
    GSM_RxBuf_tail = 0;
    GSM_RxBuf_head = 0;
    GSM_RxBuf_count = 0;
//	portEXIT_CRITICAL();
}

//������ ������
unsigned char GSM_RxBuf_Char_Get (void) {

    unsigned char sym;
	portENTER_CRITICAL();
    if (GSM_RxBuf_count > 0) {                                          //���� �������� ����� �� ������  
        sym = GSM_RxBuf[GSM_RxBuf_head];                                 //��������� �� ���� ������    
        GSM_RxBuf_count--;                                               //��������� ������� ��������
        GSM_RxBuf_head++;                                                //���������������� ������ ������ ������  

        if (GSM_RxBuf_head == GSM_SIZE_RX_BUF) {
		    GSM_RxBuf_head = 0;
        }
    portEXIT_CRITICAL();
    return sym;                                                      //������� ����������� ������
    }

return (-1);
}


//������ � �����
unsigned char GSM_RxBuf_Char_Put (unsigned char c) {
    
	if (GSM_RxBuf_count < GSM_SIZE_RX_BUF) {                          //���� � ������ ��� ���� �����                     
	    
//		portENTER_CRITICAL();
        GSM_RxBuf[GSM_RxBuf_tail] = c;                               //������� ������ �� UDR � �����
        GSM_RxBuf_tail++;                                            //��������� ������ ������ ��������� ������ 

        if (GSM_RxBuf_tail == GSM_SIZE_RX_BUF) {
	        GSM_RxBuf_tail = 0;  
        }

    	GSM_RxBuf_count++;                                               //��������� ������� �������� ��������
//	    portEXIT_CRITICAL();
    return (0); 
	}

return (-1);
} 

