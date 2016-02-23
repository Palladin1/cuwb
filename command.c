//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/eeprom.h>

#include "global.h"		// include our global settings
#include "command.h"
//#include "portsIO.h"
#include "uart2.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "DS1337.h"	    // include DS1337 function library
#include "i2ceeprom.h"  // include i2ceeprom function library
#include "inteepr.h"

#include "portmacro.h"

#include  "indicator.h"
#include  <string.h>


static struct {
    u08 TmrBill;
    u08 TmrCoin;
    u08 NoWater;
    u08 NoPower1;
    u08 NoPower2;
    u08 DoorOpn;
    u08 Start;
    u08 Stop;
    u08 Reset;
    u16 NoWrkBill;
    u08 RegPresent;
    u08 Reserv1Press;
} Cntr;


static u08 PumpShouldTurnOn = 0;

static const u08 Days_In_Month_Buf[MONTH_MAX] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

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

void StopGetManey(void)
{
	
	INHIBIT_COIN_ON;
	INHIBIT_BILL_ON;
}


void SaveEvent (u08 *time_and_date_buf, const u16 cntr_money, const u16 cntr_water, const u16 cntr_bill, const u16 cntr_coin, u08 event)
{

    u08 EventBuff[EXT_EEPR_LINE_WRT_SIZE];
	u08 AdrEventBuff[2];
	u08 i;
		

	for(i = 0; i < 6; i++) {
		EventBuff[i] = time_and_date_buf[i];
	}
	
	EventBuff[6] = (cntr_money >> 8);
	EventBuff[7] = (cntr_money & 0x00FF);
	EventBuff[8] = (cntr_water >> 8);
	EventBuff[9] = (cntr_water & 0x00FF);

	EventBuff[10] = (cntr_bill >> 8);
	EventBuff[11] = (cntr_bill & 0x00FF);
	EventBuff[12] = (cntr_coin >> 8);
	EventBuff[13] = (cntr_coin & 0x00FF);

	EventBuff[14] = event;

//    Global_Time_Deluy(5);
    if (EEPR_LOCAL_COPY.ext_eepr_data_adr < (EEPR_LOCAL_COPY.ext_eepr_cur_adr + EXT_EEPR_LINE_WRT_SIZE)) { 
	    ADR_LAST_DATTA  = EEPR_LOCAL_COPY.ext_eepr_cur_adr;
        EEPR_LOCAL_COPY.ext_eepr_cur_adr = 0x0000;

	    AdrEventBuff[0] = (u08)(ADR_LAST_DATTA >> 8);
	    AdrEventBuff[1] = (u08)(ADR_LAST_DATTA & 0x00FF);
	    i2ceepromWriteBloc(EEPR_LOCAL_COPY.ext_eepr_data_adr, AdrEventBuff, 2);
		
    	Global_Time_Deluy(5);
	}
   
    i2ceepromWriteBloc(EEPR_LOCAL_COPY.ext_eepr_cur_adr, EventBuff, EXT_EEPR_LINE_WRT_SIZE);
	EEPR_LOCAL_COPY.ext_eepr_cur_adr += EXT_EEPR_LINE_WRT_SIZE;
	IntEeprWordWrite(ExtEeprCarAdrEEPROMAdr, EEPR_LOCAL_COPY.ext_eepr_cur_adr);
}



