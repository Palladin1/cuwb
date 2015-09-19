//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/eeprom.h>

#include "global.h"		// include our global settings
#include "command.h"
#include "portsIO.h"
#include "uart2.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "DS1337.h"	    // include DS1337 function library
#include "i2ceeprom.h"  // include i2ceeprom function library
#include "inteepr.h"

#include "portmacro.h"


void SellingStart(void) {

//	KLAPAN1_ON;
	KLAPAN2_ON;
	KLAPAN3_ON;

	if (IS_BOARD_VERSION_NEW) {
		
		PUMP_ON;
	}
	else {
	    PUMP_OFF;
	}

}


void SellingStop(void) {

    if (IS_BOARD_VERSION_NEW) {
		PUMP_OFF;
	}
	else {
	    PUMP_ON;
	}

//	KLAPAN1_OFF;
	KLAPAN2_OFF;
	KLAPAN3_OFF;
	
}

void StartGetManey(void) {

	INHIBIT_COIN_OFF;
    INHIBIT_BILL_OFF;
	EN_ESCROW_BILL_OFF;
}

void StopGetManey(void) {
	
	INHIBIT_COIN_ON;
	INHIBIT_BILL_ON;
}


void SaveEvent(const u16 cntmaney, const u16 cntwater, u08 event) {

    u08 EventBuff[12];
	u08 AdrEventBuff[2];	

	DS1337ReadDatta(EventBuff);
	for(u08 count=4; count<=6; count++) {
		EventBuff[count-1] = EventBuff[count];
	}
	
	EventBuff[6] = (cntmaney >> 8);
	EventBuff[7] = (cntmaney & 0x00FF);
	EventBuff[8] = (cntwater >> 8);
	EventBuff[9] = (cntwater & 0x00FF);
	EventBuff[10] = event;

    Global_Time_Deluy(5);
    if (*ext_eepr_data_adr < (*ext_eepr_cur_adr + 11)) { 
	    ADR_LAST_DATTA  = *ext_eepr_cur_adr;
        *ext_eepr_cur_adr = 0x0000;

	    AdrEventBuff[0] = (u08)(ADR_LAST_DATTA >> 8);
	    AdrEventBuff[1] = (u08)(ADR_LAST_DATTA & 0x00FF);
	    i2ceepromWriteBloc(*ext_eepr_data_adr, AdrEventBuff, 2);
		
    	Global_Time_Deluy(5);
	}
   
    i2ceepromWriteBloc(*ext_eepr_cur_adr, EventBuff, 11);
	*ext_eepr_cur_adr += 11;
	IntEeprWordWrite(ExtEeprCarAdrEEPROMAdr, *ext_eepr_cur_adr);
}



