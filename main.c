/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*
* Filename      : main.c
* Version       : V1.00
* Programmer(s) : Palladin
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "includes.h"

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/
const char  AT[]                      PROGMEM = "AT\n";
const char  ECHO_OFF[] 	              PROGMEM = "ATE0\n";                  
const char  SET_BAUD[]                PROGMEM = "AT+IPR=19200\n";	      
const char  SAVE_FLESH[]              PROGMEM = "AT&W\n";		          

const char  PFONE_BOOK[]              PROGMEM = "AT+CPBS=\"SM\"\n";        // Pfone book on sim
//const char  PFONE_BOOK[]            PROGMEM = "AT+CPBS=\"SM\",\"SM\",\"SM\"";
const char  CLEAR_SMS[]               PROGMEM = "AT+CMGD=1\n";         
const char  INDICATION_NEW_SMS[]      PROGMEM = "AT+CNMI=1,0,0,0,0\n";   
const char  SET_TEXT_MODE[]           PROGMEM = "AT+CMGF=1\n";
const char  SET_FORMAT_1_SMS[]        PROGMEM = "AT+CSCS=\"GSM\"\n";
const char  SET_FORMAT_2_SMS[]        PROGMEM = "AT+CSMP=17,200,0,25\n";
const char  SEND_SMS[] 	              PROGMEM = "AT+CMGS=";

//GPRS Connection
const char  SET_GPRS_FORMAT[]         PROGMEM = "AT+QIFGCNT=0\n"; 
const char  SET_TYPE_CONNECTION[]     PROGMEM = "AT+QICSGP=1,";       // "1,APN","user","password"
const char  TRANSPARENT_MODE[]        PROGMEM = "AT+QIMODE=1\n";       // 
const char  NON_TRANSPARENT_MODE[]    PROGMEM = "AT+QIMODE=0\n";       //
const char  MULTIPLE_CONNECTION[]     PROGMEM = "AT+QIMUX=1\n";       // 
const char  SINGLE_CONNECTION[]       PROGMEM = "AT+QIMUX=0\n";
const char  CONNECT_BY_DOMAIN_NAME[]  PROGMEM = "AT+QIDNSIP=1\n";     // "0" - use IP adress, "1" - use domain name
const char  CONNECT_BY_IP_ADDRESS[]   PROGMEM = "AT+QIDNSIP=0\n";     // "0" - use IP adress, "1" - use domain name

const char  TCPIP_STACK_CONNECT[]    PROGMEM = "AT+QIREGAPP\n";    
const char  ACTIVEATE_FGCNT[]         PROGMEM = "AT+QIACT\n"; 
const char  QUERY_IP[]                PROGMEM = "AT+QILOCIP\n";    
const char  CONNECT_TO_SERVER[]       PROGMEM = "AT+QIOPEN=\"TCP\",";
const char  SEND_GPRS_DATA[]          PROGMEM = "AT+QISEND\n";
const char  CHECK_STATUS_CONNECTION[] PROGMEM = "AT+QISTAT\n";

const char  MODE_SERVER_OR_CLIENT[]   PROGMEM = "AT+QISRVC?\n";
const char  MODE_SERVER_SET[]         PROGMEM = "AT+QISRVC=2\n";
const char  MODE_CLIENT_SET[]         PROGMEM = "AT+QISRVC=1\n";

const char  DISCONNECT_SESSION[]      PROGMEM = "AT+QICLOSE\n";
const char  DISCONNECT_GPRS[]         PROGMEM = "AT+QIDEACT\n";
const char  GET_QUALITY_OF_NET[]      PROGMEM = "AT+CSQ\n";

//const char  VISUAL_DIAL_NUM[]       PROGMEM = "AT+CLIP=1\n";

const char  Get[]  PROGMEM = "GET /";
const char  Http[] PROGMEM = " HTTP/1.1\n";
const char  Data[] PROGMEM = "?data=";
const char  Pass[] PROGMEM = "&pass=";
const char  Host[] PROGMEM = "Host: ";
const char  Conn[] PROGMEM = "Connection: Keep-Alive\n\n\32"; // \32 - Ctrl^z


#define  ACCELEROMETER_PERIOD    3000u           //3000 * 100mS = 300S
#define  CHECK_COUNTER_PERIOD    50u             //50  * 100mS = 5S

#define  MINUTES_IN_DAY        1440ul    


#define  TIME_SEND_REGUEST    50ul   // 1 second = x * 100ms - is task sleep    

#define  REPORT_FLAG_OK     0
#define  REPORT_FLAG_ERR    1
/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/

TimeAndDate TimeAndDate_System = {0};

typedef enum {
	    
    STATE_NET_STATUS,
	STATE_NET_QUALITY_LOW,
	STATE_MODEM_IDLE,
	STATE_MODEM_ON,
	STATE_MODEM_OFF,
	STATE_MODEM_FIRST_INIT,
    STATE_MODEM_INIT,
	STATE_SOME_WAIT,
//    STATE_MODEM_CHECK,

    STATE_GPRS_CONNECT,
	STATE_GPRS_CHECK,
	STATE_GPRS_OPEN,
	STATE_GPRS_FAIL,
    STATE_GPRS_DEACTIVATE,
	STATE_GPRS_DISCONNECT,
    STATE_GPRS_FORMED_BUFF,
	STATE_HTTP_SERVER_200_OK,
	STATE_STACK_MODE_CHACK,
	STATE_STACK_MODE_CHANGE,
    STATE_GPRS_SEND_DATA

  
} CARRENT_STATE_CARRENT;


/* The events which sends to server by GSM modem */
enum {
    Fl_Ev_NoWater               =  1,          
    Fl_Ev_NoPower               =  2,
    Fl_Ev_TakeManey             =  3,
    Fl_Ev_GetMoving             =  4,
    Fl_Ev_LimWater              =  5,
    Fl_Ev_RequestData           =  6,
    Fl_Ev_ErrorBill             =  7,
	Fl_Ev_RegError              =  8,
    Fl_Ev_JustAfterReset        =  9,
	Fl_Ev_ServiceModeActivate   = 10,
	Fl_Ev_ServiceModeDeactivate = 11,
	Fl_Ev_ServiceOpening        = 12,  
	Fl_Ev_WillBlocked           = 13,
} SYSTEM_EVENTS;


CARRENT_STATE_CARRENT CARRENT_STATE = STATE_MODEM_IDLE;


u08 gFlLedStateWater = 0;

volatile u08 WtrCntTimer;
volatile u16 AccellCntTimer;

volatile u16 NewPrice = 0;


xQueueHandle xEventsQueue;

xSemaphoreHandle xUart_RX_Semaphore;

xSemaphoreHandle xI2CMutex;
xSemaphoreHandle xExtSignalStatusSem; 
xSemaphoreHandle xTimeSendRequestSem;

xSemaphoreHandle xGsmModemWaitingSem;

xTimerHandle xTimer_ButtonPoll;

xTimerHandle xTimer_NoWaterBuzzerSignal;
xTimerHandle xTimer_BuzzerOff;

xTimerHandle xTimer_ModemStart;

xTimerHandle xTimer_TimeBlockChack;


volatile u08 buzer_flag = 0;

static u16 ExtSignalStatus = 0;

volatile static u08 IsRegistratorConnect = 0;

static u16 Tmr_For_Init_Rr = 600; /* need time in Sec = Tmr_For_Init_Rr * 100ms, 600 * 100 = 60 S  */

volatile u08 Fl_Send_HourBeforeBlock = 0;
volatile u08 Fl_Send_TimeDateCurGet = 0;

volatile u08 Hours_BeforeApparatBlock = 0;

#if CHECK_STACK
#define TASK_NUMBER    5
volatile static u16 DebugBuff[TASK_NUMBER] = {0};
#endif //CHECK_STACK


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void vTask2( void *pvParameters );
void vTask3( void *pvParameters );
void vTask4( void *pvParameters );
void vTask5( void *pvParameters );
void vTask6( void *pvParameters );

void vCallback_ButtonPoll (xTimerHandle xTimer);

void vCallback_NoWaterBuzzerSignal (xTimerHandle xTimer);
void vCallback_BuzzerOff (xTimerHandle xTimer);

void vCallback_ModemStart (xTimerHandle xTimer);

void vCallback_TimerBlockCheck (xTimerHandle xTimer);

//void vCoRoutineBuzerControll (xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex);

static inline u16 MoneyToWater (u16 money_quantity);
static inline u16 MoneyToPulse (u16 money_quantity); 
static inline u16 PulseQuantityToMoney (u16 pulse_quantity);

void Uart0_Resiv (u08 udrdata);
void Global_Time_Deluy (unsigned int time_val);

//u08 ModemSendCom (u08 *buff, u32 deluy_time_ms);
u08 custom_at_handler(u08 *pData);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main( void )
{
    xEventsQueue = xQueueCreate(20, sizeof(u08 *));

    vSemaphoreCreateBinary(xUart_RX_Semaphore);
	vSemaphoreCreateBinary(xExtSignalStatusSem);
    vSemaphoreCreateBinary(xTimeSendRequestSem);

	vSemaphoreCreateBinary(xGsmModemWaitingSem);

	xI2CMutex = xSemaphoreCreateMutex();

    InitPortsIO();

    StopGetManey();

    RegistratorInit();

	IndicatorInit();

	QueueEncashmentInit();

#if MODEM_DBG
//Uart0Disable();
Uart0Enable(Uart0_Resiv,  19200);
#endif


    if (TimeAndDateRtcRead(&TimeAndDate_System) != 0) {                     /* if date and time don't read or correct it set default */
        TimeAndDateDefaultSet(&TimeAndDate_System);
    } 
	
////////////////////////////////////////////////////////////////////////////////////////////    

	xTaskCreate(vTask2, (signed char*) "T2", configMINIMAL_STACK_SIZE +  30, NULL, 1, NULL);         /*  40 */

    xTaskCreate(vTask3, (signed char*) "T3", configMINIMAL_STACK_SIZE +  60, NULL, 1, NULL);         /*  70 */

	xTaskCreate(vTask4, (signed char*) "T4", configMINIMAL_STACK_SIZE +  90, NULL, 2, NULL);         /*  70 */

    xTaskCreate(vTask5, (signed char*) "T5", configMINIMAL_STACK_SIZE + 220, NULL, 1, NULL);         /* 230 */
    
	xTaskCreate(vTask6, (signed char*) "T6", configMINIMAL_STACK_SIZE +  60, NULL, 1, NULL);         /*  60 */
 
	xTimer_ButtonPoll = xTimerCreate((signed char *)"K", 5 / portTICK_RATE_MS, pdTRUE, NULL, vCallback_ButtonPoll);
	xTimerReset(xTimer_ButtonPoll, 0);
    
	xTimer_NoWaterBuzzerSignal = xTimerCreate((signed char *)"W", 2000 / portTICK_RATE_MS, pdTRUE, NULL, vCallback_NoWaterBuzzerSignal);
    xTimerReset(xTimer_NoWaterBuzzerSignal, 0);

	xTimer_BuzzerOff = xTimerCreate((signed char *)"B", 200 / portTICK_RATE_MS, pdFALSE, NULL, vCallback_BuzzerOff);

	xTimer_ModemStart = xTimerCreate((signed char *)"M", 100 / portTICK_RATE_MS, pdFALSE, NULL, vCallback_ModemStart);

	xTimer_TimeBlockChack = xTimerCreate((signed char *)"R", 60000 / portTICK_RATE_MS, pdTRUE, NULL, vCallback_TimerBlockCheck);
	xTimerReset(xTimer_TimeBlockChack, 0);

//  xCoRoutineCreate(vCoRoutineBuzerControll, 1, 0);

	/* Запуск планировщика, после чего задачи запустятся на выполнение. */
	vTaskStartScheduler();

	for( ;; ) ;

    return (0);
}