u16 KeySkan(u16 key_kode) { 

	if (COUNT_COIN) {
       
        if (Cntr.TmrCoin < (u08)((EEPR_LOCAL_COPY.coin_time_pulse_coef) >> 8)) {
		    Cntr.TmrCoin++;
        }
	}
	else {
	    if (Cntr.TmrCoin > 0) {
	        u08 TimeMax = ((EEPR_LOCAL_COPY.coin_time_pulse_coef) >> 8);
		    u08 TimeMin = ((EEPR_LOCAL_COPY.coin_time_pulse_coef) & 0x00FF);
			if ((Cntr.TmrCoin > TimeMin) && (Cntr.TmrCoin < TimeMax)) {
				 key_kode |= (1 << 0);		    }
			
		}
		else {
			key_kode &= ~(1 << 0);
	    }
		Cntr.TmrCoin = 0;
	}


	if (COUNT_BILL) {
        
        if (Cntr.TmrBill < (u08)((EEPR_LOCAL_COPY.bill_time_pulse_coef) >> 8)) {
		    Cntr.TmrBill++;
        }
	}
	else {
	    if (Cntr.TmrBill > 0) {
	        u08 TimeMax = ((EEPR_LOCAL_COPY.bill_time_pulse_coef) >> 8);
		    u08 TimeMin = ((EEPR_LOCAL_COPY.bill_time_pulse_coef) & 0x00FF);
			if ((Cntr.TmrBill > TimeMin) && (Cntr.TmrBill < TimeMax)) {
				key_kode |= (1 << 1);
		    }
			
		}
		else {
			key_kode &= ~(1 << 1);
		}
		Cntr.TmrBill = 0;
	}
    
//=================================
	
	if (!(WATER_PRESENT)) {	
 	    if (Cntr.NoWater == 20) {
            key_kode |= (1 << 2);
			Cntr.NoWater = 30;
		}    
        else if (Cntr.NoWater < 20) {
		    Cntr.NoWater++;
        }
	}
	else {
		key_kode &= ~(1 << 2);
	    Cntr.NoWater = 0;
	}
//=================================	

	if (STATUS_PWR_12V) {		      
        if (Cntr.NoPower1 == 20) {
			key_kode |= (1 << 3);
			Cntr.NoPower1 = 30;
		}    
        else if (Cntr.NoPower1 < 20) {
		    Cntr.NoPower1++;
        }
	}
	else {
		key_kode &= ~(1 << 3);
	    Cntr.NoPower1 = 0;
	}
//=================================

	if (STATUS_PWR_5V) {		      
        if (Cntr.NoPower2 == 20) {
			key_kode |= (1 << 4);
			Cntr.NoPower2 = 30;
		}    
        else if (Cntr.NoPower2 < 20) {
		    Cntr.NoPower2++;
        }
	}
	else {
		key_kode &= ~(1 << 4);
	    Cntr.NoPower2 = 0;
	}
//=================================	

	if (!(BTN_DOOR)) {				  
	    if (Cntr.DoorOpn == 20) {
			key_kode |= (1 << 5);
			Cntr.DoorOpn = 30;
		}    
        else if (Cntr.DoorOpn < 20) {
		    Cntr.DoorOpn++;
        }
	}
	else {
		key_kode &= ~(1 << 5);   
	    Cntr.DoorOpn = 0;
	}
//=================================
   	
	if (!(BTN_START)) {
	    if (Cntr.Start == 20) {
			key_kode |= (1 << 6);
			Cntr.Start = 30;
		}    
        else if (Cntr.Start < 20) {
		    Cntr.Start++;
        }
	}
	else {
		key_kode &= ~(1 << 6);
	    Cntr.Start = 0;
	}
//=================================

	if (!(BTN_STOP)) {
        if (Cntr.Stop == 20) {
			key_kode |= (1 << 7);
			Cntr.Stop = 30;
		}    
        else if (Cntr.Stop < 20) {
		    Cntr.Stop++;
        }
	}
	else {
		key_kode &= ~(1 << 7);
	    Cntr.Stop = 0;
	}
//=================================

	if (BTN_RESET) {
 	    if (Cntr.Reset == 20) {
			key_kode |= (1 << 8);
			Cntr.Reset = 30;
		}    
        else if (Cntr.Reset < 20) {
		    Cntr.Reset++;
        }
	}
	else {
		key_kode &= ~(1 << 8);
	    Cntr.Reset = 0;
	}
	
//=================================

	if (STATUS_COUNT_BILL) {
	    if (Cntr.NoWrkBill == 25000) {
			key_kode |= (1 << 9);
			Cntr.NoWrkBill = 25000 + 10;
		}    
        else if (Cntr.NoWrkBill < 25000) {
		    Cntr.NoWrkBill++;
        }
	}
	else {
		key_kode &= ~(1 << 9);
	    Cntr.NoWrkBill = 0;
	}
	
//=================================

	if (!BTN_REGISTRATOR_PRESENT) {
	    if (Cntr.RegPresent == 20) {
			key_kode |= (1 << 10);
			Cntr.RegPresent = 30;
		}    
        else if (Cntr.RegPresent < 20) {
		    Cntr.RegPresent++;
        }
	}
	else {
		key_kode &= ~(1 << 10);
	    Cntr.RegPresent = 0;
	}

//=================================

	if (!BTN_RESERV1) {
	    if (Cntr.Reserv1Press == 20) {
			key_kode |= (1 << 11);
			Cntr.Reserv1Press = 30;
		}    
        else if (Cntr.Reserv1Press < 20) {
		    Cntr.Reserv1Press++;
        }
	}
	else {
		key_kode &= ~(1 << 11);
	    Cntr.Reserv1Press = 0;
	}
	

    return key_kode;	
}