u16 KeySkan(u16 key_kode) { 

	if (COUNT_COIN) {
       
        if (CntTmrCoin < (u08)((*coin_time_pulse_coef) >> 8)) {
		    CntTmrCoin++;
        }
	}
	else {
	    if (CntTmrCoin > 0) {
	        u08 TimeMax = ((*coin_time_pulse_coef) >> 8);
		    u08 TimeMin = ((*coin_time_pulse_coef) & 0x00FF);
			if ((CntTmrCoin > TimeMin) && (CntTmrCoin < TimeMax)) {
				 key_kode |= (1 << 0);		    }
			
		}
		else {
			key_kode &= ~(1 << 0);
	    }
		CntTmrCoin = 0;
	}


	if (COUNT_BILL) {
        
        if (CntTmrBill < (u08)((*bill_time_pulse_coef) >> 8)) {
		    CntTmrBill++;
        }
	}
	else {
	    if (CntTmrBill > 0) {
	        u08 TimeMax = ((*bill_time_pulse_coef) >> 8);
		    u08 TimeMin = ((*bill_time_pulse_coef) & 0x00FF);
			if ((CntTmrBill > TimeMin) && (CntTmrBill < TimeMax)) {
				key_kode |= (1 << 1);
		    }
			
		}
		else {
			key_kode &= ~(1 << 1);
		}
		CntTmrBill = 0;
	}
    
//=================================
	
	if (!(WATER_PRESENT)) {	
 	    if (CntNoWater == 20) {
            key_kode |= (1 << 2);
			CntNoWater = 30;
		}    
        else if (CntNoWater < 20) {
		    CntNoWater++;
        }
	}
	else {
		key_kode &= ~(1 << 2);
	    CntNoWater = 0;
	}
//=================================	

	if (STATUS_PWR_12V) {		      
        if (CntNoPower1 == 20) {
			key_kode |= (1 << 3);
			CntNoPower1 = 30;
		}    
        else if (CntNoPower1 < 20) {
		    CntNoPower1++;
        }
	}
	else {
		key_kode &= ~(1 << 3);
	    CntNoPower1 = 0;
	}
//=================================

	if (STATUS_PWR_5V) {		      
        if (CntNoPower2 == 20) {
			key_kode |= (1 << 4);
			CntNoPower2 = 30;
		}    
        else if (CntNoPower2 < 20) {
		    CntNoPower2++;
        }
	}
	else {
		key_kode &= ~(1 << 4);
	    CntNoPower2 = 0;
	}
//=================================	

	if (!(BTN_DOOR)) {				  
	    if (CntDoorOpn == 20) {
			key_kode |= (1 << 5);
			CntDoorOpn = 30;
		}    
        else if (CntDoorOpn < 20) {
		    CntDoorOpn++;
        }
	}
	else {
		key_kode &= ~(1 << 5);   
	    CntDoorOpn = 0;
	}
//=================================
   	
	if (!(BTN_START)) {
	    if (CntStart == 20) {
			key_kode |= (1 << 6);
			CntStart = 30;
		}    
        else if (CntStart < 20) {
		    CntStart++;
        }
	}
	else {
		key_kode &= ~(1 << 6);
	    CntStart = 0;
	}
//=================================

	if (!(BTN_STOP)) {
        if (CntStop == 20) {
			key_kode |= (1 << 7);
			CntStop = 30;
		}    
        else if (CntStop < 20) {
		    CntStop++;
        }
	}
	else {
		key_kode &= ~(1 << 7);
	    CntStop = 0;
	}
//=================================

	if (BTN_RESET) {
 	    if (CntReset == 20) {
			key_kode |= (1 << 8);
			CntReset = 30;
		}    
        else if (CntReset < 20) {
		    CntReset++;
        }
	}
	else {
		key_kode &= ~(1 << 8);
	    CntReset = 0;
	}
	
//=================================

	if (STATUS_COUNT_BILL) {
	    if (CntNoWrkBill == 20) {
			key_kode |= (1 << 9);
			CntNoWrkBill = 30;
		}    
        else if (CntNoWrkBill < 20) {
		    CntNoWrkBill++;
        }
	}
	else {
		key_kode &= ~(1 << 9);
	    CntNoWrkBill = 0;
	}
	
//=================================

	if (!BTN_REGISTRATOR_PRESENT) {
	    if (CntRegPresent == 20) {
			key_kode |= (1 << 10);
			CntRegPresent = 30;
		}    
        else if (CntRegPresent < 20) {
		    CntRegPresent++;
        }
	}
	else {
		key_kode &= ~(1 << 10);
	    CntRegPresent = 0;
	}

return key_kode;	

}

//=====================================================================