void vCallback_ButtonPoll (xTimerHandle xTimer)
{
    static u16 temp_key = 0;

    ExtSignalStatus = KeySkan(ExtSignalStatus);
		
	if (ExtSignalStatus != temp_key) {
	    temp_key = ExtSignalStatus;
	    xSemaphoreGive(xExtSignalStatusSem);
	}
}


void vCallback_NoWaterBuzzerSignal (xTimerHandle xTimer)
{
    static u16 buzer_timer = BUZER_TIME;
	static u08 is_buzzer_on = 1;

    if (buzer_flag == 1) {
	    if (is_buzzer_on) {
		    is_buzzer_on = 0; 
	        BUZZER_ON;
			xTimerChangePeriod(xTimer_NoWaterBuzzerSignal, (300 / portTICK_RATE_MS), 0);
        }
		else {    
		    is_buzzer_on = 1;
			BUZZER_OFF;
			xTimerChangePeriod(xTimer_NoWaterBuzzerSignal, (1700 / portTICK_RATE_MS), 0);
		
	        if (buzer_timer == 0) {

				portENTER_CRITICAL();
	        	buzer_flag = 0;
		        portENTER_CRITICAL();
	            
	            buzer_timer = BUZER_TIME;
            }
            else {
	            buzer_timer--;
		    }
		}
	}
	else {
	    if (is_buzzer_on == 0) {
		    is_buzzer_on = 1;
			BUZZER_OFF;
		}

	    xTimerChangePeriod(xTimer_NoWaterBuzzerSignal, (2000 / portTICK_RATE_MS), 0);
	}
}


void vCallback_BuzzerOff (xTimerHandle xTimer)
{
   BUZZER_OFF; 
}


void vCallback_ModemStart (xTimerHandle xTimer)
{
    CARRENT_STATE = STATE_MODEM_ON;

	BUZZER_OFF;
}


void vCallback_TimerBlockCheck (xTimerHandle xTimer)
{
    static u08 chack_period = 60;
    	
	chack_period--;
	if (chack_period == 0) {
	    chack_period = 60;

		if (Fl_Send_HourBeforeBlock == 0) {
	        Fl_Send_HourBeforeBlock = 1;
	    }  
	}     
}


void vTask2( void *pvParameters )
{

    static u16 CountManey  = 0;
    static u16 CountWater  = 0;
	static u16 min_counter = 600;
    
	static u08 sec_counter = 10;
	
	static u08 indicator_data_buf[8];

	u16 DayMinutesCounter = MINUTES_IN_DAY;

    u16 interval_for_send = 0;

	u16 TimeSendRequestCnt = 0;

	u08 after_reset = 1;

	static u16 modem_error_timer = 60*60*1000 / 100;

	for( ;; )
    {
 
 	    if (CountPulse == 0) {
   		    CountManey = 0;
	    }
	    else {
			CountManey = PulseQuantityToMoney(CountPulse);
	    } 

	    if (CountManey == 0) {
   		    CountWater = 0;
	    }
	    else {
		    CountWater = MoneyToWater(CountManey);
	    }

		if (!CountManey && gFlLedStateWater) {
		    /* symbol "-" write instead all digits to indicator */
            memset(indicator_data_buf, 10, sizeof indicator_data_buf / sizeof(indicator_data_buf[0]));
			IndicatorDataWrite(indicator_data_buf);
        }
        else {
   		    /* number money and water sow ea indicator */
            IndicatorDataConvert(indicator_data_buf, CountManey, CountWater);
			IndicatorDataWrite(indicator_data_buf);
        }


        /*Change price of water, NewPrice get from back messages from server to mgsm modem*/
 		if (NewPrice) {
		    portENTER_CRITICAL();
            EEPR_LOCAL_COPY.cost_litre_coef = NewPrice;
			NewPrice = 0;
			portEXIT_CRITICAL();
			
			xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    	    IntEeprWordWrite (CostLitreCoefEEPROMAdr, EEPR_LOCAL_COPY.cost_litre_coef);
            xSemaphoreGive(xI2CMutex);
		}
		
        if (WtrCntTimer > 0) {
		    portENTER_CRITICAL();
            WtrCntTimer--;
			portEXIT_CRITICAL();
		}

		if (AccellCntTimer > 0) {
		    portENTER_CRITICAL();
	        AccellCntTimer--;
			portEXIT_CRITICAL();
        }

        if (min_counter == 0) {
		    min_counter = 600;

			if (DayMinutesCounter >= MINUTES_IN_DAY) {

				xSemaphoreTake(xI2CMutex, portMAX_DELAY);
                if (TimeAndDateRtcRead(&TimeAndDate_System) != 0) {                     /* if date and time don't read or correct it set to default */
                    TimeAndDateDefaultSet(&TimeAndDate_System);
                } 
				xSemaphoreGive(xI2CMutex); 

    			DayMinutesCounter = GetTimeAsMinute(&TimeAndDate_System);

                if (Fl_Send_TimeDateCurGet == 0) {
                    Fl_Send_TimeDateCurGet = 1;
				}
			}

            DayMinutesCounter++;

			/*Send report*/
			if (interval_for_send == 0) {
		        interval_for_send = EEPR_LOCAL_COPY.report_interval;
				if (interval_for_send == 0) {
			        interval_for_send = 1;
				}
				else {
				    if (after_reset) {
					    after_reset = 0;
						SYSTEM_EVENTS = Fl_Ev_JustAfterReset;
						xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
					}

				    if (uxQueueMessagesWaiting(xEventsQueue) == 0) { 
				        SYSTEM_EVENTS = Fl_Ev_RequestData;
                        xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);

						modem_error_timer = 60*60*1000 / 100;
					} else {
					    if (modem_error_timer == 0) {
						    modem_error_timer = 60*60*1000 / 100;
							CARRENT_STATE = STATE_MODEM_OFF;
						} else {
						    --modem_error_timer;
						}
					}
				}
		    }
			interval_for_send--;
		}
		else {
		    min_counter--;
		}


        if (sec_counter == 0) {
		    sec_counter = 10;
	        TimeAndDateSecAdd(&TimeAndDate_System);
		}
		else {
		    sec_counter--;
		}

		TimeSendRequestCnt++;
		if (TimeSendRequestCnt == TIME_SEND_REGUEST) {
		    TimeSendRequestCnt = 0;
			xSemaphoreGive(xTimeSendRequestSem);
		}


        if (Tmr_For_Init_Rr > 0) {
	        Tmr_For_Init_Rr--;
		}


		vTaskDelay(100 / portTICK_RATE_MS);

#if CHECK_STACK
    DebugBuff[0] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK
    }

    vTaskDelete(NULL);
}


void vTask3( void *pvParameters )
{
    u08 rx_data_buff[MAX_RX_SIZE_BUFF];

    xSemaphoreTake(xUart_RX_Semaphore, 0);
	

	for( ;; )
    {

		if (xSemaphoreTake(xUart_RX_Semaphore, 0) == pdTRUE) { 
		
            memset(&rx_data_buff[0], 0x0, MAX_RX_SIZE_BUFF); 
            memcpy(&rx_data_buff[0], (char *)&BUF_UART_RX[0], MAX_RX_SIZE_BUFF);
		    memset((char *)&BUF_UART_RX[0], 0x0, MAX_RX_SIZE_BUFF);
        
		    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
	        GetCmd(&rx_data_buff[0]);
            xSemaphoreGive(xI2CMutex); 
		}
		else if (!IS_SERVICE_MODE && IsRegistratorConnect) {
           
		    RegistratorProcessing(50);
		}


        vTaskDelay(50 / portTICK_RATE_MS);   

#if CHECK_STACK
    DebugBuff[1] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

	}

    vTaskDelete(NULL);
}


