#include <avr/pgmspace.h>	// for working with flesh

#include "modem.h"
#include "global.h"			// include our global settings
#include "portsIO.h"
#include "rprintf.h"		// include printf function library

#include "gsm_buffer.h"



/*
*********************************************************************************************************
**                                        
*********************************************************************************************************
*/


unsigned char ModemStatus (void) {

	if (STATUS_GSM) {
	    return 1;
	}

return 0;
}


unsigned char ModemSetSetings (void) {

/*	
// Set baudrate									
	rprintfProgStr(SET_BAUD);		
	rprintfCRLF();


//Pfone book on sim 
	rprintfProgStr(PFONE_BOOK);	
	rprintfCRLF();


	rprintfProgStr(SET_TEXT_MODE);
	rprintfCRLF();


	rprintfProgStr(SET_FORMAT_1_SMS);
	rprintfCRLF();



    rprintfProgStr(INDICATION_NEW_SMS);
	rprintfCRLF();



//Save settigs into modem's flash
	rprintfProgStr(SAVE_FLESH);
	rprintfCRLF();


	rprintfProgStr(CLEAR_SMS);
	rprintfCRLF();


	rprintfProgStr(CLEAR_ALL_SMS);
	rprintfCRLF();
*/    

return (0);
}


unsigned char ModemSMSSend (unsigned char *number, unsigned char *text_sms) {
  
// 	    rprintfProgStr(SEND_SMS);
//        rprintfProgStr("\""); 
 
//	    rprintfStr((char *)number);
    
//	    rprintfProgStr("\""); 
//	    rprintfChar(0x0D);
	
//        static unsigned char i;
//    	for(i = 0; i <= buff_size; i++) {
//	        uartSendByte(1, text_sms[i]);
//    	}
//        rprintfStr((char *)text_sms);
//	    rprintfChar(0x1A);   //Send Ctrl+z for end the sms' text

return (1);
}


unsigned char ModemSendCom (const char *com_buff, unsigned long com_deluy_time_ms) {

    //unsigned char ret = 0;
    com_deluy_time_ms *= CYCLES_PER_MS;
	
	ModemAnsver = ACK_IDLE,
	
    rprintfProgStr(com_buff);

    while (com_deluy_time_ms != 0) {
	    
	    //if (ModemAnsver != IDLE) {
        if (ModemAnsver) {
		    break;
		}
		com_deluy_time_ms--;
	}

return ModemAnsver;
}

unsigned char ModemSendData (const char *data_buff, unsigned long data_deluy_time_ms) {

    //unsigned char ret = 0;
    data_deluy_time_ms *= CYCLES_PER_MS;
	
	ModemAnsver = ACK_IDLE,
	
    rprintfStr((char *) data_buff);

    while (data_deluy_time_ms != 0) {
	    
	    //if (ModemAnsver != IDLE) {
        if (ModemAnsver) {
		    break;
		}
		data_deluy_time_ms--;
	}

return ModemAnsver;
}

void ModemSendPfoneNum (void) {
  
   	    rprintfChar('\"');
		rprintfChar((u08)(*pfone_number_1 >> 8));
	    rprintfChar((u08)(*pfone_number_1 & 0x00FF));
	    rprintfChar((u08)(*pfone_number_2 >> 8));
	    rprintfChar((u08)(*pfone_number_2 & 0x00FF));
	    rprintfChar((u08)(*pfone_number_3 >> 8));
	    rprintfChar((u08)(*pfone_number_3 & 0x00FF));
	    rprintfChar((u08)(*pfone_number_4 >> 8));
	    rprintfChar((u08)(*pfone_number_4 & 0x00FF));
	    rprintfChar((u08)(*pfone_number_5 >> 8));
	    rprintfChar((u08)(*pfone_number_5 & 0x00FF));
        rprintfChar((u08)(*pfone_number_6 >> 8));
        rprintfChar((u08)(*pfone_number_6 & 0x00FF));
        rprintfChar((u08)(*pfone_number_7 >> 8));
	    rprintfChar((u08)(*pfone_number_7 & 0x00FF));
		rprintfChar('\"');
}
