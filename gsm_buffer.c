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

//возвращает колличество символов находящихся в приемном буфере
unsigned char GSM_RxBuf_Count_Get (void) {

    return GSM_RxBuf_count;  
}

//"очищает" приемный буфер
void GSM_RxBuf_Flush (void) {

//    portENTER_CRITICAL();
    GSM_RxBuf_tail = 0;
    GSM_RxBuf_head = 0;
    GSM_RxBuf_count = 0;
//	portEXIT_CRITICAL();
}

//чтение буфера
unsigned char GSM_RxBuf_Char_Get (void) {

    unsigned char sym;
	portENTER_CRITICAL();
    if (GSM_RxBuf_count > 0) {                                          //если приемный буфер не пустой  
        sym = GSM_RxBuf[GSM_RxBuf_head];                                 //прочитать из него символ    
        GSM_RxBuf_count--;                                               //уменьшить счетчик символов
        GSM_RxBuf_head++;                                                //инкрементировать индекс головы буфера  

        if (GSM_RxBuf_head == GSM_SIZE_RX_BUF) {
		    GSM_RxBuf_head = 0;
        }
    portEXIT_CRITICAL();
    return sym;                                                      //вернуть прочитанный символ
    }

return (-1);
}


//Запись в буфер
unsigned char GSM_RxBuf_Char_Put (unsigned char c) {
    
	if (GSM_RxBuf_count < GSM_SIZE_RX_BUF) {                          //если в буфере еще есть место                     
	    
//		portENTER_CRITICAL();
        GSM_RxBuf[GSM_RxBuf_tail] = c;                               //считать символ из UDR в буфер
        GSM_RxBuf_tail++;                                            //увеличить индекс хвоста приемного буфера 

        if (GSM_RxBuf_tail == GSM_SIZE_RX_BUF) {
	        GSM_RxBuf_tail = 0;  
        }

    	GSM_RxBuf_count++;                                               //увеличить счетчик принятых символов
//	    portEXIT_CRITICAL();
    return (0); 
	}

return (-1);
} 