void vTask4( void *pvParameters )
{
    static  u16 PumpTimeCoef ;
//	u16 get_key_skan = 0;
 
    static  TimeAndDate Time_And_Date_Bcd = {0};

    static  struct {
        u08 SellEnable    : 1;
        u08 SellStart     : 1;
        u08 SellStop	  : 1;
        u08 MergeEnable   : 1;
        u08 ErrPower	  : 1;
        u08 ErrWater	  : 1;
        u08 SeifOpened    : 1;
        u08 ErrMinWater   : 1;
	    u08 ErrRsvBill    : 1;
	    u08 WtrCntrErr    : 1;
//	    u08 ServiceOpened : 1;
		u08 MoneyGet      : 1;
		u08 AppBlock      : 1;
	} Fl;

	
	static  struct {
	    u08 NoWater    : 1;
        u08 NoWrkBill  : 1;
        u08 NoPower1   : 1;
        u08 NoPower2   : 1;
        u08 Start      : 1;
        u08 Stop       : 1;
        u08 Reset      : 1;
        u08 DoorOpn    : 1;
        u08 BillGet    : 1;
        u08 CoinGet    : 1;
	    u08 ServiceKey : 1;
	} Sygnal_Get;


	static  u08 Is_Registrator_Err_Gprs_Send = 0;
	
/*			
    const u08 Fl_Ev_NoWater              =  1;          
    const u08 Fl_Ev_NoPower              =  2;
    const u08 Fl_Ev_TakeManey            =  3;
    const u08 Fl_Ev_GetMoving            =  4;
    const u08 Fl_Ev_LimWater             =  5;
//  const u08 Fl_Ev_RequestData = 6;
    const u08 Fl_Ev_ErrorBill            =  7;
	const u08 Fl_Ev_RegError             =  8;
	const u08 Fl_Ev_ServiceKeyPresent    =  9;
	const u08 Fl_Ev_ServiceKeyNotPresent = 10;
*/



    

    static  u16 ManeySave = 0;
    static  u16 WaterSave = 0;
	static  u16 CountRManey = 0;

    static  u16 tmp_cnt_pulse = 0;
    
    PumpTimeCoef = EEPR_LOCAL_COPY.pump_off_time_coef;

	
	static  u08 Fl_RegistratorErr = 1;
	static  u08 registrator_connect_prev;
	static  u08 Fl_Send_Sell_End = 0;
	static  u08 Fl_Get_New_Data = 0;
	static  u08 Fl_Send_Exclude_Sum_Read  = 0;
	static  u08 Fl_Send_Exclude_Sum_Clear = 0;


	static  u08 is_service_mode;

	static  u16 coin_which_get_cntr;
	static  u16 bill_which_get_cntr;

	static  u08 is_service_key_present;
	
	typedef  enum {
	    IDLE_STATE,
		WAIT_INIT,
		SEND_SELL_START,
		SEND_SELL_END,
		SEND_SELL_CANCEL,
		SEND_TIME_DATE_GET, 
		SEND_MODEM_STATUS_CHECK, 
		SEND_STORED_SUM_READ,
		SEND_STORED_SUM_CLEAR,
		REGISTRATOR_ANSVER_GET,
		REGISTRATOR_ANSVER_WAIT,
		SERVICE_MODE
	} REGISTRATOR_STATE_COMMUNICATE;

	REGISTRATOR_STATE_COMMUNICATE  registrator_state;
	REGISTRATOR_STATE_COMMUNICATE  registrator_ansver_to;
 
	enum event_to_ext_eeprom {
	    EV_SAVE_SELL_START      = 1,
		EV_SAVE_CASH_COLLECTION = 3,
		EV_SAVE_NO_POWER        = 4
	};

	static  RegistratorReceivedData err_data;
	static  RegistratorReceivedData request_data;
	
	static  RegistratorMsg CUWB_RegistratorMsg;

	static  s32 registrator_sum;

    registrator_state = WAIT_INIT;
	registrator_ansver_to = IDLE_STATE;

    
    xSemaphoreTake(xTimeSendRequestSem, 0);
	xSemaphoreTake(xExtSignalStatusSem, 0);

    IsRegistratorConnect = 0;
    
    if (RegistratorSaveWater > 0) {     /* The RegistratorSaveWater takes it value by read internal eeprom while initialization of the microcontroller */
	    Fl_Send_Sell_End = 1;
	}

    if (RegistratorCashClear > 0) {     /* The RegistratorSaveWater takes it value by read internal eeprom while initialization of the microcontroller */
	    Fl_Send_Exclude_Sum_Clear = 1;
	}


	ShouldReturnToBuyer.Money = 0;
	ShouldReturnToBuyer.Water = 0;

	is_service_mode = ((IS_SERVICE_MODE) ? 1 : 0);

	AccellCntTimer = ACCELEROMETER_PERIOD;
	Sygnal_Get_Accellerometer = 0;

    wdt_enable(WDTO_2S);

	for( ;; )
    {

    wdt_reset();

#if (CHECK_STACK == 2)
    DebugBuff[2] = uxTaskGetStackHighWaterMark(NULL);
#endif  /* CHECK_STACK */
		
		
    switch (registrator_state) {

	    case WAIT_INIT: {
		     if (xSemaphoreTake(xTimeSendRequestSem, 0) == pdTRUE) {
                 
				 if (IsRegistratorConnect && !is_service_mode) {

 		             Uart0Enable(RegistratorCharPut, 9600);

					 registrator_state = SEND_SELL_START;
                 }
		         else {

		             Uart0Enable(Uart0_Resiv,  19200);

					 registrator_state = (is_service_mode) ? SERVICE_MODE: IDLE_STATE;
	             }
                 registrator_connect_prev = IsRegistratorConnect;
  	         }

			 break;
		}
	    case SERVICE_MODE: {

			 break;
		}
	    case IDLE_STATE: {

             if (IsRegistratorConnect != registrator_connect_prev) {
			     registrator_connect_prev = IsRegistratorConnect;
	             registrator_state = WAIT_INIT;
  	         }    
	         else if (IsRegistratorConnect && !Fl.MoneyGet && !Fl.SellStart) { 

			     if (Fl_Send_Sell_End == 1) {
				     registrator_state = SEND_SELL_END;
				     
					 if (registrator_ansver_to != SEND_SELL_START) {
				         registrator_state = SEND_SELL_START;
					 }
				 }
				 else if (Fl_Send_Exclude_Sum_Clear == 1) {
				     
					 if (registrator_ansver_to == SEND_SELL_START) {
				         registrator_state = SEND_SELL_CANCEL;    
					 } else if ((registrator_ansver_to == SEND_STORED_SUM_READ && Fl_Send_Exclude_Sum_Read == 0) || 
					             registrator_ansver_to == SEND_STORED_SUM_CLEAR) {

					     registrator_state = SEND_STORED_SUM_CLEAR;
					 } else {
                         registrator_state = SEND_STORED_SUM_READ;
					     Fl_Send_Exclude_Sum_Read = 1;
					 }
				 }
				 else if (Fl_Send_TimeDateCurGet == 1) {
				     registrator_state = SEND_TIME_DATE_GET;

					 if (registrator_ansver_to == SEND_SELL_START) {
				         registrator_state = SEND_SELL_CANCEL;    
					 }
				 }
				 else if (Fl_Send_HourBeforeBlock == 1) {
				     registrator_state = SEND_MODEM_STATUS_CHECK;
				     
					 if (registrator_ansver_to == SEND_SELL_START) {
				         registrator_state = SEND_SELL_CANCEL;    
					 }
				 }
	             else  if (xSemaphoreTake(xTimeSendRequestSem, 0) == pdTRUE) {
				     registrator_state = SEND_SELL_START;
				     
					 if (registrator_ansver_to == SEND_SELL_START && Fl_RegistratorErr == 0) {
				         registrator_state = SEND_SELL_CANCEL;
					 }
				 }
			 }
		     break;
		}
		case SEND_SELL_START: {
		     if ( RegistratorDataSet(RCMD_SELL_START, NULL) ) {
				 registrator_ansver_to = SEND_SELL_START;
				 registrator_state = REGISTRATOR_ANSVER_WAIT;
			 }
			 break;
		}
		case SEND_SELL_END: {
		     
		     CUWB_RegistratorMsg.Data.ProductInfo.Number = 0;
             CUWB_RegistratorMsg.Data.ProductInfo.Quantity = RegistratorSaveWater * 10;
             CUWB_RegistratorMsg.Data.ProductInfo.Price = EEPR_LOCAL_COPY.cost_litre_coef;
             
		     if ( RegistratorDataSet(RCMD_SELL_END, &CUWB_RegistratorMsg) ) {
			     registrator_ansver_to = SEND_SELL_END;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
	    	 }
			 break;
		}
		case SEND_SELL_CANCEL: {
			 
#if (CHECK_STACK == 1)
			     //unsigned portBASE_TYPE uxTaskGetStackHighWaterMark( xTaskHandle xTask );
                 static u08 i = 0;
                 DebugBuff[2] = uxTaskGetStackHighWaterMark(NULL);
                 CUWB_RegistratorMsg.Data.OperationNum.Operation = ((i+1) * 1000 + DebugBuff[i++]);
             
                 if(i >= TASK_NUMBER) {
                     i = 0;
				 }
#else 
             CUWB_RegistratorMsg.Data.OperationNum.Operation = ROPERATION_CANCEL_SELL;
#endif /* CHECK_STACK */


             if ( RegistratorDataSet(RCMD_SELL_CANCELL, &CUWB_RegistratorMsg) ) {
                 registrator_ansver_to = SEND_SELL_CANCEL;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
     		 }
			 break;
		}
        case SEND_TIME_DATE_GET: {
		 
			 if ( RegistratorDataSet(RCMD_DATA_TIME_GET, NULL) ) {
			     registrator_ansver_to = SEND_TIME_DATE_GET;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
	    	 }
			 break;
		}
		case SEND_MODEM_STATUS_CHECK: {

			 CUWB_RegistratorMsg.Data.ReportPrint.IsPrint = 0;          /* 0 - not print a report, 1 - print a report */

			 if ( RegistratorDataSet(RCMD_MODEM_STATUS, &CUWB_RegistratorMsg) ) {
			     registrator_ansver_to = SEND_MODEM_STATUS_CHECK;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
	    	 }
			 break;
		}
        case SEND_STORED_SUM_READ: {
			 
			 CUWB_RegistratorMsg.Data.AddExcludeSum.OperatorNumber = ROPERATOR_NUMBER_DEFAULT;
			 CUWB_RegistratorMsg.Data.AddExcludeSum.Sum = 0;                                       /* If need to know current sum this parameter should be set to 0 */


             if ( RegistratorDataSet(RCMD_ADD_EXCLUDE_SUM, &CUWB_RegistratorMsg) ) {
			     registrator_ansver_to = SEND_STORED_SUM_READ;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
	    	 }
			 break;
		}
        case SEND_STORED_SUM_CLEAR: {
			 
			 CUWB_RegistratorMsg.Data.AddExcludeSum.OperatorNumber = ROPERATOR_NUMBER_DEFAULT;
			 CUWB_RegistratorMsg.Data.AddExcludeSum.Sum = registrator_sum;                                       /* for to clear the sum this parameter must be less then 0 */
             CUWB_RegistratorMsg.Data.AddExcludeSum.Sum *= (-1);

             if ( RegistratorDataSet(RCMD_ADD_EXCLUDE_SUM, &CUWB_RegistratorMsg) ) {
			     registrator_ansver_to = SEND_STORED_SUM_CLEAR;
                 registrator_state = REGISTRATOR_ANSVER_WAIT;
	    	 }
			 break;
		}
		case REGISTRATOR_ANSVER_WAIT: {

			 if (RegistratorStatusGet() == RR_CONNECTION_OK) {
			     registrator_state = REGISTRATOR_ANSVER_GET;
             }
			 else if (RegistratorStatusGet() == RR_CONNECTION_ERROR) {
			     Fl_RegistratorErr = 1;
             }
			 break;
		} 
    	case REGISTRATOR_ANSVER_GET: {
		     switch (registrator_ansver_to) {
			     case SEND_SELL_START: {
                      RegistratorDataGet(&err_data, ERROR_CODE);

					  switch ( RegistratorErrorCode(&err_data) ) {
					  
					      case RR_ERR_NO: {
					           Fl_RegistratorErr = 0;
						       registrator_state = IDLE_STATE;
							   //registrator_state = SEND_SELL_CANCEL;
							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_CANCEL;
							   break;
                          }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;
							   break;
					      }
					  }
					  break;
				 }
				 case SEND_SELL_END: {
                      RegistratorDataGet(&err_data, ERROR_CODE);

					  switch ( RegistratorErrorCode(&err_data) ) {
					      case RR_ERR_NO: {
					           Fl_Send_Sell_End = 0;
					           Fl_RegistratorErr = 0;

						       RegistratorSaveWater = 0;
					           if (IntEeprDwordRead(RegistratorWaterEEPROMAdr) != 0) {  
							       xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    	      	                   IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
                                   xSemaphoreGive(xI2CMutex);
							   }

					           registrator_state = IDLE_STATE;

							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_START;

							   break;
					      }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;

							   break;
					      }
				     }
					 break;
				 }
				 case SEND_SELL_CANCEL: {
			          Fl_RegistratorErr = 0;
                      registrator_state = IDLE_STATE;
                      break;
				 }
				 case SEND_TIME_DATE_GET: {
                      switch ( RegistratorErrorCode(&err_data) ) {
					      case RR_ERR_NO: {
						       Fl_RegistratorErr = 0;
							   RegistratorDataGet(&request_data, DATA);

                               if (request_data.len >= 10) {

					               xSemaphoreTake(xI2CMutex, portMAX_DELAY);                               
							       TimeAndDayFromRegStr(&TimeAndDate_System, (u08 *)request_data.dataptr);
							       TimeAndDayToBcd(&Time_And_Date_Bcd, TimeAndDate_System);
							       TimeAndDateRtcWrite(&Time_And_Date_Bcd);
                                   xSemaphoreGive(xI2CMutex);

                                   xSemaphoreTake(xI2CMutex, portMAX_DELAY);
							       TimeAndDateRtcRead(&TimeAndDate_System);
                                   xSemaphoreGive(xI2CMutex);

								   Fl_Send_TimeDateCurGet = 0;
							   }

					           registrator_state = IDLE_STATE;
							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_CANCEL;
							   break;
					      }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;
							   break;
					      }
				     }
					 break;
                 }
				 case SEND_MODEM_STATUS_CHECK: {
				      RegistratorDataGet(&err_data, ERROR_CODE);

					  switch ( RegistratorErrorCode(&err_data) ) {
					      case RR_ERR_NO: {
						       Fl_RegistratorErr = 0;
							   RegistratorDataGet(&request_data, DATA);

                               if (request_data.len > 10) {
                                   TimeAndDate when_not_transmited;

                                   TimeAndDayFromRegStr(&when_not_transmited, (u08 *)(strstr((const char *)request_data.dataptr, ";") + 1));

								   xSemaphoreTake(xI2CMutex, portMAX_DELAY); 
                                   u16 h;
								   if ((h = HoursToBlocking(&TimeAndDate_System, &when_not_transmited)) > 72) {
								       h = 72;
								   }
								   xSemaphoreGive(xI2CMutex);

                                   Hours_BeforeApparatBlock = h;
							       if (!Fl.AppBlock && Hours_BeforeApparatBlock > (72 - (72 - 12))) {
								       Fl.AppBlock = 1;
							           SYSTEM_EVENTS = Fl_Ev_WillBlocked;
				                       xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
							       }
								   else {
								       Fl.AppBlock = 0;
								   }
                                   

								   Fl_Send_HourBeforeBlock = 0;
                               }

					           registrator_state = IDLE_STATE;
							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_CANCEL;
							   break;
					      }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;
							   break;
					      }
				     }
					 break;
				 }
				 case SEND_STORED_SUM_READ: {
                      RegistratorDataGet(&err_data, ERROR_CODE);

					  switch ( RegistratorErrorCode(&err_data) ) {
					      case RR_ERR_NO: {
					           Fl_RegistratorErr = 0;
							   RegistratorDataGet(&request_data, DATA);

							   if (request_data.len >= 4) {
							       char *pos;

								   pos = strstr((const char *)request_data.dataptr, ".");
								   if (pos) {
								       *pos = 0;
									   registrator_sum = atol((const char *)request_data.dataptr);
                                       registrator_sum = registrator_sum * 100 + atol((const char *)(pos + 1));
								   
						               Fl_Send_Exclude_Sum_Read = 0;
								   }
							   }

							   registrator_state = IDLE_STATE;
							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_CANCEL;
							   break;
					      }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;
							   break;
					      }
				     }
					 break;
			     }
                 case SEND_STORED_SUM_CLEAR: {
                      RegistratorDataGet(&err_data, ERROR_CODE);

					  switch ( RegistratorErrorCode(&err_data) ) {
					      case RR_ERR_NO: {
					           Fl_RegistratorErr = 0;
							   RegistratorDataGet(&request_data, DATA);

							   if (request_data.len >= 4) {

						           RegistratorCashClear = 0;
							       xSemaphoreTake(xI2CMutex, portMAX_DELAY);
					               if (IntEeprDwordRead(RegistratorCashEEPROMAdr) != 0) {  
    	      	                       IntEeprDwordWrite(RegistratorCashEEPROMAdr, RegistratorCashClear);
							       }
                                   xSemaphoreGive(xI2CMutex);

                                   registrator_sum = 0;
					               Fl_Send_Exclude_Sum_Clear = 0;
							   }

							   registrator_state = IDLE_STATE;
							   break;
					      }
					      case RR_ERR_STATE_NOT_RIGHT: {
					           Fl_RegistratorErr = 0;
						       registrator_state = SEND_SELL_CANCEL;
							   break;
					      }
					      default: {
					           Fl_RegistratorErr = 1;
				    	       registrator_state = IDLE_STATE;
							   break;
					      }
				     }
					 break;
			     }
				 default:
				 break;
			 }

