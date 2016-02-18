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


/*
struct MONEY_EXTRACT {
    struct ENCASHMENT_T Cur;
    u16 *FirstAdress;
	u08  Num;
} ;
*/

u08 CntTmrBill;
u08 CntTmrCoin;
u08 CntNoWater;
u08 CntNoPower1;
u08 CntNoPower2;
u08 CntDoorOpn;
u08 CntStart;
u08 CntStop;
u08 CntReset;
u16 CntNoWrkBill;
u08 CntRegPresent;
u08 CntReserv1Press;

u16 MoneyToReturn;
u16 WaterToReturn;

u08 IsDataToReturnSent;

volatile u08 Fl_State_Water;
volatile u08 Fl_State_Power;
volatile u08 Fl_State_RsvBill;
volatile u08 Fl_State_WtrCnt;
volatile u08 Fl_State_RrState;
volatile u08 Fl_State_Reserve;

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
void Create_Report_String (u08 *time_and_date_buf, u08 *report_buff, u08 EventNamber);

void Uart1Enable (void);
void Uart1Disable (void);

void uartSendBuf (u08 num, u08 *s , u08 len);

u16 atoin (u08 *s, u08 n);

ENCASHMENT_T QueueEncashmentInit (void);
void QueueEncashmentPut (ENCASHMENT_T *data);
void QueueEncashmentGet (ENCASHMENT_T *data, u08 with_remove);
u08 QueueEncashmentNum (void);


#endif    /* COMMAND_H */
