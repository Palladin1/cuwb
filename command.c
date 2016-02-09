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

#include  "indicator.h"
//#include  <time.h>


static u08 PumpShouldTurnOn = 0;

static const u08 Days_In_Month_Buf[MONTH_MAX] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void SellingStart(void) {

    portENTER_CRITICAL();

	KLAPAN1_ON;
	KLAPAN2_ON;
	KLAPAN3_ON;

	if (IS_BOARD_VERSION_NEW) {
		
		PUMP_ON;
	}
	else {
	    PUMP_OFF;
	}

    portEXIT_CRITICAL();

    PumpShouldTurnOn = 1;
}


/* 
    One pulse equal 20 mS
    and we have ebout 3 mlitre by one pulse
*/
enum {
    PULS_EQUAL_TIME_CNT_DEFAULT = 20,
    PULS_EQUAL_TIME_CNT_20MS    = 20,
    PULS_EQUAL_TIME_CNT_40MS    = 40
};

volatile static u08 puls_time_cnt = PULS_EQUAL_TIME_CNT_DEFAULT;


void vApplicationTickHook(void)
{
    IndicatorDataShow();

    if (IS_COUNTER_WATER_NOT_ACTIVE) {
	    if ( PumpShouldTurnOn ) {
    
            if ( CountPulse > 0 ) {
		        if ( puls_time_cnt > 0 ) {
		            --puls_time_cnt;
		        }
		        else {
			        --CountPulse;
		            puls_time_cnt = (IS_COUNT_WATER_ONE_FRONT) ? PULS_EQUAL_TIME_CNT_40MS : PULS_EQUAL_TIME_CNT_20MS;
		        }
		    }  
	    }
	    else {
	        puls_time_cnt = (IS_COUNT_WATER_ONE_FRONT) ? PULS_EQUAL_TIME_CNT_40MS : PULS_EQUAL_TIME_CNT_20MS;
	    } 
    }
}