//			 registrator_ansver_to = 0;
			 break;
		}
//		default : {
//		     break;
//		}
	}

///////////////////////////////////////////////////////////////////////////////////////
      
        Fl.SellEnable = 1;	     

     
		if (xSemaphoreTake(xExtSignalStatusSem, 0) == pdTRUE) {  

            Sygnal_Get.CoinGet   = ExtSignalStatus & 1;
			Sygnal_Get.BillGet   = ((ExtSignalStatus >> 1) & 1);
			Sygnal_Get.NoWater   = ((ExtSignalStatus >> 2) & 1);
            Sygnal_Get.NoPower1  = ((ExtSignalStatus >> 3) & 1);
			Sygnal_Get.NoPower2  = ((ExtSignalStatus >> 4) & 1);
			Sygnal_Get.DoorOpn   = ((ExtSignalStatus >> 5) & 1);
		    Sygnal_Get.Start     = ((ExtSignalStatus >> 6) & 1);
		    Sygnal_Get.Stop      = ((ExtSignalStatus >> 7) & 1);
		    Sygnal_Get.Reset     = ((ExtSignalStatus >> 8) & 1);
		    Sygnal_Get.NoWrkBill = ((ExtSignalStatus >> 9) & 1);

			IsRegistratorConnect = ((ExtSignalStatus >> 10) & 1);

			Sygnal_Get.ServiceKey = ((ExtSignalStatus >> 11) & 1);
		}

		if (is_service_mode) {
		    Sygnal_Get.ServiceKey = 0;
		}

		if (Sygnal_Get.NoPower1 || Sygnal_Get.NoPower2) {

		    Fl.SellEnable = 0;
		    Fl_State.Power = REPORT_FLAG_ERR;            
          
            if (!Fl.ErrPower) {
		        Fl.ErrPower = 1;
			    //Fl_Ev_NoPower = 1;
	
//                if (!is_service_mode) {

                    if (Fl.SellStart || Fl.SellStop || Fl.MoneyGet || Fl.WtrCntrErr) {

                        if (CountPulse > 0) {
						    ShouldReturnToBuyer.Money = PulseQuantityToMoney(CountPulse);
				            ShouldReturnToBuyer.Water = MoneyToWater(ShouldReturnToBuyer.Money);

							xSemaphoreTake(xI2CMutex, portMAX_DELAY);
					        TimeAndDayToBcd(&Time_And_Date_Bcd, TimeAndDate_System);

                            SaveEvent((u08 *)&Time_And_Date_Bcd, ShouldReturnToBuyer.Money, ShouldReturnToBuyer.Water, 0, 0, EV_SAVE_NO_POWER);             /* save data to external eeprom */ 
			                xSemaphoreGive(xI2CMutex);

							                                                       /* all flags sets that same method as the end of sell */
							portENTER_CRITICAL();
                            CountPulse = 0; 
		                    portEXIT_CRITICAL();
                        }
						else if (IsDataToReturnSent == 1) {
                            IsDataToReturnSent = 0;						    

						    ShouldReturnToBuyer.Money = 0;
						    ShouldReturnToBuyer.Water = 0;
						}


						if (Fl.MoneyGet && ManeySave > 0) {
					
			                MoneyCounterToSave.Sum += ManeySave;

				            WaterSave = MoneyToWater(ManeySave);

				            RegistratorSaveWater += WaterSave;                                     /* emount of water to transmit to registrator */

                            if (EEPR_LOCAL_COPY.amount_water <= WaterSave) {                                      /* how many water left in the barrel */
			                    EEPR_LOCAL_COPY.amount_water = 0;
			                }
			                else {
		                        EEPR_LOCAL_COPY.amount_water -= WaterSave;
			                }


							xSemaphoreTake(xI2CMutex, portMAX_DELAY);

			                IntEeprBlockWrite((u16)&MoneyCounterToSave, MoneyCounterEEPROMAdr, sizeof(MoneyCounterToSave));

			                IntEeprDwordWrite(AmountWaterEEPROMAdr, EEPR_LOCAL_COPY.amount_water);

                            if (!is_service_mode) {
                                IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
								Fl_Get_New_Data = 1;
                            }					        

                            xSemaphoreGive(xI2CMutex);
                            
							ManeySave = WaterSave = 0;

							Fl.MoneyGet = 0;
		                }
		            }
//				}
				
				/* xQueueSend(xEventsQueue, &Fl_Ev_NoPower, 0); */
				SYSTEM_EVENTS = Fl_Ev_NoPower;
				xQueueSendToFront(xEventsQueue, &SYSTEM_EVENTS, 0);
            }
	    }
        else {
	        Fl_State.Power = REPORT_FLAG_OK;
	        Fl.ErrPower = 0;
	    }
			
    	if (Sygnal_Get.NoWater && (!Fl.SellStop)) {
 
    		Fl.SellEnable = 0;                                        
            Fl_State.Water = REPORT_FLAG_ERR;

            if (!Fl.ErrWater) {
			    gFlLedStateWater = 1;
                Fl.ErrWater = 1;
		        //Fl_Ev_NoWater = 1;
				SYSTEM_EVENTS = Fl_Ev_NoWater;
				xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
            }
	    }
        else {
		    gFlLedStateWater = 0;
	        Fl.ErrWater = 0;
		    Fl_State.Water = REPORT_FLAG_OK;
	    }

		 /* if water counter don't count, we can't sell the water */ 
		if (Fl.WtrCntrErr) {
		    Fl.SellEnable = 0;
		}


        if ((Fl_RegistratorErr || !IsRegistratorConnect) && !is_service_mode) {
		    if (!Is_Registrator_Err_Gprs_Send && (Tmr_For_Init_Rr == 0)) {
			    Fl_State.RrState = REPORT_FLAG_ERR;
			    SYSTEM_EVENTS = Fl_Ev_RegError;
		        xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
			    Is_Registrator_Err_Gprs_Send = 1;
			}
		}
		else if (Is_Registrator_Err_Gprs_Send) {
		    Fl_State.RrState = REPORT_FLAG_OK;
			Is_Registrator_Err_Gprs_Send = 0;
		}


		if (!is_service_mode && Fl_Send_Sell_End && !Fl_RegistratorErr && !Fl.SellStop) {  
		    Fl.SellEnable = 0;
		}


		if ((CountRManey >= 1000) || (!Fl.SellEnable) 
	                              || Fl.SeifOpened 
				    			  || Sygnal_Get.NoWater
								  || Fl.WtrCntrErr) {
		    StopGetManey();
	    }
		else if (!is_service_mode && (Fl_RegistratorErr || (Fl.SellStop && Fl_Send_Sell_End)
								                       || !IsRegistratorConnect)) {
		    StopGetManey();
		}
	    else {
		    if (!Fl.SellStart) {
			    StartGetManey();
		    }
	        else {
		        Fl.SellEnable = 0;
		    }
	    }

	
	    if ((Sygnal_Get.CoinGet || Sygnal_Get.BillGet) && !Fl.SellStart) {
            		
            Fl.MoneyGet = 1;

			if (Sygnal_Get.CoinGet) {
			    Sygnal_Get.CoinGet = 0;
	    	    CountRManey += 25;
		        ManeySave += 25;

				coin_which_get_cntr += 25;
            }
			else {
    			Sygnal_Get.BillGet = 0;
		        CountRManey += 100;
		        ManeySave += 100;
				
				bill_which_get_cntr++;
			}

			portENTER_CRITICAL();
            CountPulse = MoneyToPulse(CountRManey);
		    portEXIT_CRITICAL();
	    }
	

	    if (!Fl.MoneyGet) {
            CountRManey = PulseQuantityToMoney(CountPulse);
		}
		

	    if (Sygnal_Get.Start && !Sygnal_Get.Stop 
		                     && Fl.SellEnable 
							 && !Fl.SellStart
							 && CountPulse) {
         
		    StopGetManey();
		    Fl.MoneyGet = 0;
			
            if (ManeySave > 0) {

			    MoneyCounterToSave.Sum += ManeySave;

				MoneyCounterToSave.Coin += coin_which_get_cntr;
				MoneyCounterToSave.Bill += bill_which_get_cntr;
				
				WaterSave = MoneyToWater(ManeySave);
				RegistratorSaveWater += WaterSave;                                      /* set data to transmit to registrator */

			    if (EEPR_LOCAL_COPY.amount_water <= WaterSave) {
			        EEPR_LOCAL_COPY.amount_water = 0;
			    }
			    else {
		            EEPR_LOCAL_COPY.amount_water -= WaterSave;
			    }

                xSemaphoreTake(xI2CMutex, portMAX_DELAY);  
				TimeAndDayToBcd(&Time_And_Date_Bcd, TimeAndDate_System);

		        SaveEvent((u08 *)&Time_And_Date_Bcd, ManeySave, WaterSave, bill_which_get_cntr, coin_which_get_cntr, EV_SAVE_SELL_START);
			    
				IntEeprBlockWrite((u16)&MoneyCounterToSave, MoneyCounterEEPROMAdr, sizeof(MoneyCounterToSave));

			    IntEeprDwordWrite(AmountWaterEEPROMAdr, EEPR_LOCAL_COPY.amount_water);

                if (!is_service_mode) {
				    IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
					Fl_Get_New_Data = 1;
				}

                xSemaphoreGive(xI2CMutex);
                
				coin_which_get_cntr = 0;
				bill_which_get_cntr = 0;

				ManeySave = WaterSave = 0;
		    }

            if (PumpTimeCoef <= (EEPR_LOCAL_COPY.pump_on_time_coef)) {
                PumpTimeCoef = 0;
            }
		    else {
		        PumpTimeCoef = (PumpTimeCoef - (EEPR_LOCAL_COPY.pump_on_time_coef));
		    }
				
            portENTER_CRITICAL();
        	WtrCntTimer = CHECK_COUNTER_PERIOD;  /* Starting the timer for check does the counter of water connected or worck */    
            portEXIT_CRITICAL();
            tmp_cnt_pulse = CountPulse;          

		    SellingStart();
		    Fl.SellStop = 0;
		    Fl.SellStart = 1;
	    }

	    
        if (Sygnal_Get.Stop && (!(Sygnal_Get.Start) || Fl.SellStart)) {
	        
			SellingStop();

		    Fl.SellStart = 0;
		    Fl.SellStop  = 1;
	    }


	    if ((CountPulse <= PumpTimeCoef) && (Fl.SellStart || Fl.SellStop)) {

		    SellingStop();

		    Fl.SellStart = 0; 
		    Fl.SellStop = 0; 
            CountRManey = 0;	

            portENTER_CRITICAL();
            CountPulse = 0;
		    portEXIT_CRITICAL();

		    PumpTimeCoef = EEPR_LOCAL_COPY.pump_off_time_coef;
    	}


        if (Sygnal_Get.ServiceKey) {
		    if (is_service_key_present == 0) {
			    is_service_key_present = 1; 

				SellingStop();

				SYSTEM_EVENTS = Fl_Ev_ServiceModeActivate;
                xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
			}
		}
		else if (is_service_key_present) {
		    is_service_key_present = 0;

			CountRManey = 0;
		    ManeySave = 0;

			coin_which_get_cntr = 0;
			bill_which_get_cntr = 0;

            portENTER_CRITICAL();
            CountPulse = 0;
		    portEXIT_CRITICAL();

            SYSTEM_EVENTS = Fl_Ev_ServiceModeDeactivate;
			xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
	    }


        if (Sygnal_Get.Reset) {
       
			if (!Fl.SeifOpened && Sygnal_Get.ServiceKey) {
		        SYSTEM_EVENTS = Fl_Ev_ServiceOpening;
			    xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);

		    } else if (!Fl.SeifOpened && !Sygnal_Get.ServiceKey) { 
			    ENCASHMENT_T encashment_data;
				u16 dattaH;
		        u16 dattaL;
             
			    StopGetManey();

				if (MoneyCounterToSave.Sum != 0) {

                    dattaH = (u16)((MoneyCounterToSave.Sum) >> 16);
		            dattaL = (u16)((MoneyCounterToSave.Sum) & 0x0000FFFF);

				    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
				    TimeAndDayToBcd(&Time_And_Date_Bcd, TimeAndDate_System);
                    SaveEvent((u08 *)&Time_And_Date_Bcd, dattaH, dattaL, MoneyCounterToSave.Bill, MoneyCounterToSave.Coin, EV_SAVE_CASH_COLLECTION);
				    memcpy((u08 *)&encashment_data.DateTime, (const u08 *) &TimeAndDate_System.Minute, sizeof(encashment_data.DateTime));
                    xSemaphoreGive(xI2CMutex);

				
		    		encashment_data.Money.Sum = MoneyCounterToSave.Sum;
	    			encashment_data.Money.Bill = MoneyCounterToSave.Bill;
    				encashment_data.Money.Coin = MoneyCounterToSave.Coin;

                    RegistratorCashClear = MoneyCounterToSave.Sum;
   
				    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
					QueueEncashmentPut(&encashment_data);
					MoneyCounterToSave.Sum = 0;
					MoneyCounterToSave.Bill = 0;
					MoneyCounterToSave.Coin = 0;
					IntEeprBlockWrite((u16)&MoneyCounterToSave, MoneyCounterEEPROMAdr, sizeof(MoneyCounterToSave));

                    if (!is_service_mode) {  
				        IntEeprDwordWrite(RegistratorCashEEPROMAdr, RegistratorCashClear);
					    Fl_Send_Exclude_Sum_Clear = 1;
					}

			        xSemaphoreGive(xI2CMutex);

					SYSTEM_EVENTS = Fl_Ev_TakeManey;
				    xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
				}
				
#if 1
                if (EEPR_LOCAL_COPY.amount_water != EEPR_LOCAL_COPY.max_size_barrel) {
			    	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
                    IntEeprDwordWrite(AmountWaterEEPROMAdr, EEPR_LOCAL_COPY.max_size_barrel);
					xSemaphoreGive(xI2CMutex);
                    EEPR_LOCAL_COPY.amount_water = EEPR_LOCAL_COPY.max_size_barrel;
				}
#endif
                
                                 				
				BUZZER_ON;
				xTimerChangePeriod(xTimer_BuzzerOff, (200 / portTICK_RATE_MS), 0);
				if (xTimerIsTimerActive(xTimer_BuzzerOff) == pdFALSE) {
			        xTimerStart(xTimer_BuzzerOff, 0);
			    }

#if 0
                if (EEPR_LOCAL_COPY.amount_water < EEPR_LOCAL_COPY.max_size_barrel) {                        /* Is the signal of buzzer to make remember to push the key "drain of water" */
    			    buzer_flag = 1;                                            /* which need after the  */
				}
#endif
		    }

			Fl.SeifOpened = 1;
        }
        else {
			Fl.SeifOpened = 0;
	    }

	    if (Sygnal_Get.DoorOpn && !Fl.MergeEnable) {
		    if (!Fl.SellStart && !Fl.SellStop && !(Sygnal_Get.Reset) && !Fl.WtrCntrErr) {
	
	    		Fl.MergeEnable = 1;
	            SellingStart();
	        }
        }