//=====================================================================


void GetCmd (unsigned char *get_cmd_buff) {

extern TimeAndDate TimeAndDate_System;

    void SET_THE_RTC (void) {

		get_cmd_buff[8] = get_cmd_buff[7];
		get_cmd_buff[7] = get_cmd_buff[6];
		get_cmd_buff[6] = get_cmd_buff[5];
		get_cmd_buff[5] = 1;

//		portENTER_CRITICAL();
		DS1337WriteDatta((get_cmd_buff+2));
		
		TimeAndDateRtcRead(&TimeAndDate_System);
//		portENTER_CRITICAL();
		
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}
	
	void RAEAD_FROM_RTC (void) {

 //       portENTER_CRITICAL();
	    TimeAndDateRtcRead(&TimeAndDate_System);
//		portENTER_CRITICAL();
		
        TimeAndDayToBcd((TimeAndDate *)&get_cmd_buff[2], TimeAndDate_System);

		get_cmd_buff[0] = 7;

		uartSendBuf(0, &get_cmd_buff[0], 8);
	}

	void READ_EXT_EEPROM (void) {

		u16 start_memAddr_for_read = 0;
	    u16 stop_memAddr_for_read = EEPR_LOCAL_COPY.ext_eepr_cur_adr; 
        u16 start_adr_uart_rx_buf = (((u16)get_cmd_buff[3]) << 8) + get_cmd_buff[2];
        u16 stop_adr_uart_rx_buf  = (((u16)get_cmd_buff[5]) << 8) + get_cmd_buff[4];
        
	   	
		if (start_adr_uart_rx_buf < (EEPR_LOCAL_COPY.ext_eepr_data_adr - EXT_EEPR_LINE_WRT_SIZE)) {
		    start_memAddr_for_read = start_adr_uart_rx_buf;
		}


        if ((stop_adr_uart_rx_buf < EEPR_LOCAL_COPY.ext_eepr_data_adr) && (stop_adr_uart_rx_buf != 0)) {
    	    stop_memAddr_for_read = stop_adr_uart_rx_buf;
		}
		else if ((ADR_LAST_DATTA != 0) && (start_memAddr_for_read == 0)) {

            start_memAddr_for_read = EEPR_LOCAL_COPY.ext_eepr_cur_adr;
								 
		    while (start_memAddr_for_read < ADR_LAST_DATTA) {
		
		    	i2ceepromReadBloc(start_memAddr_for_read, (get_cmd_buff+2), EXT_EEPR_LINE_WRT_SIZE);
	
			    start_memAddr_for_read += EXT_EEPR_LINE_WRT_SIZE;
			    get_cmd_buff[0] = EXT_EEPR_LINE_WRT_SIZE + 1;
			    
			    uartSendBuf(0, &get_cmd_buff[0], EXT_EEPR_LINE_WRT_SIZE + 2);

                Global_Time_Deluy(20);
    	    }
			start_memAddr_for_read = 0;
		} 
		
		while (start_memAddr_for_read < stop_memAddr_for_read) {
		
			i2ceepromReadBloc(start_memAddr_for_read, (get_cmd_buff + 2), EXT_EEPR_LINE_WRT_SIZE);
	
			start_memAddr_for_read += EXT_EEPR_LINE_WRT_SIZE;
			get_cmd_buff[0] = EXT_EEPR_LINE_WRT_SIZE + 1;
			
			uartSendBuf(0, &get_cmd_buff[0], EXT_EEPR_LINE_WRT_SIZE + 2);

            Global_Time_Deluy(20);
    	}


		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}
	
	void ERASE_EXT_EEPROM (void)	{	

        get_cmd_buff[2] = 0;
		get_cmd_buff[3] = 0;
		i2ceepromWriteBloc(EEPR_LOCAL_COPY.ext_eepr_data_adr, (get_cmd_buff + 2), 2);
		ADR_LAST_DATTA = 0;
		EEPR_LOCAL_COPY.ext_eepr_cur_adr = 0;
	
		portENTER_CRITICAL();
		IntEeprWordWrite(ExtEeprCarAdrEEPROMAdr, EEPR_LOCAL_COPY.ext_eepr_cur_adr);
		portEXIT_CRITICAL();

		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}

	void WRITE_INT_EEPROM (void) {

		u16 tmp_ext_eepr_data_adr = EEPR_LOCAL_COPY.ext_eepr_data_adr;

		portENTER_CRITICAL();
		IntEeprWordWrite((uint16_t)*(&((uint16_t*) get_cmd_buff)[1]), ((uint16_t*)get_cmd_buff)[2]);
		portEXIT_CRITICAL();
			
		portENTER_CRITICAL();												
        IntEeprBlockRead((uint16_t)(&EEPR_LOCAL_COPY.cost_litre_coef), CostLitreCoefEEPROMAdr, sizeof(EEPR_LOCAL_COPY));
        portEXIT_CRITICAL();

///////////////////////////////////////////////////////////////
        if (tmp_ext_eepr_data_adr != EEPR_LOCAL_COPY.ext_eepr_data_adr) {

            if ((EEPR_LOCAL_COPY.ext_eepr_data_adr - 1) >= ADR_LAST_DATTA ) {              
			    get_cmd_buff[2] = (u08)(ADR_LAST_DATTA >> 8);
	            get_cmd_buff[3] = (u08)(ADR_LAST_DATTA & 0x00FF);
                i2ceepromWriteBloc(EEPR_LOCAL_COPY.ext_eepr_data_adr, (get_cmd_buff + 2), 2);
			}
			else {
			    ADR_LAST_DATTA = 0x0000;
				get_cmd_buff[2] = (u08)(ADR_LAST_DATTA >> 8);
	            get_cmd_buff[3] = (u08)(ADR_LAST_DATTA & 0x00FF);
                i2ceepromWriteBloc(EEPR_LOCAL_COPY.ext_eepr_data_adr, (get_cmd_buff + 2), 2);
			}
		}
///////////////////////////////////////////////////////////////
		get_cmd_buff[0] = 1;
		uartSendBuf(0, &get_cmd_buff[0], 2);
	}

	void READ_INT_EEPROM (void) {
		
		portENTER_CRITICAL();		
		((uint16_t*) get_cmd_buff)[2] = IntEeprWordRead((uint16_t)*(&((uint16_t*)get_cmd_buff)[1]));
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


void Create_Report_String (struct COLLECTION_DATA_TO_SERVER *data, u08 *report_buff) 
{
    u08 cnt_buf;
	u08 i;
	u08 *p = (u08 *)&data->DateTime;

//	if ( !data || !report_buff) {
//	    return;
//	}

    cnt_buf = 0;
	     
    itoan(*data->ApparatNum, &report_buff[cnt_buf], 4); 
    cnt_buf += 4;                                      
		
	for (i = 0; i < 5; i++) {
	    itoan(p[i], &report_buff[cnt_buf + i * 2], 2);
	}
	cnt_buf += 10;

    itoan(data->Money.Sum, &report_buff[cnt_buf], 6);
    cnt_buf += 6;

	itoan(*data->WaterQnt, &report_buff[cnt_buf], 6);
	cnt_buf += 6;
	    
	itoan(*data->Price, &report_buff[cnt_buf], 4);
	cnt_buf += 4;

    itoan(*data->TimeToBlock, &report_buff[cnt_buf], 2);
	cnt_buf += 2;

	itoan(data->Money.Bill, &report_buff[cnt_buf], 4);
    cnt_buf += 4;
	
	itoan(data->Money.Bill, &report_buff[cnt_buf], 4);
    cnt_buf += 4;
/*
************************************************************ 
*       Sets the flags of stats from board unit     
************************************************************
*/
    for (i = 0; i < 6; i++) {
	    report_buff[cnt_buf+i] = data->Flag1[i] + '0';
	}
	cnt_buf += i;

/*
************************************************************ 
*       End flags sets 
************************************************************
*/
    itoan(data->EventNum, &report_buff[cnt_buf], 2);
	cnt_buf += 2;
	report_buff[cnt_buf] = 0;
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
	
	days_num = pgm_read_byte(&Days_In_Month_Buf[mounth_current-1]);

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

#define  QUEUE_ENCASHMENT_LEN_MAX    3u


struct QUEUE_ENCASHMENT_T {
    ENCASHMENT_T *Head;
    ENCASHMENT_T *Tail;
    u08  Num;
};


static  struct QUEUE_ENCASHMENT_T QueueEncashment = {0};
static  ENCASHMENT_T *QueueEncashmantDataStartAdr = (ENCASHMENT_T *)(EncashmentSaveEEPROMAdr + sizeof(QueueEncashment)); 
static  ENCASHMENT_T *QueueEncashmantDataEndAdr   = (ENCASHMENT_T *)(EncashmentSaveEEPROMAdr + sizeof(QueueEncashment) + sizeof(ENCASHMENT_T) * QUEUE_ENCASHMENT_LEN_MAX); 


ENCASHMENT_T QueueEncashmentInit (void)
{
    ENCASHMENT_T cur = {{0}, {0}};

    IntEeprBlockRead((unsigned int)&QueueEncashment, EncashmentSaveEEPROMAdr, sizeof(QueueEncashment));

	if (QueueEncashment.Num > 0 && QueueEncashment.Num  <= QUEUE_ENCASHMENT_LEN_MAX
	                            && QueueEncashment.Tail >= QueueEncashmantDataStartAdr
								&& QueueEncashment.Tail <  QueueEncashmantDataEndAdr
	                            && QueueEncashment.Head >= QueueEncashmantDataStartAdr
								&& QueueEncashment.Head <  QueueEncashmantDataEndAdr   ) {
	    IntEeprBlockRead((unsigned int)&cur, (unsigned int)QueueEncashment.Tail, sizeof(ENCASHMENT_T));
	}
	else {
	    QueueEncashment.Tail = QueueEncashmantDataStartAdr;
		QueueEncashment.Head = QueueEncashmantDataStartAdr;
		QueueEncashment.Num = 0;
	}

	return cur;
}


void QueueEncashmentPut (ENCASHMENT_T *data)
{
    if (!data) {
	    return;
	}

    IntEeprBlockWrite((unsigned int)data, (unsigned int)QueueEncashment.Head, sizeof(ENCASHMENT_T));

    if (QueueEncashment.Num < QUEUE_ENCASHMENT_LEN_MAX) {
        QueueEncashment.Num++;
	}
	else  if (QueueEncashment.Head == QueueEncashment.Tail) {
       
	    QueueEncashment.Tail++;
		if (QueueEncashment.Tail == QueueEncashmantDataEndAdr) {
		    QueueEncashment.Tail = QueueEncashmantDataStartAdr;
	    }
	}

	QueueEncashment.Head++;
    if (QueueEncashment.Head == QueueEncashmantDataEndAdr) {
	    QueueEncashment.Head = QueueEncashmantDataStartAdr;
	}

	IntEeprBlockWrite((unsigned int)&QueueEncashment, EncashmentSaveEEPROMAdr, sizeof(QueueEncashment));
}


void QueueEncashmentGet (ENCASHMENT_T *data, u08 with_remove)
{
    u08 tmp;
	ENCASHMENT_T clr = {{0}};
    if (!data) {
	    return;
	}

    if (QueueEncashment.Num > 0) {
        IntEeprBlockRead((unsigned int)data, (unsigned int)QueueEncashment.Tail, sizeof(ENCASHMENT_T));
	    
		if (with_remove) {

		    QueueEncashment.Num--;
            tmp = 0;
			IntEeprBlockWrite((unsigned int)&clr, (unsigned int)(QueueEncashment.Tail), sizeof(ENCASHMENT_T));

		    QueueEncashment.Tail++;
            if (QueueEncashment.Tail == QueueEncashmantDataEndAdr) {
		        QueueEncashment.Tail = QueueEncashmantDataStartAdr;
	        }  

			IntEeprBlockWrite((unsigned int)&QueueEncashment, EncashmentSaveEEPROMAdr, sizeof(QueueEncashment));			
		}
    }
}


u08 QueueEncashmentNum (void)
{
	return QueueEncashment.Num;
}
