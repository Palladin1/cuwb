#ifndef COMMAND_H
#define COMMAND_H

#include "portsIO.h"


#define  START_POINT_YEAR    15u

#define  SECONDS_MAX    60u
#define  MINUTES_MAX    60u
#define  HOURS_MAX      24u
#define  MONTH_MAX      12u
#define  YEAR_MAX       99u

#define  EXT_EEPR_LINE_WRT_SIZE    15


typedef struct {
    u08 Second;
    u08 Minute; 
    u08 Hour;
    u08 Day;
    u08 Month;
    u08 Year;
} TimeAndDate;


typedef struct {
    u08 Minut; 
    u08 Hour;
    u08 Day;
    u08 Month;
    u08 Year;
} ENCASHMENT_DATETIME_T;

volatile typedef struct {  //ENCASHMENT_T {
    ENCASHMENT_DATETIME_T DateTime;
	MONEY_COUNTER_T       Money;
} ENCASHMENT_T;


struct COLLECTION_DATA_TO_SERVER {
    u16 *AparatNum;
    ENCASHMENT_DATETIME_T DateTime;
	MONEY_COUNTER_T       Money;
	u32 *WaterQnt;
	u16 *Price;
	u08 *TimeToBlock;
	u08 *Flag1;
	u08 *Flag2;
	u08 *Flag3;
	u08 *Flag4;
	u08 *Flag5;
	u08 *Flag6;
	u08  EventNum;
} ;


/*
struct MONEY_EXTRACT {
    struct ENCASHMENT_T Cur;
    u16 *FirstAdress;
	u08  Num;
} ;
*/

volatile struct {
    u16 Money;
    u16 Water;
} ShouldReturnToBuyer;

u08 IsDataToReturnSent;

volatile struct {
    u08 Water;
    u08 Power;
    u08 RsvBill;
    u08 WtrCnt;
    u08 RrState;
    u08 Reserve;
} Fl_State;

void SellingStart(void);
void SellingStop(void);

void StartGetManey(void);
void StopGetManey(void);

void SaveEvent (u08 *time_and_date_buf, const u16 cntmaney, const u16 cntwater, const u08 coin_cntr, const u08 bill_cntr, u08 event);

u16 KeySkan (u16 key_kode);


void GetCmd (unsigned char *get_cmd_buff);
extern void Global_Time_Deluy (unsigned int time_val);

void SendError (u08 EventNamber);

void itoan (u32 binval, u08 *asc, u08 size);
void hextoa2 (u08 binval, u08 *asc);

u16 GetTimeAsMinute (TimeAndDate *time_and_date);
u08 TimeAndDateRtcRead (TimeAndDate *time_and_date);
void TimeAndDateRtcWrite (TimeAndDate *time_and_date_bcd);
void TimeAndDayToBcd (TimeAndDate *time_and_date_to, TimeAndDate time_and_date_from);
void TimeAndDateDefaultSet (TimeAndDate *time_and_date);
void TimeAndDateSecAdd (TimeAndDate *time_and_date);
void TimeAndDayFromRegStr (TimeAndDate *time_and_date_to, u08 *str_from);
u16 HoursToBlocking (TimeAndDate *hour_cur, TimeAndDate *hour_transmit);

void Get_Pfone_Number (u08 *phone_buff);
void Create_Report_String (struct COLLECTION_DATA_TO_SERVER *data, u08 *report_buff);

void Uart1Enable (void);
void Uart1Disable (void);

void uartSendBuf (u08 num, u08 *s , u08 len);

u16 atoin (u08 *s, u08 n);

ENCASHMENT_T QueueEncashmentInit (void);
void QueueEncashmentPut (ENCASHMENT_T *data);
void QueueEncashmentGet (ENCASHMENT_T *data, u08 with_remove);
u08 QueueEncashmentNum (void);


#endif    /* COMMAND_H */