#if 0
	    if (Sygnal_Get.DoorOpn && !Fl.MergeEnable && Sygnal_Get.Reset && !Fl.WtrCntrErr) {

		   	Fl.MergeEnable = 1;
            xSemaphoreTake(xI2CMutex, portMAX_DELAY);
 		    IntEeprDwordWrite (AmountWaterEEPROMAdr, *max_size_barrel);
            xSemaphoreGive(xI2CMutex);		
		    *amount_water = *max_size_barrel;

		    BUZZER_ON;
			xTimerChangePeriod(xTimer_BuzzerOff, (200 / portTICK_RATE_MS), 0);
			if (xTimerIsTimerActive(xTimer_BuzzerOff) == pdFALSE) {
			    xTimerStart(xTimer_BuzzerOff, 0);
			}

            buzer_flag = 0;
	    }
#endif

	    if (!Sygnal_Get.DoorOpn && Fl.MergeEnable) {
		    SellingStop();
			Fl.MergeEnable = 0;
	    }

#if 0
		if (buzer_flag && Sygnal_Get.Stop) {
   	        buzer_flag = 0;
		}
#endif

        if (Fl.SellStart && !IS_COUNTER_WATER_NOT_ACTIVE && WtrCntTimer == 0) {
		
		    if (tmp_cnt_pulse == CountPulse) {
      		   	SellingStop();
		        Fl.SellStart = 0; 
			    Fl.WtrCntrErr = 1;
			}
			else {
			    tmp_cnt_pulse = CountPulse;
			    //portENTER_CRITICAL();
        	    WtrCntTimer = CHECK_COUNTER_PERIOD;  /* Starting the timer for check does the counter of water connected or worck */    
                //portEXIT_CRITICAL();
			}
		}


        if (Fl.WtrCntrErr) {
		    if (Fl_State.WtrCnt != REPORT_FLAG_ERR) {
			    Fl_State.WtrCnt = REPORT_FLAG_ERR;
			}
		}
		else if (Fl_State.WtrCnt != REPORT_FLAG_OK) {
		    Fl_State.WtrCnt = REPORT_FLAG_OK;
		}


		if (Fl.WtrCntrErr && Sygnal_Get.DoorOpn) {
			Fl.WtrCntrErr = 0;
	    }


		if (Fl_Get_New_Data && !Fl.SellStart) {                                /* When sell finneshed or pause the quantity of water send to registrator */
			    Fl_Get_New_Data = 0;
                Fl_Send_Sell_End = 1;
	    }


