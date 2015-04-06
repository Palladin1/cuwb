#ifndef COMMAND_H
#define COMMAND_H

u08 CntTmrBill;
u08 CntTmrCoin;
u08 CntNoWater;
u08 CntNoPower1;
u08 CntNoPower2;
u08 CntDoorOpn;
u08 CntStart;
u08 CntStop;
u08 CntReset;
u08 CntNoWrkBill;
u08 CntRegPresent;

volatile u08 Fl_State_Water;
volatile u08 Fl_State_Power;
volatile u08 Fl_State_RsvBill;
volatile u08 Fl_State_WtrCnt;

void SellingStart(void);
void SellingStop(void);

void StartGetManey(void);
void StopGetManey(void);

void SaveEvent(const u16 cntmaney, const u16 cntwater, u08 event);

u16 KeySkan(u16 key_kode);


void GetCmd(unsigned char *get_cmd_buff);
extern void Global_Time_Deluy (unsigned int time_val);

void SendError(u08 EventNamber);

void itoan (u32 binval, u08 *asc, u08 size);
void hextoa2(u08 binval, u08 *asc);

u16 GetRealTime(void);
u08 bcdtoi2(u08 binval);


void Get_Pfone_Number (u08 *phone_buff);
void Create_Report_String (u08 *report_buff, u08 EventNamber);

void Uart1Enable (void);
void Uart1Disable (void);

void uartSendBuf(u08 num, u08 *s , u08 len);

#endif