void GetCmd (unsigned char *get_cmd_buff) {

    void SET_THE_RTC (void) {

		get_cmd_buff[8] = get_cmd_buff[7];
		get_cmd_buff[7] = get_cmd_buff[6];
		get_cmd_buff[6] = get_cmd_buff[5];
		get_cmd_buff[5] = 1;
		
		DS1337WriteDatta((get_cmd_buff+2));
		
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}
	
	void RAEAD_FROM_RTC (void) {
		
		DS1337ReadDatta((get_cmd_buff + 2));
		
		get_cmd_buff[0] = 7;
		get_cmd_buff[5] = get_cmd_buff[6];
		get_cmd_buff[6] = get_cmd_buff[7];
		get_cmd_buff[7] = get_cmd_buff[8];		

		uartSendBuf(0, &get_cmd_buff[0], 8);
	}

	void READ_EXT_EEPROM (void) {

		u16 start_memAddr_for_read = 0;
	    u16 stop_memAddr_for_read = *ext_eepr_cur_adr; 
        u16 start_adr_uart_rx_buf = (((u16)get_cmd_buff[3]) << 8) + get_cmd_buff[2];
        u16 stop_adr_uart_rx_buf  = (((u16)get_cmd_buff[5]) << 8) + get_cmd_buff[4];
        
	   	
		if (start_adr_uart_rx_buf < (*ext_eepr_data_adr - 11)) {
		    start_memAddr_for_read = start_adr_uart_rx_buf;
		}


        if ((stop_adr_uart_rx_buf < *ext_eepr_data_adr) && (stop_adr_uart_rx_buf != 0)) {
    	    stop_memAddr_for_read = stop_adr_uart_rx_buf;
		}
		else if ((ADR_LAST_DATTA != 0) && (start_memAddr_for_read == 0)) {

            start_memAddr_for_read = *ext_eepr_cur_adr;
								 
		    while (start_memAddr_for_read < ADR_LAST_DATTA) {
		
		    	i2ceepromReadBloc(start_memAddr_for_read, (get_cmd_buff+2), 11);
	
			    start_memAddr_for_read += 11;
			    get_cmd_buff[0] = 12;
			    
			    uartSendBuf(0, &get_cmd_buff[0], 13);
//		        _delay_ms(20);
                Global_Time_Deluy(20);
    	    }
			start_memAddr_for_read = 0;
		} 
		
		while (start_memAddr_for_read < stop_memAddr_for_read) {
		
			i2ceepromReadBloc(start_memAddr_for_read, (get_cmd_buff+2), 11);
	
			start_memAddr_for_read += 11;
			get_cmd_buff[0] = 12;
			
			uartSendBuf(0, &get_cmd_buff[0], 13);
//		    _delay_ms(20);
            Global_Time_Deluy(20);
    	}


		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}
	
	void ERASE_EXT_EEPROM (void)	{	

        get_cmd_buff[2] = 0;
		get_cmd_buff[3] = 0;
		i2ceepromWriteBloc(*ext_eepr_data_adr, (get_cmd_buff + 2), 2);
		ADR_LAST_DATTA = 0x0000;
		*ext_eepr_cur_adr = 0x0000;
		IntEeprWordWrite(ExtEeprCarAdrEEPROMAdr, *ext_eepr_cur_adr);

		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}

	void WRITE_INT_EEPROM (void) {

		u16 EepromAdr;
		u16 tmp_ext_eepr_data_adr = *ext_eepr_data_adr;

		eeprom_busy_wait();
		portENTER_CRITICAL();
		eeprom_write_word ((uint16_t*)*(&((uint16_t*) get_cmd_buff)[1]), ((uint16_t*)get_cmd_buff)[2]);
		portEXIT_CRITICAL();
			
		EepromAdr = CostLitreCoefEEPROMAdr;													
		eeprom_busy_wait();
		portENTER_CRITICAL();
		eeprom_read_block (EEPROM_DATA,(uint16_t *)*(&EepromAdr), 54);
        portEXIT_CRITICAL();

///////////////////////////////////////////////////////////////
        if (tmp_ext_eepr_data_adr != *ext_eepr_data_adr) {

            if ((*ext_eepr_data_adr - 1) >= ADR_LAST_DATTA ) {              
			    get_cmd_buff[2] = (u08)(ADR_LAST_DATTA >> 8);
	            get_cmd_buff[3] = (u08)(ADR_LAST_DATTA & 0x00FF);
                i2ceepromWriteBloc(*ext_eepr_data_adr, (get_cmd_buff + 2), 2);
			}
			else {
			    ADR_LAST_DATTA = 0x0000;
				get_cmd_buff[2] = (u08)(ADR_LAST_DATTA >> 8);
	            get_cmd_buff[3] = (u08)(ADR_LAST_DATTA & 0x00FF);
                i2ceepromWriteBloc(*ext_eepr_data_adr, (get_cmd_buff + 2), 2);
			}
		}
///////////////////////////////////////////////////////////////
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}

	void READ_INT_EEPROM (void) {
		
		eeprom_busy_wait();
		portENTER_CRITICAL();
		((uint16_t*) get_cmd_buff)[2] = eeprom_read_word ((uint16_t *)*(&((uint16_t*) get_cmd_buff)[1]));
		portEXIT_CRITICAL();
		get_cmd_buff[0] = 5;
				
		uartSendBuf(0, &get_cmd_buff[0], 6);
	}

	void WRITE_PULSE_COUNT (void) {
				
		CountPulse = (get_cmd_buff[2] << 8);
		CountPulse = CountPulse + get_cmd_buff[3];
			
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}

	void READ_PULSE_FAULT_COUNT (void) {

		get_cmd_buff[2] = CountPulseFault;

		get_cmd_buff[0] = 2;
		uartSendBuf(0, &get_cmd_buff[0], 3);
	}
	
	void READ_STATUS (void) {
	        
	}	


	switch (get_cmd_buff[1]) {
		
		case set_the_rtc:
			SET_THE_RTC();
			break;
		case read_from_rtc:
			RAEAD_FROM_RTC();
			break;
		case read_ext_eeprom:
			READ_EXT_EEPROM();
			break;
		case arase_ext_eeprom:
			ERASE_EXT_EEPROM();
			break;
		case write_int_eeprom:
			WRITE_INT_EEPROM();
			break;
		case read_int_eeprom:
			READ_INT_EEPROM();
			break;
		case write_pulse_count:
			WRITE_PULSE_COUNT();
			break;
		case read_pulse_fault_count:
			READ_PULSE_FAULT_COUNT();
			break;
        case read_status: {
		     READ_STATUS();
			 break;
		} 
		default: 
			break;
	}
}