/////////// Got signal from accelerometer //////////////////////////////////////
        static u08 is_accel_timer_start = 0;
	    if (Sygnal_Get_Accellerometer) { 
		
            if (AccellCntTimer == 0) {
		    	AccellCntTimer = ACCELEROMETER_PERIOD;
		        Sygnal_Get_Accellerometer = 0;			
			    is_accel_timer_start = 1;
        
			    SYSTEM_EVENTS = Fl_Ev_GetMoving;
                xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
            }
        } else if (is_accel_timer_start && AccellCntTimer == 0) {
		    is_accel_timer_start = 0;
			AccelerometerEnable();
		}
    
/////// the sygnall set when bill can't get money ////////////////////////////

        if (Sygnal_Get.NoWrkBill && EEPR_LOCAL_COPY.board_version) {            // If board version the first we

	        if (!Fl.ErrRsvBill) {                                               // can't get the right status 
			    Fl_State.RsvBill = REPORT_FLAG_ERR;
				Fl.ErrRsvBill = 1;

				if (uxQueueMessagesWaiting(xEventsQueue) < 8) {
				    SYSTEM_EVENTS = Fl_Ev_ErrorBill;
                    xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
				}
            }
		}                                                          // of the bill receiver   
        else if (Fl.ErrRsvBill) {
	        Fl.ErrRsvBill = 0;
			Fl_State.RsvBill = REPORT_FLAG_OK;
	    }

//  ////////////////////////////////////////////////////////////////////////////////
 
        if ((EEPR_LOCAL_COPY.amount_water <= ((u32)EEPR_LOCAL_COPY.water_level_marck_min))) {
	        if (!Fl.ErrMinWater) {
	            Fl.ErrMinWater = 1;	
			    
				SYSTEM_EVENTS = Fl_Ev_LimWater;
				xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);

            }
	    }
        else {
	        Fl.ErrMinWater = 0;	
	    }

//  ////////////////////////////////////////////////////////////////////////////////
    
    vTaskDelay(2 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}


