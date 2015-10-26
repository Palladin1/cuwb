#ifndef COMMAND_H
#define COMMAND_H


typedef struct {
    u08 Second;
    u08 Minute; 
    u08 Hour;
    u08 Day;
    u08 Month;
    u08 Year;
} TimeAndDate;


    
#define  START_POINT_YEAR    15u

#define  SECONDS_MAX    60u
#define  MINUTES_MAX    60u
#define  HOURS_MAX      24u
#define  MONTH_MAX      12u
#define  YEAR_MAX       99u


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

u16 MoneyToReturn;
u16 WaterToReturn;

u08 IsDataToReturnSent;

u08 Fl_State_Water;
u08 Fl_State_Power;
u08 Fl_State_RsvBill;
u08 Fl_State_WtrCnt;


void SellingStart(void);
void SellingStop(void);

void StartGetManey(void);
void StopGetManey(void);

void SaveEvent (u08 *time_and_date_buf, const u16 cntmaney, const u16 cntwater, u08 event);

u16 KeySkan (u16 key_kode);


void GetCmd (unsigned char *get_cmd_buff);
extern void Global_Time_Deluy (unsigned int time_val);

void SendError (u08 EventNamber);

void itoan (u32 binval, u08 *asc, u08 size);
void hextoa2 (u08 binval, u08 *asc);

u16 GetTimeAsMinute (TimeAndDate *time_and_date);
u08 TimeAndDateRtcRead (TimeAndDate *time_and_date);
void TimeAndDayToBcd (TimeAndDate *time_and_date_to, TimeAndDate time_and_date_from);
void TimeAndDateDefaultSet (TimeAndDate *time_and_date);
void TimeAndDateSecAdd (TimeAndDate *time_and_date);

void Get_Pfone_Number (u08 *phone_buff);
void Create_Report_String (u08 *time_and_date_buf, u08 *report_buff, u08 EventNamber);

void Uart1Enable (void);
void Uart1Disable (void);

void uartSendBuf (u08 num, u08 *s , u08 len);


#endif    /* COMMAND_H */