/*
void Get_Pfone_Number (u08 *phone_buff) {

  	    phone_buff[0] = '\"';
		phone_buff[1]  = (u08)(*pfone_number_1 >> 8);                       // Read pfone namber for send message
	    phone_buff[2]  = (u08)(*pfone_number_1 & 0x00FF);
	    phone_buff[3]  = (u08)(*pfone_number_2 >> 8);
	    phone_buff[4]  = (u08)(*pfone_number_2 & 0x00FF);
	    phone_buff[5]  = (u08)(*pfone_number_3 >> 8);
	    phone_buff[6]  = (u08)(*pfone_number_3 & 0x00FF);
	    phone_buff[7]  = (u08)(*pfone_number_4 >> 8);
	    phone_buff[8]  = (u08)(*pfone_number_4 & 0x00FF);
	    phone_buff[9]  = (u08)(*pfone_number_5 >> 8);
	    phone_buff[10] = (u08)(*pfone_number_5 & 0x00FF);
        phone_buff[12] = (u08)(*pfone_number_6 >> 8);
        phone_buff[13] = (u08)(*pfone_number_6 & 0x00FF);
        phone_buff[14] = (u08)(*pfone_number_7 >> 8);
	    phone_buff[15] = '\"';
		phone_buff[16] = '\r';
		phone_buff[17] = '\0';
}
*/

void Get_APN (u08 *apn_buff) {
    
	u16 EepromAdr;
	EepromAdr = CostLitreCoefEEPROMAdr;													
	eeprom_busy_wait();
	eeprom_read_block (apn_buff, (uint16_t *)*(&EepromAdr), 15);
}

void Get_IP (u08 *ip_buff) {

	u16 EepromAdr;    
	EepromAdr = CostLitreCoefEEPROMAdr;													
	eeprom_busy_wait();
    eeprom_read_block (ip_buff, (uint16_t *)*(&EepromAdr), 8);
}

void Get_SCRIPT_PASS (u08 *script_pass_buff) {
    
	u16 EepromAdr;	
	EepromAdr = CostLitreCoefEEPROMAdr;													
	eeprom_busy_wait();
	eeprom_read_block (script_pass_buff, (uint16_t *)*(&EepromAdr), 8);
}