void vTask5( void *pvParameters )
{
    u08 send_data_buff[60];
	u08 Script_Name[16];
	u08 Password[10];
	u08 Dns_Name[30];
	u08 Server_Name[30];

	u08 is_domain_name = 1;
	
	static struct COLLECTION_DATA_TO_SERVER data;

	u08 err_conn_cnt = 0;
	u08 disconnect_count = 0; 
	u08 err_conn_fail = 0;
	
    u08 *p_data_len = 0;      


	memset((u08 *)&Fl_State, REPORT_FLAG_OK, sizeof(Fl_State));

    
    data.ApparatNum  = (u16 *) &EEPR_LOCAL_COPY.vodomat_number;
	data.WaterQnt    = 0;
	data.Price       = (u16 *) &EEPR_LOCAL_COPY.cost_litre_coef;

	data.TimeToBlock = (u08 *) &Hours_BeforeApparatBlock;
	
	data.Flag1    = (u08 *) &Fl_State.Water;
	data.Flag2    = (u08 *) &Fl_State.Power;
	data.Flag3    = (u08 *) &Fl_State.RsvBill;
	data.Flag4    = (u08 *) &Fl_State.WtrCnt;
	data.Flag5    = (u08 *) &Fl_State.RrState;
	data.Flag6    = (u08 *) &Fl_State.Reserve;
	data.EventNum = 0;

    memcpy((u16 *)&data.Money, (u16 *)&MoneyCounterToSave,  sizeof(data.Money));

	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    memcpy((u16 *)&data.DateTime, (u16 *)&TimeAndDate_System.Minute,  sizeof(data.DateTime));
	xSemaphoreGive(xI2CMutex);


    struct GSM_WAIT_STRUCT {
        u16 Interval;
	    CARRENT_STATE_CARRENT State_Change;
    } GSM_Timer = {0};

//	GSM_WAIT_STRUCT ;


    memset(Script_Name, 0x00, 16);
	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
	IntEeprBlockRead((u16)&Script_Name[0], ScriptNameEEPROMAdr, 16);
    Script_Name[15] = 0;
	xSemaphoreGive(xI2CMutex);
				 
    memset(Password, 0x00, 10);
	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    IntEeprBlockRead((u16)&Password[0], PasswordEEPROMAdr, 10);
	Password[9] = 0;
	xSemaphoreGive(xI2CMutex);
	

    memset(Server_Name, 0x00, 30);
	xSemaphoreTake(xI2CMutex, portMAX_DELAY);
	IntEeprBlockRead((u16)&Server_Name[0], ServerNameEEPROMAdr, 30);
	xSemaphoreGive(xI2CMutex);

    is_domain_name = DomainNameOrIpChack(Server_Name, 30);
	while (is_domain_name == -1) ;

    static u08 len;
	len = strnlen((char*)&Server_Name[0], 30); 
	Server_Name[len] = '\n';
	++len;
    Server_Name[len] = 0;

    strncpy((char*)&send_data_buff[0], (char*)&Server_Name[0], 30);

	p_data_len = (u08 *)strstr((char *)&send_data_buff[0], ",");
	if (p_data_len) {
        len = (p_data_len - &send_data_buff[1]) - 1;
	memset(Dns_Name, 0x00, 30);
        strncpy((char *)&Dns_Name[0], (char *)&send_data_buff[1], len);
        Dns_Name[len] = '\n';
        ++len;
	    Dns_Name[len] = 0; 
	}
	else {
	    Dns_Name[0] = '\n';
        Dns_Name[1] = 0;
	}
	
    vTaskDelay(5000 / portTICK_RATE_MS);


	for( ;; )
    {

        switch (CARRENT_STATE) {

            case STATE_MODEM_IDLE: {
			     break;
			}
					    
			case STATE_MODEM_ON: {
#if MODEM_DBG
	    uartSendByte(0, '1');
		uartSendByte(0, '\n');
#endif
                 if (ModemStatus() == 1) {
                     
					 Uart1Enable();		
                     vTaskDelay(2 / portTICK_RATE_MS);	     
				     
					 if (ModemSendCom(AT, 500) == ACK_OK) {
						 CARRENT_STATE = STATE_GPRS_DEACTIVATE;

				     }
				     else {
				         CARRENT_STATE = STATE_MODEM_OFF;
				     }
				 }
                 else {

			         PWRKEY_ON;
                     vTaskDelay(2000 / portTICK_RATE_MS);
					 PWRKEY_OFF;

					 u08 i = 0;
                     while (ModemStatus() == 0 && i < 60) {
				         vTaskDelay(1000 / portTICK_RATE_MS);
					     ++i;
				     }

                     if (i == 60) {
					     CARRENT_STATE = STATE_MODEM_OFF;
					 } else {
                         Uart1Enable();		
                         vTaskDelay(2 / portTICK_RATE_MS);				   
   
	                     GSM_Timer.State_Change = STATE_MODEM_FIRST_INIT;
					     GSM_Timer.Interval = 6000;
					     CARRENT_STATE = STATE_SOME_WAIT;
					}
                 }   
			     break;
			} 

			case STATE_MODEM_OFF: {
#if MODEM_DBG
			uartSendByte(0, '2');
			uartSendByte(0, '\n');
#endif
                 Uart1Disable();			     
                 vTaskDelay(100 / portTICK_RATE_MS);
				 PWRKEY_ON;
                 vTaskDelay(1000 / portTICK_RATE_MS);
                 PWRKEY_OFF;

                 CARRENT_STATE = STATE_MODEM_ON;

                 u08 i = 0;
				 while (ModemStatus()) {
				     
					 vTaskDelay(1000 / portTICK_RATE_MS);
					 if (i == 10) {
					     CARRENT_STATE = STATE_MODEM_OFF;
					     break;
                     } 
					 i++;
				 }
				 vTaskDelay(1000 / portTICK_RATE_MS);
				 

			     break;
			} 

			case STATE_MODEM_FIRST_INIT: {
#if MODEM_DBG
			uartSendByte(0, '3');
            uartSendByte(0, '\n');
#endif
                 ModemSendCom(AT, 500);

            	 ModemSendCom(ECHO_OFF, 500);

             	 // Set baudrate									
	             ModemSendCom(SET_BAUD, 500);				 		
                 
                 //Save settigs into modem's flash
	             ModemSendCom(SAVE_FLESH, 1000);				 		
	             				
				 if (ModemSendCom(AT, 500) == ACK_OK) {
				     CARRENT_STATE = STATE_MODEM_INIT;
				 }
				 else {
				     CARRENT_STATE = STATE_MODEM_OFF;
				 }
                 break;
			} 	 

			case STATE_MODEM_INIT: {
#if MODEM_DBG
			uartSendByte(0, '4');
            uartSendByte(0, '\n');
#endif
				 //Pfone book on sim 
	             ModemSendCom(PFONE_BOOK, 500);
                  
                 ModemSendCom(SET_TEXT_MODE, 500);

	             ModemSendCom(SET_FORMAT_1_SMS, 500);

                 ModemSendCom(INDICATION_NEW_SMS, 500);

//                 ModemSendCom(CLEAR_SMS, 1000);

                 CARRENT_STATE = STATE_GPRS_CONNECT;

			     break;
			} 

            //case STATE_TCP_IP_CONFIG: {  
			case STATE_GPRS_CONNECT: {
#if MODEM_DBG
			uartSendByte(0, '5');
            uartSendByte(0, '\n');
#endif
                 u08 data_len = 0; 

                 ModemSendCom(CLEAR_SMS, 3000);
				 
				 ModemSendCom(SET_GPRS_FORMAT, 500);


				 ModemSendCom(SET_TYPE_CONNECTION, 1);

				 memset(send_data_buff, 0x00, 60);

				 xSemaphoreTake(xI2CMutex, portMAX_DELAY);
				 IntEeprBlockRead((u16)&send_data_buff[0], ApnUserPassAdressEEPROMAdr, 20);
				 xSemaphoreGive(xI2CMutex);

				 xSemaphoreTake(xI2CMutex, portMAX_DELAY);
				 IntEeprBlockRead((u16)&send_data_buff[20], (ApnUserPassAdressEEPROMAdr + 20), 20);
				 xSemaphoreGive(xI2CMutex);

 				 xSemaphoreTake(xI2CMutex, portMAX_DELAY);
				 IntEeprBlockRead((u16)&send_data_buff[40], (ApnUserPassAdressEEPROMAdr + 40), 20);
				 xSemaphoreGive(xI2CMutex);

				 data_len = strnlen((char *)&send_data_buff[0], 60);
				 if (data_len <= 60 - 2) {
				     send_data_buff[data_len] = '\n';
				     send_data_buff[data_len+1] = 0;
				 }
				 else {
				     send_data_buff[60-2] = '\n';
				     send_data_buff[60-1] = 0;
				 }

				 if (ModemSendData((char *)&send_data_buff[0], 1000) == ACK_OK) {
				     CARRENT_STATE = STATE_GPRS_FORMED_BUFF;


				 if (is_domain_name) {
				     ModemSendCom(CONNECT_BY_DOMAIN_NAME, 500);
				 } else {
				     ModemSendCom(CONNECT_BY_IP_ADDRESS, 500);
				 }

				 ModemSendCom(SINGLE_CONNECTION, 500);

				 ModemSendCom(NON_TRANSPARENT_MODE, 500);

				 if (ModemSendCom(TCPIP_STACK_CONNECT, 1000) != ACK_OK) {
                     //CARRENT_STATE = STATE_GPRS_DEACTIVATE; 
				 }	
                 
				 if (ModemSendCom(ACTIVEATE_FGCNT, 10000) != ACK_OK) {
                     //CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				 }
				
				 ModemSendCom(QUERY_IP, 500);

				 } else {
				     GSM_Timer.State_Change = STATE_GPRS_CONNECT;
				 	 GSM_Timer.Interval = 100; 
				 	 CARRENT_STATE = STATE_SOME_WAIT;
				 }
				 
				 if (err_conn_cnt == 3) {
				     err_conn_cnt = 0;
					 CARRENT_STATE = STATE_MODEM_OFF;
				     //CARRENT_STATE = STATE_NET_STATUS;			 
				 } else {
				     err_conn_cnt++;
				 }
				 
                 break;
			}

			case STATE_GPRS_CHECK: {
#if MODEM_DBG
			uartSendByte(0, '6');
			uartSendByte(0, '\n');
#endif
				 GSM_Timer.State_Change = STATE_GPRS_OPEN;
				 GSM_Timer.Interval = 1000; 
				 CARRENT_STATE = STATE_SOME_WAIT;
				 			     
				 if (ModemSendCom(CHECK_STATUS_CONNECTION, 1000) != ACK_OK) {
                         
				 }  
				                
				 break;
			}

			case STATE_STACK_MODE_CHACK: {
			     
				 GSM_Timer.State_Change = STATE_GPRS_DEACTIVATE;
				 GSM_Timer.Interval = 500; 
				 CARRENT_STATE = STATE_SOME_WAIT;
				 if (ModemSendCom(MODE_SERVER_OR_CLIENT, 300) != ACK_OK) {
				     CARRENT_STATE = STATE_GPRS_CHECK;
				 }
				 
				 break;
			}

			case STATE_STACK_MODE_CHANGE: {

                 GSM_Timer.State_Change = STATE_GPRS_CHECK;
				 GSM_Timer.Interval = 500; 
				 CARRENT_STATE = STATE_SOME_WAIT;
                 if (ModemSendCom(MODE_CLIENT_SET, 300) == ACK_OK) {
				     CARRENT_STATE = STATE_GPRS_SEND_DATA;    
				 }
                 break;
            }

			case STATE_GPRS_OPEN: {
#if MODEM_DBG
			uartSendByte(0, '7');
			uartSendByte(0, '\n');
#endif

			//     vTaskDelay(10000 / portTICK_RATE_MS);
                 
				 GSM_Timer.State_Change = STATE_GPRS_DEACTIVATE;
				 GSM_Timer.Interval = 7500; 
				 CARRENT_STATE = STATE_SOME_WAIT;

				 ModemSendCom(CONNECT_TO_SERVER, 3);
                 
                 ModemSendData((char *)&Server_Name[0], 2000);
                 
				 break;
			}

            case STATE_GPRS_FAIL: {
#if MODEM_DBG
			uartSendByte(0, '8');
			uartSendByte(0, '\n');
#endif
			   
                 err_conn_fail++; 
			     vTaskDelay(30000 / portTICK_RATE_MS);
//                 CARRENT_STATE = STATE_GPRS_OPEN;
                 //CARRENT_STATE = STATE_NET_QUALITY_LOW;
				 if (err_conn_fail >= 3) {
				     err_conn_fail = 0;
				     CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				 }
				 else {
				     CARRENT_STATE = STATE_GPRS_FORMED_BUFF;
				 }
				 break;
			}

            case STATE_GPRS_FORMED_BUFF: {

                if (xQueueReceive(xEventsQueue, &data.EventNum, 200 / portTICK_RATE_MS) == pdPASS) {
				   CARRENT_STATE = STATE_GPRS_CHECK;

			    } else if (QueueEncashmentNum()) {
                    data.EventNum = Fl_Ev_TakeManey;
					CARRENT_STATE = STATE_GPRS_CHECK;
				}
			    break;
			}

			case STATE_GPRS_SEND_DATA: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '1');
			uartSendByte(0, '\n');
#endif
			     err_conn_cnt = 0;
				 disconnect_count = 0;

				 if (ModemSendCom(SEND_GPRS_DATA, 500) == ACK_CAN_SEND) {

/////////////////////////////////////////////////////////
                     ModemSendCom(Get, 1);					                   // "GET /"
                     ModemSendData((char *)&Script_Name[0], 1);                // 
                     ModemSendCom(Data, 1);					                   // "?data="
					 
					 memset(send_data_buff, 0x00, 60);

                     xSemaphoreTake(xI2CMutex, portMAX_DELAY);
					 if (data.EventNum == Fl_Ev_TakeManey && QueueEncashmentNum()) {
					     QueueEncashmentGet((ENCASHMENT_T *)&data.DateTime, 0);

						 data.WaterQnt = EEPR_LOCAL_COPY.amount_water;
                     }
					 else if (data.EventNum == Fl_Ev_NoPower) {
	                     data.Money.Sum = ShouldReturnToBuyer.Money;
						 data.Money.Bill = MoneyCounterToSave.Bill;
						 data.Money.Coin = MoneyCounterToSave.Coin;
						 memcpy((u16 *)&data.DateTime, (u16 *)&TimeAndDate_System.Minute,  sizeof(data.DateTime)); 

						 data.WaterQnt = ShouldReturnToBuyer.Water;
					 } 
					 else {
	                     data.Money.Sum = MoneyCounterToSave.Sum;
						 data.Money.Bill = MoneyCounterToSave.Bill;
						 data.Money.Coin = MoneyCounterToSave.Coin;
						 memcpy((u16 *)&data.DateTime, (u16 *)&TimeAndDate_System.Minute, sizeof(data.DateTime)); 

						 data.WaterQnt = EEPR_LOCAL_COPY.amount_water;
                     }
                     xSemaphoreGive(xI2CMutex);  
 
#if (CHECK_STACK == 1)
			         DebugBuff[3] = uxTaskGetStackHighWaterMark(NULL);
#elif (CHECK_STACK == 2)
             
			 		//unsigned portBASE_TYPE uxTaskGetStackHighWaterMark( xTaskHandle xTask );
                    static u08 i = 0;
                    static u16 z = 0;
                    DebugBuff[3] = uxTaskGetStackHighWaterMark(NULL);
                    data.Price =  &DebugBuff[i++];
					z = (i + 1);
					data.ApparatNum = &z;
             
                    if(i >= TASK_NUMBER) {
                        i = 0;
					}

#endif /* CHECK_STACK */ 
                     
					 Create_Report_String(&data, &send_data_buff[0]);

                     ModemSendData((char *)&send_data_buff[0], 1);             //
//			         memset(send_data_buff, 0x00, 300);
//			         vTaskList((portCHAR *)&send_data_buff[0]);
//                   ModemSendData((char *)&send_data_buff[0], 5);

                     ModemSendCom(Pass, 1);					 				   // "&pass="	 
					 
					 ModemSendData((char *)&Password[0], 1);
                     ModemSendCom(Http, 1);					 	               // " HTTP/1.1\n"
                     ModemSendCom(Host, 1);                                    // "Host: "					 	
					 ModemSendData((char *)&Dns_Name[0], 1);
/////////////////////////////////////////////////////////

					 CARRENT_STATE = STATE_SOME_WAIT;
                     GSM_Timer.State_Change = STATE_GPRS_DISCONNECT;
                     GSM_Timer.Interval = 5000;

					 if (ModemSendCom(Conn, 5000) == ACK_SEND_OK) {
						 //vTaskDelay(10000 / portTICK_RATE_MS);
					 }
			     } 
                 else {
				     CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				 } 
                 break;
			}

			case STATE_HTTP_SERVER_200_OK: {
				 
				 CARRENT_STATE = STATE_SOME_WAIT;
                 GSM_Timer.State_Change = STATE_GPRS_DISCONNECT;
                 GSM_Timer.Interval = 10000;

			     if (data.EventNum == Fl_Ev_TakeManey && QueueEncashmentNum()) {
				     QueueEncashmentGet((ENCASHMENT_T *)&data.DateTime, 1);
				 }
				 else if (data.EventNum == Fl_Ev_NoPower) {
				     IsDataToReturnSent = 1;
				 }
				 
				 break;
			}

			case STATE_GPRS_DISCONNECT: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '2');
			uartSendByte(0, '\n');
#endif
              
				 CARRENT_STATE = STATE_SOME_WAIT;
                 GSM_Timer.State_Change = STATE_GPRS_DEACTIVATE;
                 GSM_Timer.Interval = 500;

				 if (ModemSendCom(DISCONNECT_SESSION, 300) == ACK_ERROR) {
				     CARRENT_STATE = STATE_GPRS_FORMED_BUFF;
				 }

				 break;
			}
			
			case STATE_GPRS_DEACTIVATE: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '3');
			uartSendByte(0, '\n');
#endif

                 if (disconnect_count >= 3) {
	                 disconnect_count = 0;
					 CARRENT_STATE = STATE_MODEM_OFF;
				 }
				 else {
				 	 disconnect_count++;

				     GSM_Timer.State_Change = STATE_MODEM_OFF;
				     GSM_Timer.Interval = 100; 
				     CARRENT_STATE = STATE_SOME_WAIT;
				 				 
				     if (ModemSendCom(DISCONNECT_GPRS, 40000) == ACK_ERROR) {
				         CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				     }
				 }

				 break;
			}

			case STATE_SOME_WAIT: {

                 if (GSM_Timer.Interval == 0) {
				     CARRENT_STATE = GSM_Timer.State_Change;
					 GSM_Timer.State_Change = 0;
				 }
                 else {
				     vTaskDelay(10 / portTICK_RATE_MS);
					 GSM_Timer.Interval--;
				 }
				 break;
			}

			case STATE_NET_STATUS: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '4');
			uartSendByte(0, '\n');
#endif
                 GSM_Timer.State_Change = STATE_MODEM_OFF;
				 GSM_Timer.Interval = 100; 
				 CARRENT_STATE = STATE_SOME_WAIT; 
                 ModemSendCom(GET_QUALITY_OF_NET, 1000);

				 break;
			}

			case STATE_NET_QUALITY_LOW: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '5');
			uartSendByte(0, '\n');