void SellingStop(void) {

    PumpShouldTurnOn = 0;

	portENTER_CRITICAL();

    if (IS_BOARD_VERSION_NEW) {
		PUMP_OFF;
	}
	else {
	    PUMP_ON;
	}

	KLAPAN1_OFF;
	KLAPAN2_OFF;
	KLAPAN3_OFF;

	portEXIT_CRITICAL();	
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


void SaveEvent (u08 *time_and_date_buf, const u16 cntmaney, const u16 cntwater, const u08 coin_cntr, const u08 bill_cntr, u08 event) {

    u08 EventBuff[13];
	u08 AdrEventBuff[2];
	u08 i;
		

	for(i = 0; i < 6; i++) {
		EventBuff[i] = time_and_date_buf[i];
	}
	
	EventBuff[6] = (cntmaney >> 8);
	EventBuff[7] = (cntmaney & 0x00FF);
	EventBuff[8] = (cntwater >> 8);
	EventBuff[9] = (cntwater & 0x00FF);
/*	EventBuff[10] = event;*/

	EventBuff[10] = coin_cntr;
	EventBuff[11] = bill_cntr;
	EventBuff[12] = event;

//    Global_Time_Deluy(5);
    if (*ext_eepr_data_adr < (*ext_eepr_cur_adr + EXT_EEPR_LINE_WRT_SIZE)) { 
	    ADR_LAST_DATTA  = *ext_eepr_cur_adr;
        *ext_eepr_cur_adr = 0x0000;

	    AdrEventBuff[0] = (u08)(ADR_LAST_DATTA >> 8);
	    AdrEventBuff[1] = (u08)(ADR_LAST_DATTA & 0x00FF);
	    i2ceepromWriteBloc(*ext_eepr_data_adr, AdrEventBuff, 2);
		
    	Global_Time_Deluy(5);
	}
   
    i2ceepromWriteBloc(*ext_eepr_cur_adr, EventBuff, EXT_EEPR_LINE_WRT_SIZE);
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
	    if (CntNoWrkBill == 25000) {
			key_kode |= (1 << 9);
			CntNoWrkBill = 25000 + 10;
		}    
        else if (CntNoWrkBill < 25000) {
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

//=================================

	if (!BTN_RESERV1) {
	    if (CntReserv1Press == 20) {
			key_kode |= (1 << 11);
			CntReserv1Press = 30;
		}    
        else if (CntReserv1Press < 20) {
		    CntReserv1Press++;
        }
	}
	else {
		key_kode &= ~(1 << 11);
	    CntReserv1Press = 0;
	}
	

    return key_kode;	
}

//=====================================================================


void GetCmd (unsigned char *get_cmd_buff) {

extern TimeAndDate Time_And_Date_System;

    void SET_THE_RTC (void) {

		get_cmd_buff[8] = get_cmd_buff[7];
		get_cmd_buff[7] = get_cmd_buff[6];
		get_cmd_buff[6] = get_cmd_buff[5];
		get_cmd_buff[5] = 1;

//		portENTER_CRITICAL();
		DS1337WriteDatta((get_cmd_buff+2));
		
		TimeAndDateRtcRead(&Time_And_Date_System);
//		portENTER_CRITICAL();
		
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}
	
	void RAEAD_FROM_RTC (void) {

 //       portENTER_CRITICAL();
	    TimeAndDateRtcRead(&Time_And_Date_System);
//		portENTER_CRITICAL();
		
        TimeAndDayToBcd((TimeAndDate *)&get_cmd_buff[2], Time_And_Date_System);

		get_cmd_buff[0] = 7;

		uartSendBuf(0, &get_cmd_buff[0], 8);
	}

	void READ_EXT_EEPROM (void) {

		u16 start_memAddr_for_read = 0;
	    u16 stop_memAddr_for_read = *ext_eepr_cur_adr; 
        u16 start_adr_uart_rx_buf = (((u16)get_cmd_buff[3]) << 8) + get_cmd_buff[2];
        u16 stop_adr_uart_rx_buf  = (((u16)get_cmd_buff[5]) << 8) + get_cmd_buff[4];
        
	   	
		if (start_adr_uart_rx_buf < (*ext_eepr_data_adr - EXT_EEPR_LINE_WRT_SIZE)) {
		    start_memAddr_for_read = start_adr_uart_rx_buf;
		}


        if ((stop_adr_uart_rx_buf < *ext_eepr_data_adr) && (stop_adr_uart_rx_buf != 0)) {
    	    stop_memAddr_for_read = stop_adr_uart_rx_buf;
		}
		else if ((ADR_LAST_DATTA != 0) && (start_memAddr_for_read == 0)) {

            start_memAddr_for_read = *ext_eepr_cur_adr;
								 
		    while (start_memAddr_for_read < ADR_LAST_DATTA) {
		
		    	i2ceepromReadBloc(start_memAddr_for_read, (get_cmd_buff+2), EXT_EEPR_LINE_WRT_SIZE);
	
			    start_memAddr_for_read += EXT_EEPR_LINE_WRT_SIZE;
			    get_cmd_buff[0] = 12 + 2;
			    
			    uartSendBuf(0, &get_cmd_buff[0], EXT_EEPR_LINE_WRT_SIZE + 2);
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
		portENTER_CRITICAL();
		IntEeprWordWrite(ExtEeprCarAdrEEPROMAdr, *ext_eepr_cur_adr);
		portENTER_CRITICAL();

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
		
		portENTER_CRITICAL();												
		eeprom_busy_wait();
		eeprom_read_block ((uint16_t *)(&EEPROM_DATA[0]),(uint16_t *)*(&EepromAdr), 16);
        portEXIT_CRITICAL();

		portENTER_CRITICAL();												
		eeprom_busy_wait();
		eeprom_read_block ((uint16_t *)(&EEPROM_DATA[8]),(uint16_t *)*(&EepromAdr), 24);
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
		
		portENTER_CRITICAL();		
		eeprom_busy_wait();
		((uint16_t*) get_cmd_buff)[2] = eeprom_read_word ((uint16_t *)*(&((uint16_t*) get_cmd_buff)[1]));
		portEXIT_CRITICAL();
		get_cmd_buff[0] = 5;
				
		uartSendBuf(0, &get_cmd_buff[0], 6);
	}

	void WRITE_PULSE_COUNT (void) {
		portENTER_CRITICAL();		
		CountPulse = (get_cmd_buff[2] << 8);
		CountPulse = CountPulse + get_cmd_buff[3];
		portEXIT_CRITICAL();
			
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


inline void Create_Report_String (u08 *time_and_date_buf, u08 *report_buff, u08 EventNamber) {

	    u08 cnt_buf = 0;

        itoan(*vodomat_number, &report_buff[cnt_buf], 4); 
	
                                                                               /* Convert the date and time to ASCII */
        for (cnt_buf = 0; cnt_buf < 6; cnt_buf++) {
		    itoan(time_and_date_buf[cnt_buf], &report_buff[((cnt_buf * 2) + 4)], 2);
		}

		cnt_buf = 0;

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
			IsDataToReturnSent = 1;
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
        if (Fl_State_Water  >= '0' && Fl_State_RsvBill >= '0' && Fl_State_Power >= '0' && Fl_State_WtrCnt >= '0') {

            report_buff[32+cnt_buf] = Fl_State_Water;

            report_buff[33+cnt_buf] = Fl_State_RsvBill;

            report_buff[34+cnt_buf] = Fl_State_Power;
      
            report_buff[35+cnt_buf] = Fl_State_WtrCnt;
		}
		else {
		    report_buff[32+cnt_buf] = '0';

            report_buff[33+cnt_buf] = '0';

            report_buff[34+cnt_buf] = '0';
      
            report_buff[35+cnt_buf] = '0';
		}
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


void uartSendBuf(u08 num, u08 *s , u08 len) {
    
	while (len-- > 0)
	    uartSendByte(num, *s++);
}


static u08 bcd_to_i (u08 binval);
static u08 i_to_bcd (u08 digit);

u16 GetTimeAsMinute (TimeAndDate *time_and_date) {

    return (time_and_date->Hour * 60 + time_and_date->Minute);
}


u08 TimeAndDateRtcRead (TimeAndDate *time_and_date)
{
    u08 time_and_date_buf[7];

	DS1337ReadDatta(time_and_date_buf);

                                                                               /* chack if read data correct */
    if (time_and_date_buf[0] < 0x60 && time_and_date_buf[1] <  0x60 
	                                && time_and_date_buf[2] <  0x24
									&& time_and_date_buf[4] >= 0x01
									&& time_and_date_buf[4] <= 0x31
									&& time_and_date_buf[5] >= 0x01
									&& time_and_date_buf[5] <= 0x12
									&& time_and_date_buf[6] >= 0x15 
									&& time_and_date_buf[6] <= 0x99) {

	    time_and_date->Second = bcd_to_i(time_and_date_buf[0]);   
        time_and_date->Minute = bcd_to_i(time_and_date_buf[1]);
        time_and_date->Hour   = bcd_to_i(time_and_date_buf[2]);
        time_and_date->Day    = bcd_to_i(time_and_date_buf[4]);
        time_and_date->Month  = bcd_to_i(time_and_date_buf[5]);
        time_and_date->Year   = bcd_to_i(time_and_date_buf[6]);

		return 0;
	}

	return 1;
}


void TimeAndDateRtcWrite (TimeAndDate *time_and_date_bcd)
{
    u08 time_and_date_buf[7];

	time_and_date_buf[0] = time_and_date_bcd->Second;   
    time_and_date_buf[1] = time_and_date_bcd->Minute;
    time_and_date_buf[2] = time_and_date_bcd->Hour;
    time_and_date_buf[4] = time_and_date_bcd->Day;
    time_and_date_buf[5] = time_and_date_bcd->Month;
    time_and_date_buf[6] = time_and_date_bcd->Year;
    
	time_and_date_buf[3] = 0; 

    DS1337WriteDatta(time_and_date_buf);
}


void TimeAndDayToBcd (TimeAndDate *time_and_date_to, TimeAndDate time_and_date_from) 
{
    time_and_date_to->Second = i_to_bcd(time_and_date_from.Second);
	time_and_date_to->Minute = i_to_bcd(time_and_date_from.Minute);
	time_and_date_to->Hour   = i_to_bcd(time_and_date_from.Hour);
	time_and_date_to->Day    = i_to_bcd(time_and_date_from.Day);
	time_and_date_to->Month  = i_to_bcd(time_and_date_from.Month);
	time_and_date_to->Year   = i_to_bcd(time_and_date_from.Year);
}


void TimeAndDayFromRegStr (TimeAndDate *time_and_date_to, u08 *str_from) 
{
/*
    u08 *time_date = (u08 *)time_and_date_to;
//	if (str_from != NULL) {

        u08 i = sizeof (*time_and_date_to) / sizeof (time_and_date_to->Second);
        for (i = 0; str_from > 0; i++) {
	        time_date[i] = atoin(str_from[i], 1);
	    }
//	}
*/
    time_and_date_to->Day   = atoin(str_from, 2);
	str_from += 2;
	time_and_date_to->Month = atoin(str_from, 2);
	str_from += 2;
	time_and_date_to->Year    = atoin(str_from, 2);
	str_from += 3;
	time_and_date_to->Hour  = atoin(str_from, 2);
	str_from += 2;
	time_and_date_to->Minute   = atoin(str_from, 2);
	time_and_date_to->Second = 0;
}


void TimeAndDateDefaultSet (TimeAndDate *time_and_date)
{
    time_and_date->Second = 0;   
    time_and_date->Minute = 0;
    time_and_date->Hour   = 0;
    time_and_date->Day    = 1;
    time_and_date->Month  = 1;
    time_and_date->Year   = START_POINT_YEAR;
}


inline static u08 days_in_curr_month (u08 mounth_current, u16 year_current);


void TimeAndDateSecAdd (TimeAndDate *time_and_date)
{
    if ((++time_and_date->Second) == SECONDS_MAX) {
	    time_and_date->Second = 0;
	    
		if ((++time_and_date->Minute) == MINUTES_MAX) {
		    time_and_date->Minute = 0;
		
		    if ((++time_and_date->Hour) == HOURS_MAX) {
			    time_and_date->Hour = 0;
			
			    if ((++time_and_date->Day) == days_in_curr_month(time_and_date->Month, time_and_date->Year)) {
				    time_and_date->Day = 0;
				
				    if ((++time_and_date->Month) == MONTH_MAX) {
					    time_and_date->Month = 0;
					
					    if ((++time_and_date->Year) == YEAR_MAX) {
						    time_and_date->Year = 0;
						}
					}
				}
			}
		}
	}
}


u16 HoursToBlocking (TimeAndDate *hour_cur, TimeAndDate *hour_transmit) 
{
    u16 h;
     
    h = 0;
    if (hour_transmit->Day < hour_cur->Day) {
	    h = (hour_cur->Day - hour_transmit->Day) * 24;
	}

    if (hour_transmit->Hour < (h + hour_cur->Hour)) {
	    h += hour_cur->Hour;
		h -= hour_transmit->Hour;
	}

	return h;
}


inline static u08 days_in_curr_month (u08 mounth_current, u16 year_current)
{
    u08 days_num;
	//leap = (year % 4 == 0 && year % 1000 != 0) || year % 400 == 0;
	
	year_current += 2000;
	
	days_num = Days_In_Month_Buf[mounth_current-1];

	if (mounth_current == 2) {
	    days_num += ((year_current % 4 == 0 && year_current % 1000 != 0) || year_current % 400 == 0);
	}
	
	return days_num;
}


inline static u08 bcd_to_i(u08 binval) {

    u08 val, temp = 0;
    
    val = (binval & 0xF0);
	binval = (binval & 0x0F);

    while (val > 0)  {
        val -= 16;
		temp += 10;
    }
       
    return (temp + binval);
}

inline static u08 i_to_bcd (u08 digit)
{
    u08 ret;

	ret = 0;	
	if (digit != 0) {
	    ret = digit % 10;
        ret += (digit / 10) * 16;
	}

	return ret;
} 


u16 atoin (u08 *s, u08 n)
{ 
    u16 ret;

	ret = 0;
    while (n--) {
    
	    if (*s < '0' && '9' < *s)
	        return 0;

	    ret = ret * 10 + *s - '0'; 
		s++;
	}

	return ret;
}