inline void Create_Report_String (u08 *report_buff, u08 EventNamber) {

	    u08 cnt_buf = 0;

        itoan(*vodomat_number, &report_buff[cnt_buf], 4); 

	    DS1337ReadDatta(&report_buff[cnt_buf+4]);                              //Read data from RTC  
	
		hextoa2(report_buff[10+cnt_buf], &report_buff[14+cnt_buf]);                   // Convert the Data and time which read to ASCII
		hextoa2(report_buff[9+cnt_buf],  &report_buff[12+cnt_buf]);                   //
		hextoa2(report_buff[8+cnt_buf],  &report_buff[10+cnt_buf]);                   //
		hextoa2(report_buff[6+cnt_buf],  &report_buff[8+cnt_buf]);                    //
		hextoa2(report_buff[5+cnt_buf],  &report_buff[6+cnt_buf]);                    //
		hextoa2(report_buff[4+cnt_buf],  &report_buff[4+cnt_buf]);                    //

        if (EventNamber == 3) {     //Fl_Ev_TakeManey = 3
            itoan(CollectoinCountManey, &report_buff[16+cnt_buf], 6);
		    CollectoinCountManey = 0;
            IntEeprDwordWrite(CollectionManeyEEPROMAdr, CollectoinCountManey);
		}
		else if (EventNamber == 2) {
		    itoan((u32) MoneyToReturn, &report_buff[16+cnt_buf], 6);
		}
		else {
		    itoan(*day_maney_cnt, &report_buff[16+cnt_buf], 6);
		}

		if (EventNamber == 2) {
		    itoan((u32) WaterToReturn, &report_buff[22+cnt_buf], 6);
		}
		else {
            itoan(*amount_water, &report_buff[22+cnt_buf], 6);
		}
	    
		itoan(*cost_litre_coef, &report_buff[28+cnt_buf], 4);

/*
************************************************************ 
*       Sets the flags of stats from board unit     
************************************************************
*/
        report_buff[32+cnt_buf] = Fl_State_Water;

        report_buff[33+cnt_buf] = Fl_State_RsvBill;

        report_buff[34+cnt_buf] = Fl_State_Power;
      
        report_buff[35+cnt_buf] = Fl_State_WtrCnt;
/*
************************************************************ 
*       End flags sets 
************************************************************
*/
	    itoan(EventNamber, &report_buff[36+cnt_buf], 2);
//		report_buff[38+cnt_buf] = 0x1A;
//		report_buff[39+cnt_buf] = '\0';
}


void itoan (u32 binval, u08 *asc, u08 size) {

    u32 step[]={100000,10000,1000,100,10,1};
    u32 temp,val;
    u08 i,atemp, ofset;
	
    val=binval;
	size = (size > 6) ? 2 : size; 

    ofset = 6 - size;

    for (i = 0; i < size; i++) {
        temp=step[i+ofset];
        atemp='0';
       while(val >= temp)
       {
           atemp++;
           val-=temp;
       }
       asc[i]=atemp;
    }
}

inline void hextoa2(u08 binval, u08 *asc) {

    u08 step[] = {0x10, 0x1};
    u08 temp, val;
    u08 i, atemp;

    val = binval;
    for (i = 0; i < 2; i++) {
        temp = step[i];
        atemp = '0';
       while (val >= temp)
       {
           atemp++;
           val -= temp;
       }
       asc[i] = atemp;
    }
}


u16 GetRealTime (void) {

    u08 check_time_buff[8];
    
	DS1337ReadDatta(check_time_buff);
	check_time_buff[1] = bcdtoi2(check_time_buff[1]);
    
return ((bcdtoi2(check_time_buff[2]) * 60) + check_time_buff[1]);
}

inline u08 bcdtoi2(u08 binval) {

    u08 val, temp = 0;
    
    val = (binval & 0xF0);
	binval = (binval & 0x0F);

    while (val > 0)  {
        val -= 16;
		temp += 10;
    }
       
return (temp + binval);
}


void uartSendBuf(u08 num, u08 *s , u08 len) {
    
	while (len-- > 0)
	    uartSendByte(num, *s++);
}