#endif

				 vTaskDelay(3000 / portTICK_RATE_MS);
				 CARRENT_STATE = STATE_NET_STATUS;
				 
				 break;
			}
		
		}

//        vTaskDelay(500 / portTICK_RATE_MS);

	}

    vTaskDelete(NULL);
}


void vTask6( void *pvParameters )
{

//	u08 com_buff[200];
#define  COM_BUFF_LEN    30
    u08 com_buff[COM_BUFF_LEN];
	u08 cnt = 0;

	BUZZER_ON;                                                                 /* buzzer will be turn off when timeer finnished */
	xTimerStart(xTimer_ModemStart, 0);
	
	xSemaphoreTake(xGsmModemWaitingSem, 0);

	for( ;; )
    {
        if (GSM_RxBuf_Count_Get()) {
	     
		    com_buff[cnt] = GSM_RxBuf_Char_Get();


#if MODEM_DBG
			uartSendByte(0, com_buff[cnt]);
			//uartSendByte(0, '\n');
#endif
            
			if (cnt == 0 && !isprint(com_buff[cnt])) {
			
			}
		    else if ((com_buff[cnt] == '\n') || (com_buff[cnt] == '>')) {
				if (com_buff[cnt] == '>') {
				    com_buff[cnt+1] = 0;
				}
				else {
				    com_buff[cnt] = 0;
				} 
			
				//if (com_buff[cnt] == '>' || cnt > 1)
				if (custom_at_handler(com_buff)) {
				    xSemaphoreGive(xGsmModemWaitingSem);
				}
				
				cnt = 0;
		    }
			else {
			    cnt++;
				if(cnt == COM_BUFF_LEN - 1) {
				    cnt = 0;
				}
			}
		}

        vTaskDelay(10 / portTICK_RATE_MS);

#if CHECK_STACK
    DebugBuff[4] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

    }

    vTaskDelete(NULL);
}


/////////////////////////////////////////////////////////////////////////////////////

u08 custom_at_handler(u08 *pData)
{ 
    u08 *p;
	u08 is_ack_get;    

    is_ack_get = 0;
    if (strncmp_P((char *)pData, PSTR("Call Ready"), sizeof("Call Ready") - 1) == 0) {
		CARRENT_STATE = STATE_GPRS_CONNECT;
    }
	else if (strncmp_P((char *)pData, PSTR(">"), sizeof(">") - 1) == 0) {
		ModemAnsverSet(ACK_CAN_SEND); 
		is_ack_get = 1;
	}
	else if (strstr_P((char *)pData, PSTR("200 OK"))) {
        CARRENT_STATE = STATE_HTTP_SERVER_200_OK;
	}
	else if (strncmp_P((char *)pData, PSTR("OK"), sizeof("OK") - 1) == 0) {
		ModemAnsverSet(ACK_OK);
		is_ack_get = 1;
	}
	else if (strncmp_P((char *)pData, PSTR("ERROR"), sizeof("ERROR") - 1) == 0) {
        ModemAnsverSet(ACK_ERROR);
		is_ack_get = 1;
	}
    else if (strncmp_P((char *)pData, PSTR("SEND OK"), sizeof("SEND OK") - 1) == 0) {
        ModemAnsverSet(ACK_SEND_OK);
		is_ack_get = 1;
	}
	else if (strncmp_P((char *)pData, PSTR("SEND FAIL"), sizeof("SEND FAIL") - 1) == 0) {
        ModemAnsverSet(ACK_SEND_FAIL);
		is_ack_get = 1;
	}
	else if (strncmp_P((char *)pData, PSTR("CONNECT OK"), sizeof("CONNECT OK") - 1) == 0) {
        CARRENT_STATE = STATE_STACK_MODE_CHACK;
	}
	else if (strncmp_P((char *)pData, PSTR("STATE: CONNECT OK"), sizeof("STATE: CONNECT OK") - 1) == 0) {
        CARRENT_STATE = STATE_STACK_MODE_CHACK;
	}
	else if (strncmp_P((char *)pData, PSTR("ALREADY CONNECT"), sizeof("ALREADY CONNECT") - 1) == 0) {
        CARRENT_STATE = STATE_STACK_MODE_CHACK;
	}
	else if (strncmp_P((char *)pData, PSTR("+QISRVC: 1"), sizeof("+QISRVC: 1") - 1) == 0) {
        CARRENT_STATE = STATE_GPRS_SEND_DATA;
	}
    else if (strncmp_P((char *)pData, PSTR("+QISRVC: 2"), sizeof("+QISRVC: 2") - 1) == 0) {
        CARRENT_STATE = STATE_STACK_MODE_CHANGE;
	}
	else if (strncmp_P((char *)pData, PSTR("CLOSE OK"), sizeof("CLOSE OK") - 1) == 0) {
        CARRENT_STATE = STATE_GPRS_FORMED_BUFF;
	}
	else if (strncmp_P((char *)pData, PSTR("CONNECT FAIL"), sizeof("CONNECT FAIL") - 1) == 0) {
        CARRENT_STATE = STATE_GPRS_FAIL;
	}
	else if (strncmp_P((char *)pData, PSTR("DEACT OK"), sizeof("DEACT OK") - 1) == 0) {
        CARRENT_STATE = STATE_GPRS_CONNECT;
	}
    else if (strncmp_P((char *)pData, PSTR("REQUEST DATA"), sizeof("REQUEST DATA") - 1) == 0) {
  		SYSTEM_EVENTS = Fl_Ev_RequestData;
		xQueueSend(xEventsQueue, &SYSTEM_EVENTS, 0);
	}
    else if (strncmp_P((char *)pData, PSTR("RESET MODEM"), sizeof("RESET MODEM") - 1) == 0) {
  	    CARRENT_STATE = STATE_MODEM_OFF;
	}
	else if (strncmp_P((char *)pData, PSTR("CLOSED"), sizeof("CLOSED") - 1) == 0) {
		CARRENT_STATE = STATE_GPRS_FORMED_BUFF;
	}
	else if ( (p = (u08 *)strstr_P((char *)pData, PSTR("+CSQ:")))) {
        u08 k = 0;
        u08 *qend;
		
		p += 6;
		qend = (u08 *)strstr_P((const char *)p, PSTR(","));
		if (!(qend && qend > p)) {
			return (0);
		}

        k = qend - p;
		if (k != 2 && k != 1) {
            return (0);
		}

	    k = (u08)atoin(p, k);
		if (k == 0) {
	        return (0);
		}
		
		if (k == 99 || k < 8) { 
	        CARRENT_STATE = STATE_NET_QUALITY_LOW;
	    }
        else {
	        CARRENT_STATE = STATE_GPRS_CONNECT;    //
	    }
    }
	else if ( (p = (u08 *)strstr_P((char *)pData, PSTR("Price=")))) {
        u16 price = 0;
        
		p += 6;
        if (strlen((const char *)p) < 4) {
            return (0);
		}
		
        if ((price = atoin(p, 4)) == 0) {
		    return (0);
		}

		if (EEPR_LOCAL_COPY.cost_litre_coef != price && 500 >= price) {
		    portENTER_CRITICAL();
            NewPrice = price;
		    portEXIT_CRITICAL();
		}
    }
	else if ((p = (u08 *)strstr_P((char *)pData, PSTR("+QISRVC=")))) {
         
         if (atoin((p + 8), 1) == 2) {                                         /* chack current service server is 2, client is 1*/
		     ;
		 } else {
             ;		 
 		 }
	}

	return is_ack_get;
}
/////////////////////////////////////////////////////////////////////////////////////


static inline u16 MoneyToWater (u16 money_quantity) 
{
	if (EEPR_LOCAL_COPY.cost_litre_coef > 0)
	    return (u16)(((((u32)money_quantity * 200) / (EEPR_LOCAL_COPY.cost_litre_coef)) + 1) >> 1);

	return 0;
}


static inline u16 MoneyToPulse (u16 money_quantity) 
{
	if ((EEPR_LOCAL_COPY.cost_litre_coef  > 0) && (EEPR_LOCAL_COPY.pulse_litre_coef > 0))
//  CountPulse = (u16)((((((money * 1024) / (*cost_litre_coef)) * (*pulse_litre_coef)) >> 15) + 1) >> 1);
        return (u16)(((((((u32)money_quantity << 10) / (EEPR_LOCAL_COPY.cost_litre_coef)) * (EEPR_LOCAL_COPY.pulse_litre_coef)) >> 15) + 1) >> 1);
	    
	return 0;
}

static inline u16 PulseQuantityToMoney (u16 pulse_quantity) 
{
	if ((EEPR_LOCAL_COPY.pulse_litre_coef > 0) && (pulse_quantity > 0))
        return (u16)((((((u32)EEPR_LOCAL_COPY.cost_litre_coef) * 8388608) / ((((u32)EEPR_LOCAL_COPY.pulse_litre_coef) * 65536) / pulse_quantity)) + 1) >> 1);

	return 0;
}

/* UART0 Receiver interrupt service routine */
void Uart0_Resiv (u08 udrdata) {
//	uart1SendByte(udrdata);
    ///*		
	    BUF_UART_RX[rx] = udrdata;

	    if (rx == BUF_UART_RX[0] && 0x07 >= BUF_UART_RX[0]) {
		    rx = (MAX_RX_SIZE_BUFF - 3);
		    //static portBASE_TYPE xHigherPriorityTaskWoken;
			static signed portBASE_TYPE xHigherPriorityTaskWoken;
		    xSemaphoreGiveFromISR(xUart_RX_Semaphore, &xHigherPriorityTaskWoken);
	    }

	    rx++;
	    if (rx == (MAX_RX_SIZE_BUFF - 2)) {
	        rx = 0;
	    }
    //*/
}	

void Global_Time_Deluy (unsigned int time_val) {

    vTaskDelay(time_val / portTICK_RATE_MS);
}


extern void RegistratorSendStr (u08 *s, u08 len) {
//    uartSendBuf(0, s , len);
	while (len-- > 0) {
	    uartSendByte(0, *s);
		s++;
	}
}


extern void ModemAnsverWeit_callback (unsigned long time)
{
    for (time /= 10; time && (xSemaphoreTake(xGsmModemWaitingSem, 10 / portTICK_RATE_MS) != pdTRUE); time--) ;
}

/*
void vApplicationIdleHook( void )
{
    for( ;; ) {
        vCoRoutineSchedule();
    }
}
*/


//void vApplicationStackOverflowHook (xTaskHandle *pxTask, signed portCHAR *pcTaskName) {
//	 PORTA &= ~(1 << 4);

//     uartSendBuffer(0, &pcTaskName[0], 6);
//}

//vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {

//    uartSendBuffer(0, pcTaskName, 6);
//}

//vTaskStartTrace(Trace_Buffer, 38);

//unsigned portBASE_TYPE uxTaskGetStackHighWaterMark( xTaskHandle xTask );

