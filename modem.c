#include <avr/pgmspace.h>	// for working with flesh

#include "modem.h"
#include "global.h"			// include our global settings
#include "portsIO.h"
#include "rprintf.h"		// include printf function library

#include "gsm_buffer.h"


static MODEM_ANSVER ModemAnsver = ACK_NO;


extern void ModemAnsverWeit_callback (unsigned int time); 


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


unsigned char ModemSendCom (const char *com_buff, unsigned int com_deluy_time_ms) {

    rprintfProgStr(com_buff);

    com_deluy_time_ms *= CYCLES_PER_MS;
    ModemAnsver = ACK_NO;
//    while (com_deluy_time_ms && ModemAnsver == ACK_NO) {
//	    com_deluy_time_ms--;
//	}

    if (com_deluy_time_ms) {
	    ModemAnsverWeit_callback(com_deluy_time_ms);
	}

    return ModemAnsver;
}


unsigned char ModemSendData (const char *data_buff, unsigned int data_deluy_time_ms) {

    rprintfStr((char *) data_buff);

    data_deluy_time_ms *= CYCLES_PER_MS;
    ModemAnsver = ACK_NO;
//    while (data_deluy_time_ms && ModemAnsver == ACK_NO) {
//	    data_deluy_time_ms--;
//	}
    if (data_deluy_time_ms) {
        ModemAnsverWeit_callback(data_deluy_time_ms);
	}

    return ModemAnsver;
}


void ModemAnsverSet (const MODEM_ANSVER ansver) 
{
    ModemAnsver = ansver;
}
