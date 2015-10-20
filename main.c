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
const char  AT[] PROGMEM       	             = "AT\n";
const char  ECHO_OFF[] PROGMEM 	             = "ATE0\n";                  
const char  SET_BAUD[] PROGMEM               = "AT+IPR=19200\n";	      
const char  SAVE_FLESH[] PROGMEM             = "AT&W\n";		          

const char  PFONE_BOOK[] PROGMEM             = "AT+CPBS=\"SM\"\n";        // Pfone book on sim
//const char  PFONE_BOOK[] PROGMEM             = "AT+CPBS=\"SM\",\"SM\",\"SM\"";
const char  READ_PF_NUM[] PROGMEM	         = "CPBR=1\n";                // Read pfone namber from sim
const char  CLEAR_SMS[] PROGMEM              = "AT+CMGD=1\n";         
const char  CLEAR_ALL_SMS[] PROGMEM          = "AT+CMGD=1,4";         
const char  INDICATION_NEW_SMS[] PROGMEM     = "AT+CNMI=2,1,0,0,0\n";   
const char  SET_TEXT_MODE[] PROGMEM          = "AT+CMGF=1\n";
const char  SET_FORMAT_1_SMS[] PROGMEM       = "AT+CSCS=\"GSM\"\n";
const char  SET_FORMAT_2_SMS[] PROGMEM       = "AT+CSMP=17,200,0,25\n";
const char  SEND_SMS[] PROGMEM		         = "AT+CMGS=";

//GPRS Connection
const char  SET_GPRS_FORMAT[] PROGMEM        = "AT+QIFGCNT=0\n"; 
const char  SET_TYPE_CONNECTION[] PROGMEM    = "AT+QICSGP=1,";     // "1,APN","user","password"
//const char  SET_GPRS_MODE[] PROGMEM        = "AT+QIMUX=0\n";     // "0" - non transparent "1" -transparent mode
const char  CONNECT_DOMAIN_NAME[] PROGMEM    = "AT+QIDNSIP=1\n";    // "0" - use IP adress, "1" - use domain name

const char  CONNECT_STACKS[] PROGMEM         = "AT+QIREGAPP\n";    
const char  ACTIVEATE_FGCNT[] PROGMEM        = "AT+QIACT\n"; 
const char  QUERY_IP[] PROGMEM               = "AT+QILOCIP\n";    
const char  CONNECT_TO_SERVER[] PROGMEM      = "AT+QIOPEN=\"TCP\",";
const char  SEND_GPRS_DATA[] PROGMEM         = "AT+QISEND\n";
const char  CHECK_STATUS_CONNECTION[] PROGMEM= "AT+QISTAT\n";

const char  DISCONNECT_SESSION[] PROGMEM     = "AT+QICLOSE\n";
const char  DISCONNECT_GPRS[] PROGMEM        = "AT+QIDEACT\n";
const char  GET_QUALITY_OF_NET[] PROGMEM     = "AT+CSQ\n";

//const char  VISUAL_DIAL_NUM[] PROGMEM        = "AT+CLIP=1\n";

const char  Get[] PROGMEM        = "GET /";
const char  Http[] PROGMEM       = " HTTP/1.1\n";
const char  Data[] PROGMEM       = "?data=";
const char  Pass[] PROGMEM       = "&pass=";
const char  Host[] PROGMEM       = "Host: ";
const char  Conn[] PROGMEM       = "Connection: Keep-Alive\n\n\32"; // \32 - Ctrl^z

// SMS Text
const char  MESSAGE_EMPTY[] PROGMEM          = "Low signal quality. Available only SMS\32";
const char  MESSAGE_NO_WATER[] PROGMEM       = "NET VODU\32";
const char  MESSAGE_NO_POWER[] PROGMEM       = "NET NAPRAGENIA\32";
const char  MESSAGE_TAKE_MANEY[] PROGMEM     = "INKASACIA\32";
const char  MESSAGE_GET_MOVING[] PROGMEM     = "UDAR\32";
const char  MESSAGE_LIM_WATER[] PROGMEM      = "MALO VODU\32";
const char  MESSAGE_SEND_ANSVER[] PROGMEM    = "STATISTIKA\32";
const char  MESSAGE_ERR_BILL[] PROGMEM       = "OSHUBKA KYPYROPRIEMNIKA\32";


PGM_P SMS_TEXT[] PROGMEM = {

    MESSAGE_EMPTY,
	MESSAGE_NO_WATER,
    MESSAGE_NO_POWER,
    MESSAGE_TAKE_MANEY,
    MESSAGE_GET_MOVING,
    MESSAGE_LIM_WATER,
    MESSAGE_SEND_ANSVER,
    MESSAGE_ERR_BILL

};


#define ACCELEROMETR_PERIOD   3000ul           //3000 * 100mS = 300S
#define CHECK_COUNTER_PERIOD  50ul             //50  * 100mS = 5S

#define MINUTES_IN_DAY        1440ul    

#define REPORT_FLAG_ERR       0x31   
#define REPORT_FLAG_OK        0x30

#define SMS_FLAG_DAY          0x01
#define SMS_FLAG_NIGHT        0x00
#define SMS_FLAG_SEND_DISABLE 0x02   

#define  TIME_SEND_REGUEST    50ul   // 1 second = x * 100ms - is task sleep    


/*
*********************************************************************************************************
*                                            LOCAL VARIABLES
*********************************************************************************************************
*/


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
	STATE_SMS_PREPARE,
	STATE_SMS_SEND_DATA,
    STATE_GPRS_SEND_DATA
  
} CARRENT_STATE_CARRENT;

CARRENT_STATE_CARRENT CARRENT_STATE;

//u08 LcdDatta[8];
u08 gFlLedStateWater = 0;

u08 WtrCntTimer;
u16 AxellCntTimer;

volatile u08 FlDayOrNight = SMS_FLAG_SEND_DISABLE;
volatile u16 NewPrice = 0;


xQueueHandle xEventsQueue;

xSemaphoreHandle xUart_RX_Semaphore;

xSemaphoreHandle xI2CMutex;
xSemaphoreHandle xExtSignalStatusSem; 
xSemaphoreHandle xTimeSendRequestSem;


//u08 Trace_Buffer[300];
//u08 send_data_buff[300];

#if BUZER_TIME
u08 buzer_flag = 0;
#endif


static u08 IsRegistratorConnect = 0;

static u16 ExtSignalStatus = 0;

static u16 Tmr_For_Init_Rr = 600; /* need time in Sec = Tmr_For_Init_Rr * 100ms, 600 * 100 = 60 S  */


#if CHECK_STACK
#define TASK_NUMBER    7
static u16 DebugBuff[TASK_NUMBER] = {0};
#endif //CHECK_STACK

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );
void vTask3( void *pvParameters );
void vTask4( void *pvParameters );
void vTask5( void *pvParameters );
void vTask6( void *pvParameters );

#if BUZER_TIME
void vTask7( void *pvParameters );
#endif

//void vCoRoutineBuzerControll (xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex);


static inline u16 MoneyToWater (u16 money_quantity);
static inline u16 MoneyToPulse (u16 money_quantity); 
static inline u16 PulseQuantityToMoney (u16 pulse_quantity);

void Uart0_Resiv (u08 udrdata);
void Global_Time_Deluy (unsigned int time_val);

//u08 ModemSendCom (u08 *buff, u32 deluy_time_ms);
void custom_at_handler(u08 *pData);

u16 atoin (u08 *s, u08 n);

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


u08 Counter_Test_Fag = 0; 

int main( void )
{
    xEventsQueue = xQueueCreate(16, sizeof(unsigned char *));

    vSemaphoreCreateBinary(xUart_RX_Semaphore);
	vSemaphoreCreateBinary(xExtSignalStatusSem);
    vSemaphoreCreateBinary(xTimeSendRequestSem);
	

    xI2CMutex = xSemaphoreCreateMutex();


    InitPortsIO();

    StopGetManey();

    RegistratorInit();

	IndicatorInit();

#if MODEM_DBG
Uart0Disable();
Uart0Enable(Uart0_Resiv,  19200);
#endif


////////////////////////////////////////////////////////////////////////////////////////////    

	xTaskCreate(vTask1, (signed char*) "Task_1", configMINIMAL_STACK_SIZE + 60, NULL, 2, NULL);   //60

	xTaskCreate(vTask2, (signed char*) "Task_2", configMINIMAL_STACK_SIZE + 40, NULL, 1, NULL);   //40

    xTaskCreate(vTask3, (signed char*) "Task_3", configMINIMAL_STACK_SIZE + 60, NULL, 1, NULL);   //60

	xTaskCreate(vTask4, (signed char*) "Task_4", configMINIMAL_STACK_SIZE + 70, NULL, 1, NULL);   //70

    xTaskCreate(vTask5, (signed char*) "Task_5", configMINIMAL_STACK_SIZE + 280, NULL, 1, NULL); //280
    
	xTaskCreate(vTask6, (signed char*) "Task_6", configMINIMAL_STACK_SIZE + 80, NULL, 1, NULL);   //80
 
    #if BUZER_TIME    
	xTaskCreate(vTask7, (signed char*) "Task_7", configMINIMAL_STACK_SIZE + 20, NULL, 1, NULL);   //20
    #endif

//    xCoRoutineCreate(vCoRoutineBuzerControll, 1, 0);

	/* Запуск шедулера, после чего задачи запустятся на выполнение. */
	vTaskStartScheduler();

	for( ;; );
return (0);
}

void vTask1( void *pvParameters )
{
    u16 temp_key = 0;


    for( ;; ) {

        ExtSignalStatus = KeySkan(ExtSignalStatus);
		
		if (ExtSignalStatus != temp_key) {
		    temp_key = ExtSignalStatus;
		    xSemaphoreGive(xExtSignalStatusSem);
		}

		vTaskDelay(2 / portTICK_RATE_MS);

#if CHECK_STACK
    DebugBuff[0] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

    }

    vTaskDelete (NULL);
}


void vTask2( void *pvParameters )
{

    static u16 CountManey  = 0;
    static u16 CountWater  = 0;
	static u16 min_counter = 600;

	static u08 indicator_data_buf[8];

	u16 DayOrNightTimer = MINUTES_IN_DAY;

    u16 interval_for_send = 0;

	u16 TimeSendRequestCnt = 0;

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
//		    NoWtrForLCD();
            memset(indicator_data_buf, 10, sizeof indicator_data_buf / sizeof(indicator_data_buf[0]));
			IndicatorDataWrite(indicator_data_buf);
        }
        else {
//		    DecodeForLCD(CountManey, CountWater);
            IndicatorDataConvert(indicator_data_buf, CountManey, CountWater);
			IndicatorDataWrite(indicator_data_buf);
        }


        /*Change price of water*/
 		if (NewPrice) {
		    portENTER_CRITICAL();
            *cost_litre_coef = NewPrice;
			NewPrice = 0;
			portEXIT_CRITICAL();
			
			xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    	    IntEeprWordWrite (CostLitreCoefEEPROMAdr, *cost_litre_coef);
            xSemaphoreGive(xI2CMutex);
		}
		
        if (WtrCntTimer > 0) {
		    portENTER_CRITICAL();
            WtrCntTimer--;
			portEXIT_CRITICAL();
		}

		if (AxellCntTimer > 0) {
		    portENTER_CRITICAL();
	        AxellCntTimer--;
			portEXIT_CRITICAL();
        }

        if (min_counter == 0) {
		    min_counter = 600;

			if (DayOrNightTimer >= MINUTES_IN_DAY) {
			    //DayOrNightTimer = MINUTES_IN_DAY;
//				xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    			DayOrNightTimer = GetRealTime();
//				xSemaphoreGive(xI2CMutex); 
			}

            if ((0 == *lower_report_limit) && (0 == *upper_report_limit)) {
                FlDayOrNight = SMS_FLAG_SEND_DISABLE;
			}
			else if ((DayOrNightTimer >= *lower_report_limit) && (DayOrNightTimer <= *upper_report_limit)) {
                FlDayOrNight = SMS_FLAG_DAY;
			}
			else {
			    FlDayOrNight = SMS_FLAG_NIGHT;
			}
			
            DayOrNightTimer++;

			/*Send report*/
			if (interval_for_send == 0) {
		        interval_for_send = *report_interval;
				if (interval_for_send == 0) {
			        interval_for_send = 1;
				}
				else {
				    if (uxQueueMessagesWaiting(xEventsQueue) < 8) {            /*Check queue for know when with GPRS whots happened */
				        u08 tmp_event = 6;                                     /*and we don't wont to overflow the queue            */
                        xQueueSend(xEventsQueue, &tmp_event, 0);
					}
				}
		    }
			interval_for_send--;
		}
		else {
		    min_counter--;
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
    DebugBuff[1] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK
    }

    vTaskDelete (NULL);
}


void vTask3( void *pvParameters )
{
    u08 rx_data_buff[20];

    xSemaphoreTake(xUart_RX_Semaphore, 0);
	

	for( ;; )
    {

		if (xSemaphoreTake(xUart_RX_Semaphore, 0) == pdTRUE) { 
		
            memset(&rx_data_buff[0], 0x0, 20); 
            memcpy(&rx_data_buff[0], (char *)&BUF_UART_RX[0], 20);
		    memset((char *)&BUF_UART_RX[0], 0x0, 20);
        
		    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
	        GetCmd(&rx_data_buff[0]);
            xSemaphoreGive(xI2CMutex); 
		}
		else if (!IS_SERVICE_MODE && IsRegistratorConnect) {
           
		    RegistratorProcessing(50);
		}


        vTaskDelay(50 / portTICK_RATE_MS);   

#if CHECK_STACK
    DebugBuff[2] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

	}

    vTaskDelete (NULL);
}


void vTask4( void *pvParameters )
{
    static u16 PumpTimeCoef ;
//	u16 get_key_skan = 0;

    static u08 Fl_SellEnable   = 0;
    static u08 Fl_SellStart    = 0;
    static u08 Fl_SellStop	   = 0;
    static u08 Fl_MergeEnable  = 0;

    static u08 Fl_ErrPower	   = 0;
    static u08 Fl_ErrWater	   = 0;
    static u08 Fl_ErrReset	   = 0;
    static u08 Fl_ErrMinWater  = 0;
	static u08 Fl_ErrRsvBill   = 0;

	static u08 Is_Registrator_Err_Gprs_Send = 0;
	
	static u08 Fl_WtrCntrErr   = 0;

    u08 Fl_Ev_NoWater     = 1;          //  the namber of sending by SMS event
    u08 Fl_Ev_NoPower     = 2;
    u08 Fl_Ev_TakeManey   = 3;
    u08 Fl_Ev_GetMoving   = 4;
    u08 Fl_Ev_LimWater    = 5;
//    u08 Fl_Ev_RequestData = 6;
    u08 Fl_Ev_ErrorBill   = 7;
	u08 Fl_Ev_RegError    = 8;
			

    static u08 Sygnal_Get_NoWater;
    static u08 Sygnal_Get_NoWrkBill;
    static u08 Sygnal_Get_NoPower1;
    static u08 Sygnal_Get_NoPower2;
    static u08 Sygnal_Get_Start;
    static u08 Sygnal_Get_Stop;
    static u08 Sygnal_Get_Reset;
    static u08 Sygnal_Get_DoorOpn;
    static u08 Sygnal_Get_BillGet;
    static u08 Sygnal_Get_CoinGet;

    static u08 Fl_ManeyGet = 0;

    static u16 ManeySave = 0;
    static u16 WaterSave = 0;
	static u16 CountRManey = 0;

    static u16 tmp_cnt_pulse = 0;
    
    PumpTimeCoef = *pump_off_time_coef;

	
	static u08 Fl_RegistratorErr = 1;
	static u08 registrator_connect_prev;
	static u08 Fl_Send_Sell_End = 0;
	static u08 Fl_Get_New_Data = 0;

	static u08 is_service_mode;
	
	enum {
	    IDLE_STATE,
		WAIT_INIT,
		SEND_SELL_START,
		SEND_SELL_END,
		SEND_SELL_CANCEL,
		SERVICE_MODE
	} registrator_state;

	enum event_to_ext_eeprom {
	    EV_SAVE_SELL_START = 1,
		EV_SAVE_CASH_COLLECTION = 3,
		EV_SAVE_NO_POWER
	};

	static u08 should_send_to_registrator = 1;
	static RegistratorReceivedData err_data;
	
    registrator_state = WAIT_INIT;

	
	static RegistratorMsg CUWB_RegistratorMsg;
	static RegistratorMsg *pCUWB_RegistratorMsg = &CUWB_RegistratorMsg;

    pCUWB_RegistratorMsg->Data.ProductInfo.Number = 0;
    pCUWB_RegistratorMsg->Data.ProductInfo.Quantity = 0;
    pCUWB_RegistratorMsg->Data.ProductInfo.Prise = 0;


    xSemaphoreTake(xTimeSendRequestSem, 0);
	xSemaphoreTake(xExtSignalStatusSem, 0);

    IsRegistratorConnect = 0;
    
    if (RegistratorSaveWater > 0) {     /* The RegistratorSaveWater takes it value by read internal eeprom while initialization of the microcontroller */
	    Fl_Send_Sell_End = 1;
	}

	MoneyToReturn = 0;
	WaterToReturn = 0;

	is_service_mode = ((IS_SERVICE_MODE) ? 1 : 0);

// TODO:   
//    wdt_enable(WDTO_2S);

	for( ;; )
    {
//////////////////////////////////////////////////////////////////////////
 //   wdt_reset();
		
		
///////////////////////////////////////////////////////////////////////////////////////		
    switch (registrator_state) {

	    case WAIT_INIT: {
		     if ( xSemaphoreTake(xTimeSendRequestSem, 0) == pdTRUE ) {
                 
				 if (IsRegistratorConnect && !is_service_mode) {
  		             Uart0Disable();
 		             Uart0Enable(RegistratorCharPut, 9600);

					 registrator_state = SEND_SELL_START;
                 }
		         else {
		             Uart0Disable();
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
	         else if (Fl_Send_Sell_End == 1 && IsRegistratorConnect) {
                 registrator_state = SEND_SELL_END;
	         }
	         else  if (!Fl_ManeyGet && !Fl_SellStart 
			                        && !Fl_SellStop 
			                        && (xSemaphoreTake(xTimeSendRequestSem, 0) == pdTRUE)
									&& IsRegistratorConnect) {

			     registrator_state = SEND_SELL_CANCEL;
			 }

		     break;
		}
		case SEND_SELL_START: {
		     if (should_send_to_registrator) {
				      if ( RegistratorDataSet(RCMD_SELL_START, NULL) ) {
			              should_send_to_registrator = 0; 
			          }
			 }
			 else {

			     switch ( RegistratorStatusGet() ) {
				     case RR_CONNECTION_ERROR: {
	                      Fl_RegistratorErr = 1;

						  break;
				     }
				     case RR_CONNECTION_OK: {
				          					 
					      RegistratorDataGet(&err_data, ERROR_CODE);

					      switch ( RegistratorErrorCode(&err_data) ) {
					  
					          case RR_ERR_NO: {
					               Fl_RegistratorErr = 0;
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
				    	           registrator_state = SEND_SELL_START;

								   break;
					          }
					      }

						  should_send_to_registrator = 1;
						  break;
					 }
					 default:
					      break;
				 }
			 }
		     break;
		}
		case SEND_SELL_END: {
		     
			 if (should_send_to_registrator) {
			     pCUWB_RegistratorMsg->Data.ProductInfo.Number = 0;
                 pCUWB_RegistratorMsg->Data.ProductInfo.Quantity = RegistratorSaveWater * 10;
                 pCUWB_RegistratorMsg->Data.ProductInfo.Prise = *cost_litre_coef;
             
			     if ( RegistratorDataSet(RCMD_SELL_END, (void **) &pCUWB_RegistratorMsg) ) {
                     should_send_to_registrator = 0;
     			 }
	    	 }
			 else {
			     switch ( RegistratorStatusGet() ) {
				     case RR_CONNECTION_ERROR: {
                          Fl_RegistratorErr = 1;

						  break;
                     }
					 case RR_CONNECTION_OK: {
                          RegistratorDataGet(&err_data, ERROR_CODE);

					      switch ( RegistratorErrorCode(&err_data) ) {
					  
					          case RR_ERR_NO: {
					               Fl_Send_Sell_End = 0;
					               Fl_RegistratorErr = 0;

						           RegistratorSaveWater = 0;
					               if (IntEeprDwordRead(RegistratorWaterEEPROMAdr) != 0)   
								       xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    	      	                       IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
                                       xSemaphoreGive(xI2CMutex);

					               registrator_state = SEND_SELL_START;

								   break;
					          }
					          case RR_ERR_STATE_NOT_RIGHT: {
					               Fl_RegistratorErr = 0;
						           registrator_state = SEND_SELL_START;

								   break;
					          }
					          default: {
					               Fl_RegistratorErr = 1;
				    	           registrator_state = SEND_SELL_START;

								   break;
					          }
						  }

                          should_send_to_registrator = 1;
					      break;
				     }
					 default:
					      break;
				 }
			 }
		     break;
		}
		case SEND_SELL_CANCEL: {
			 
			 if (should_send_to_registrator) {
#if CHECK_STACK
			     //unsigned portBASE_TYPE uxTaskGetStackHighWaterMark( xTaskHandle xTask );
                 static u08 i = 0;
                 DebugBuff[3] = uxTaskGetStackHighWaterMark(NULL);
                 pCUWB_RegistratorMsg->Data.OperationNum.Operation = ((i+1) * 1000 + DebugBuff[i++]);
             
                 if(i >= TASK_NUMBER)
                     i = 0;
#else
              pCUWB_RegistratorMsg->Data.OperationNum.Operation = ROPERATION_CANCEL_SELL;
#endif //CHECK_STACK


                 if ( RegistratorDataSet(RCMD_SELL_CANCELL, (void **) &pCUWB_RegistratorMsg) ) {
                     should_send_to_registrator = 0;
     		     }
			 }
			 else {
			     switch ( RegistratorStatusGet() ) {
				     case RR_CONNECTION_ERROR: {
	                      Fl_RegistratorErr = 1;

						  break;
			         }
				     case RR_CONNECTION_OK: {
				          Fl_RegistratorErr = 0;

                          registrator_state = SEND_SELL_START;

                          should_send_to_registrator = 1;
					      break;
					 }
					 default:
					      break;
				 }
			 }		   
			 break;
		}
//		default : {
//		     break;
//		}
	}

///////////////////////////////////////////////////////////////////////////////////////
      
        Fl_SellEnable = 1;	     
	     
		if (xSemaphoreTake(xExtSignalStatusSem, 0) == pdTRUE) {  

            Sygnal_Get_CoinGet   = ExtSignalStatus & 1;
			Sygnal_Get_BillGet   = ((ExtSignalStatus >> 1) & 1);
			Sygnal_Get_NoWater   = ((ExtSignalStatus >> 2) & 1);
            Sygnal_Get_NoPower1  = ((ExtSignalStatus >> 3) & 1);
			Sygnal_Get_NoPower2  = ((ExtSignalStatus >> 4) & 1);
			Sygnal_Get_DoorOpn   = ((ExtSignalStatus >> 5) & 1);
		    Sygnal_Get_Start     = ((ExtSignalStatus >> 6) & 1);
		    Sygnal_Get_Stop      = ((ExtSignalStatus >> 7) & 1);
		    Sygnal_Get_Reset     = ((ExtSignalStatus >> 8) & 1);
		    Sygnal_Get_NoWrkBill = ((ExtSignalStatus >> 9) & 1);

			IsRegistratorConnect = ((ExtSignalStatus >> 10) & 1);
		}

		if (Sygnal_Get_NoPower1 || Sygnal_Get_NoPower2) {

		    Fl_SellEnable = 0;
		    Fl_State_Power = REPORT_FLAG_ERR;            
          
            if (!Fl_ErrPower) {
		        Fl_ErrPower = 1;
			    //Fl_Ev_NoPower = 1;
	
//                if (!is_service_mode) {

                    if (Fl_SellStart || Fl_SellStop || Fl_ManeyGet || Fl_WtrCntrErr) {

                        if (CountPulse > 0) {
						    MoneyToReturn = PulseQuantityToMoney(CountPulse);
				            WaterToReturn = MoneyToWater(MoneyToReturn);

					        xSemaphoreTake(xI2CMutex, portMAX_DELAY);

                            SaveEvent(MoneyToReturn, WaterToReturn, EV_SAVE_NO_POWER);             /* save data to external eeprom */ 

			                xSemaphoreGive(xI2CMutex);

							                                                       /* all flags sets that same method as the end of sell */
							portENTER_CRITICAL();
                            CountPulse = 0; 
		                    portEXIT_CRITICAL();
                        }
						else if (IsDataToReturnSent == 1) {
                            IsDataToReturnSent = 0;						    

						    MoneyToReturn = 0;
						    WaterToReturn = 0;
						}


						if (Fl_ManeyGet && ManeySave > 0) {
					
			                *day_maney_cnt += ManeySave;

				            WaterSave = MoneyToWater(ManeySave);

				            RegistratorSaveWater += WaterSave;                                     /* set data to transmit to registrator */

                            if (*amount_water <= WaterSave) {                                      /* how many water left in the barrel */
			                    *amount_water = 0;
			                }
			                else {
		                        *amount_water -= WaterSave;
			                }


							xSemaphoreTake(xI2CMutex, portMAX_DELAY);

			                IntEeprDwordWrite(DayManeyCntEEPROMAdr, *day_maney_cnt);

			                IntEeprDwordWrite(AmountWaterEEPROMAdr, *amount_water);

                            if (!is_service_mode) {
                                IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
								Fl_Get_New_Data = 1;
                            }					        

                            xSemaphoreGive(xI2CMutex);
                            
							ManeySave = WaterSave = 0;

							Fl_ManeyGet = 0;
		                }
		            }
//				}
				
				/*xQueueSend(xEventsQueue, &Fl_Ev_NoPower, 0);*/
				xQueueSendToFront(xEventsQueue, &Fl_Ev_NoPower, 0);
            }
	    }
        else {
	        Fl_State_Power = REPORT_FLAG_OK;
	        Fl_ErrPower = 0;
	    }
			
    	if (Sygnal_Get_NoWater && (!Fl_SellStop)) {
 
    		Fl_SellEnable = 0;                                        
            Fl_State_Water = REPORT_FLAG_ERR;

            if (!Fl_ErrWater) {
			    gFlLedStateWater = 1;
                Fl_ErrWater = 1;
		        //Fl_Ev_NoWater = 1;
				xQueueSend(xEventsQueue, &Fl_Ev_NoWater, 0);
            }
	    }
        else {
		    gFlLedStateWater = 0;
	        Fl_ErrWater = 0;
		    Fl_State_Water = REPORT_FLAG_OK;
	    }

		 /* if water counter don't count, we can't sell the water */ 
		if (Fl_WtrCntrErr) {
//		    KLAPAN1_ON;

		    Fl_SellEnable = 0;
		}
		else {
//portENTER_CRITICAL();
//		    KLAPAN1_OFF;
//portEXIT_CRITICAL();
	    }


        if ((Fl_RegistratorErr || !IsRegistratorConnect) && !is_service_mode) {
		    if (!Is_Registrator_Err_Gprs_Send && (Tmr_For_Init_Rr == 0)) {
		        xQueueSend(xEventsQueue, &Fl_Ev_RegError, 0);
			    Is_Registrator_Err_Gprs_Send = 1;
			}
		}
		else if (Is_Registrator_Err_Gprs_Send) {
			Is_Registrator_Err_Gprs_Send = 0;
		}


		if (!is_service_mode && Fl_Send_Sell_End && !Fl_RegistratorErr && !Fl_SellStop) {  
		    Fl_SellEnable = 0;
		}


		if ((CountRManey >= 1000) || (!Fl_SellEnable) 
	                              || Fl_ErrReset 
				    			  || Sygnal_Get_NoWater
								  || Fl_WtrCntrErr) {
		    StopGetManey();
	    }
		else if (!is_service_mode && (Fl_RegistratorErr || (Fl_SellStop && Fl_Send_Sell_End)
								                       || !IsRegistratorConnect)) {
		    StopGetManey();
		}
	    else {
		    if (!Fl_SellStart) {
			    StartGetManey();
		    }
	        else {
		        Fl_SellEnable = 0;
		    }
	    }

	
	    if (Sygnal_Get_CoinGet || Sygnal_Get_BillGet) {
            		
            Fl_ManeyGet = 1;

			if (Sygnal_Get_CoinGet) {
			    Sygnal_Get_CoinGet = 0;
	    	    CountRManey += 25;
		        ManeySave += 25;
            }
			else {
    			Sygnal_Get_BillGet = 0;
		        CountRManey += 100;
		        ManeySave += 100;
			}

			portENTER_CRITICAL();
            CountPulse = MoneyToPulse(CountRManey);
		    portEXIT_CRITICAL();
	    }
	

	    if (!Fl_ManeyGet) {
            CountRManey = PulseQuantityToMoney(CountPulse);
		}
		

	    if (Sygnal_Get_Start && !Sygnal_Get_Stop 
		                     && Fl_SellEnable 
							 && !Fl_SellStart
							 && CountPulse) {
         
		    StopGetManey();
		    Fl_ManeyGet = 0;
			
            if (ManeySave > 0) {

			    *day_maney_cnt += ManeySave;
				WaterSave = MoneyToWater(ManeySave);
				
				RegistratorSaveWater += WaterSave;                                      /* set data to transmit to registrator */

			    if (*amount_water <= WaterSave) {
			        *amount_water = 0;
			    }
			    else {
		            *amount_water -= WaterSave;
			    }

                xSemaphoreTake(xI2CMutex, portMAX_DELAY);
		       
			    SaveEvent(ManeySave, WaterSave, 1);
			    
				IntEeprDwordWrite(DayManeyCntEEPROMAdr, *day_maney_cnt);

			    IntEeprDwordWrite(AmountWaterEEPROMAdr, *amount_water);

                if (!is_service_mode) {
				    IntEeprDwordWrite(RegistratorWaterEEPROMAdr, RegistratorSaveWater);
					Fl_Get_New_Data = 1;
				}

                xSemaphoreGive(xI2CMutex);

				ManeySave = WaterSave = 0;
		    }

            if (PumpTimeCoef <= (*pump_on_time_coef)) {
                PumpTimeCoef = 0;
            }
		    else {
		        PumpTimeCoef = (PumpTimeCoef - (*pump_on_time_coef));
		    }
				
	        SellingStart();
		    Fl_SellStop = 0;
		    Fl_SellStart = 1;

            portENTER_CRITICAL();
        	WtrCntTimer = CHECK_COUNTER_PERIOD;  /* Starting the timer for check does the counter of water connected or worck */    
            portEXIT_CRITICAL();
            tmp_cnt_pulse = CountPulse;          
	    }

	    
        if (Sygnal_Get_Stop && (!(Sygnal_Get_Start) || Fl_SellStart)) {
	        
			SellingStop();

		    Fl_SellStart = 0;
		    Fl_SellStop  = 1;
	    }


	    if ((CountPulse <= PumpTimeCoef) && (Fl_SellStart || Fl_SellStop)) {

		    SellingStop();

		    Fl_SellStart = 0; 
		    Fl_SellStop = 0; 
            CountRManey = 0;	

            portENTER_CRITICAL();
            CountPulse = 0;
		    portEXIT_CRITICAL();

		    PumpTimeCoef = *pump_off_time_coef;
    	}


	    if (Sygnal_Get_Reset) {
       
	        if (!Fl_ErrReset) { 
                StopGetManey();
		        Fl_ErrReset = 1;
			    //Fl_Ev_TakeManey = 1;
				xQueueSend(xEventsQueue, &Fl_Ev_TakeManey, 0);

                u16 dattaH;
		        u16 dattaL;
		        dattaH = (u16) ((*day_maney_cnt) >> 16);
		        dattaL = (u16) ((*day_maney_cnt) & 0x0000FFFF);
             
			    xSemaphoreTake(xI2CMutex, portMAX_DELAY);
		        SaveEvent(dattaH, dattaL, 3);
                xSemaphoreGive(xI2CMutex);

				xSemaphoreTake(xI2CMutex, portMAX_DELAY);
				CollectoinCountManey += *day_maney_cnt;
                *day_maney_cnt = 0;
    			IntEeprDwordWrite(CollectionManeyEEPROMAdr, CollectoinCountManey);
				IntEeprDwordWrite(DayManeyCntEEPROMAdr, *day_maney_cnt);
                xSemaphoreGive(xI2CMutex);
                                 
                BUZZER_ON;
                vTaskDelay(200 / portTICK_RATE_MS);	
    	        BUZZER_OFF;
                
				#if BUZER_TIME
				if (*amount_water < *max_size_barrel) {
    			    buzer_flag = 1;
				}
				#endif
		    }
        }
        else {
	        Fl_ErrReset = 0;
	    }


	    if (Sygnal_Get_DoorOpn && !Fl_MergeEnable) {
		    if (!Fl_SellStart && !Fl_SellStop && !(Sygnal_Get_Reset) && !Fl_WtrCntrErr) {
	
	    		Fl_MergeEnable = 1;
	            SellingStart();
	        }
        }


	    if (Sygnal_Get_DoorOpn && !Fl_MergeEnable && Sygnal_Get_Reset && !Fl_WtrCntrErr) {

		   	Fl_MergeEnable = 1;
            xSemaphoreTake(xI2CMutex, portMAX_DELAY);
 		    IntEeprDwordWrite (AmountWaterEEPROMAdr, *max_size_barrel);
            xSemaphoreGive(xI2CMutex);		
		    *amount_water = *max_size_barrel;

			BUZZER_ON;
            vTaskDelay(100 / portTICK_RATE_MS);
		    BUZZER_OFF;

            #if BUZER_TIME
    		buzer_flag = 0;
			#endif
        }


	    if (!Sygnal_Get_DoorOpn && Fl_MergeEnable) {
		    SellingStop();
			Fl_MergeEnable = 0;
	    }


        #if BUZER_TIME
		if (buzer_flag && Sygnal_Get_Stop)
   	        buzer_flag = 0;
		#endif



        if (Fl_SellStart && !IS_COUNTER_WATER_NOT_ACTIVE && WtrCntTimer == 0) {
		
		    if (tmp_cnt_pulse == CountPulse) {
      		   	SellingStop();
		        Fl_SellStart = 0; 
			    Fl_WtrCntrErr = 1;
			}
			else {
			    tmp_cnt_pulse = CountPulse;
			    //portENTER_CRITICAL();
        	    WtrCntTimer = CHECK_COUNTER_PERIOD;  /* Starting the timer for check does the counter of water connected or worck */    
                //portEXIT_CRITICAL();
			}
		}


        if (Fl_WtrCntrErr) {
		    if (Fl_State_WtrCnt != REPORT_FLAG_ERR) {
			    Fl_State_WtrCnt = REPORT_FLAG_ERR;
			}
		}
		else if (Fl_State_WtrCnt != REPORT_FLAG_OK) {
		    Fl_State_WtrCnt = REPORT_FLAG_OK;
		}


		if (Fl_WtrCntrErr && Sygnal_Get_DoorOpn) {
		    
			Fl_WtrCntrErr = 0;
	    }


		if (Fl_Get_New_Data && !Fl_SellStart) {                                /* When sell finneshed or pause the quantity of water send to registrator */
			    Fl_Get_New_Data = 0;
                Fl_Send_Sell_End = 1;
	    }


/////////// Get signal from axelerometr //////////////////////////////////////
	    if (Sygnal_Get_Axellerometr) { 
            Sygnal_Get_Axellerometr = 0;
			
			if (AxellCntTimer == 0) {
			    AxellCntTimer = ACCELEROMETR_PERIOD;
                xQueueSend(xEventsQueue, &Fl_Ev_GetMoving, 0);
            }
        }
    
/////// the sygnall set when bill can't get maney ////////////////////////////

        if (Sygnal_Get_NoWrkBill && *board_version) {            // If board version the first we

	        if (!Fl_ErrRsvBill) {                                // can't get the right status 
			    Fl_State_RsvBill = REPORT_FLAG_ERR;
				Fl_ErrRsvBill = 1;

				if (uxQueueMessagesWaiting(xEventsQueue) < 8)
                    xQueueSend(xEventsQueue, &Fl_Ev_ErrorBill, 0);
            }
		}                                                          // of the bill receiver   
        else if (Fl_ErrRsvBill) {
	        Fl_ErrRsvBill = 0;
			Fl_State_RsvBill = REPORT_FLAG_OK;
	    }

//  ////////////////////////////////////////////////////////////////////////////////
 
        if ((*amount_water <= ((u32)*sms_water_level))) {
	        if (!Fl_ErrMinWater) {
				xQueueSend(xEventsQueue, &Fl_Ev_LimWater, 0);
	            Fl_ErrMinWater = 1;
            }
	    }
        else {
	        Fl_ErrMinWater = 0;	
	    }

//  ////////////////////////////////////////////////////////////////////////////////
    
//vTaskDelay(10 / portTICK_RATE_MS);

    }

    vTaskDelete (NULL);
}


void vTask5( void *pvParameters )
{

    u08 send_data_buff[60];
	u08 Script_Name[16];
	u08 Password[10];
	u08 Dns_Name[30];
	u08 Server_Name[30];
	u08 num_event = 0;

	u08 err_conn_cnt = 0;
	u08 disconnect_count = 0; 
	u08 err_conn_fail = 0;
	
	u08 mashines_namber[6];
//	u08 tmp_net_quality = 1;          

    u08 *p_data_len = 0;      

	Fl_State_Water   = REPORT_FLAG_OK; 
	Fl_State_Power   = REPORT_FLAG_OK; 
	Fl_State_RsvBill = REPORT_FLAG_OK; 
	Fl_State_WtrCnt  = REPORT_FLAG_OK;

    typedef struct {
        u16 Interval;
	    CARRENT_STATE_CARRENT State_Change;
    } GSM_WAIT_STRUCT;

	GSM_WAIT_STRUCT GSM_Timer;


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
                     while (ModemStatus() == 0);

                     Uart1Enable();		
                     vTaskDelay(2 / portTICK_RATE_MS);				   

					 GSM_Timer.State_Change = STATE_MODEM_FIRST_INIT;
					 GSM_Timer.Interval = 6000;  //23.09.2012 //6000
					 CARRENT_STATE = STATE_SOME_WAIT;
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

                 ModemSendCom(CLEAR_SMS, 1000);

//                 ModemSendCom(CLEAR_ALL_SMS, 1000); //23.09.2012 - comented this, becouse it not work

                 CARRENT_STATE = STATE_GPRS_CONNECT;

			     break;
			} 

			case STATE_GPRS_CONNECT: {
#if MODEM_DBG
			uartSendByte(0, '5');
            uartSendByte(0, '\n');
#endif
                 u08 data_len = 0; 
				 ModemSendCom(SET_GPRS_FORMAT, 500);

				 ModemSendCom(CONNECT_DOMAIN_NAME, 500);

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
				 send_data_buff[data_len] = '\n';
				 send_data_buff[data_len+1] = 0;

				 if (ModemSendData((char *)&send_data_buff[0], 1000) == ACK_OK) {

				     CARRENT_STATE = STATE_GPRS_FORMED_BUFF;

				     if (ModemSendCom(CONNECT_STACKS, 1000) != ACK_OK) {
                         //CARRENT_STATE = STATE_GPRS_DEACTIVATE; 
				     }	
                     if (ModemSendCom(ACTIVEATE_FGCNT, 10000) != ACK_OK) {
                         //CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				     }
					 ModemSendCom(QUERY_IP, 500);                  // add 23.09.2012, becouse without this command modem
                                                                   // won't connecteeng to the server
				 }
				 else {
				     GSM_Timer.State_Change = STATE_GPRS_CONNECT;
					 GSM_Timer.Interval = 100; 
					 CARRENT_STATE = STATE_SOME_WAIT;
						     
				 }
				 
				 if (err_conn_cnt == 3) {
				     err_conn_cnt = 0;
					 //CARRENT_STATE = STATE_MODEM_OFF;
				     CARRENT_STATE = STATE_NET_STATUS;			 
				 }
				 else {
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

			case STATE_GPRS_OPEN: {
#if MODEM_DBG
			uartSendByte(0, '7');
			uartSendByte(0, '\n');
#endif

			//     vTaskDelay(10000 / portTICK_RATE_MS);
                 
				 GSM_Timer.State_Change = STATE_GPRS_DEACTIVATE;
				 GSM_Timer.Interval = 2000; 
				 CARRENT_STATE = STATE_SOME_WAIT;

				 ModemSendCom(CONNECT_TO_SERVER, 3);
                 
//    			 strcpy(Server_Name, "\"92.49.248.31\",10050\0");
//               uartSendBuffer(0, &Server_Name[0], 21);

                 ModemSendData((char *)&Server_Name[0], 2000);
                 
     			 //ModemSendData((char *)&send_data_buff[0], 1000);
                 
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
				     CARRENT_STATE = STATE_SMS_PREPARE;
				 }
				 break;
			}

            case STATE_GPRS_FORMED_BUFF: {

                if (xQueueReceive(xEventsQueue, &num_event, 200 / portTICK_RATE_MS) == pdPASS) {
				    
					CARRENT_STATE = STATE_GPRS_CHECK;
                } 
			    break;
			}

            case STATE_SMS_PREPARE: {
#if MODEM_DBG
			uartSendByte(0, '9');
			uartSendByte(0, '\n');
#endif
                 CARRENT_STATE = STATE_GPRS_FORMED_BUFF; 

                 if (FlDayOrNight == SMS_FLAG_DAY) { 
    			     if ((num_event == 2) || (num_event == 7)) {
	    			     //    CARRENT_STATE = STATE_SMS_SEND_DATA;
					 }
				 }
				 else if (FlDayOrNight == SMS_FLAG_NIGHT) {
                     if ((num_event == 2) || (num_event == 3)) {
    				     CARRENT_STATE = STATE_SMS_SEND_DATA;
					 }
				 }	     
                 break;
			}

            case STATE_SMS_SEND_DATA: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '0');
			uartSendByte(0, '\n');
#endif

    		     vTaskDelay(200 / portTICK_RATE_MS);

                 ModemSendCom(SEND_SMS, 1);
				 ModemSendPfoneNum();

				 if (ModemSendData("\r", 500) == ACK_CAN_SEND) {

                     itoan(*vodomat_number, mashines_namber, 4);
	                 mashines_namber[4] = '-';
	                 mashines_namber[5] = 0;
					 ModemSendData((char *)mashines_namber, 1);
					 ModemSendCom((char*)pgm_read_word(&(SMS_TEXT[num_event])), 2000);
					 
					 vTaskDelay(200 / portTICK_RATE_MS);
//                     num_event = 0;
                 } 

                 CARRENT_STATE = STATE_GPRS_FORMED_BUFF;               
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
                     Create_Report_String(&send_data_buff[0], num_event);
                     xSemaphoreGive(xI2CMutex);  

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
                     GSM_Timer.State_Change = STATE_SMS_PREPARE;
                     GSM_Timer.Interval = 500;

					 if (ModemSendCom(Conn, 5000) == ACK_SEND_OK) {

						 vTaskDelay(10000 / portTICK_RATE_MS);
					 }
			     } 
                 else {
				     CARRENT_STATE = STATE_GPRS_DEACTIVATE;
				 } 
                 break;
			}

			case STATE_GPRS_DISCONNECT: {
#if MODEM_DBG
			uartSendByte(0, '1');
			uartSendByte(0, '2');
			uartSendByte(0, '\n');
#endif
				 
				 ModemSendCom(DISCONNECT_SESSION, 500);
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

				     GSM_Timer.State_Change = STATE_GPRS_CONNECT;
				     GSM_Timer.Interval = 100; 
				     CARRENT_STATE = STATE_SOME_WAIT;
				 				 
				     ModemSendCom(DISCONNECT_GPRS, 500);
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

#if CHECK_STACK
    DebugBuff[4] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

	}

    vTaskDelete (NULL);
}


void vTask6( void *pvParameters )
{

//	u08 com_buff[200];
    u08 com_buff[30];
	u08 cnt = 0;

    CARRENT_STATE = STATE_MODEM_ON;

	BUZZER_ON;
    vTaskDelay(100 / portTICK_RATE_MS);
	BUZZER_OFF;

	for( ;; )
    {
        if (GSM_RxBuf_Count_Get()) {
	     
		    com_buff[cnt] = GSM_RxBuf_Char_Get();


#if MODEM_DBG
			uartSendByte(0, com_buff[cnt]);
			uartSendByte(0, '\n');
#endif

		    if ((com_buff[cnt] == '\n') || (com_buff[cnt] == '>')) {
		        cnt = 0; 
				custom_at_handler(&com_buff[0]);
		    }
			else {
			    cnt++;
				if(cnt == 30)
				    cnt = 0;
			}
		}

        vTaskDelay(10 / portTICK_RATE_MS);

#if CHECK_STACK
    DebugBuff[5] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK

    }

    vTaskDelete (NULL);
}


#if BUZER_TIME
void vTask7 (void *pvParameters)
{
    u16 buzer_timer = BUZER_TIME;

    for( ;; )
    {
#if 1
		if (buzer_flag == 1) {
		    BUZZER_ON;
            vTaskDelay(300 / portTICK_RATE_MS);
		    BUZZER_OFF;
	        vTaskDelay(1700 / portTICK_RATE_MS);
        
		    if (buzer_timer == 0) {
	        
			    buzer_flag = 0;
			    buzer_timer = BUZER_TIME;
            }
            else {
		        buzer_timer--;
		    }
		}
		else {
		    vTaskDelay(2000 / portTICK_RATE_MS);
		}
#else
    vTaskDelay(2000 / portTICK_RATE_MS);
#endif 

#if CHECK_STACK
    DebugBuff[6] = uxTaskGetStackHighWaterMark(NULL);
#endif //CHECK_STACK
        
	}

    vTaskDelete (NULL);
}
#endif


/*
void vCoRoutineBuzerControll (xCoRoutineHandle xHandle, unsigned portBASE_TYPE uxIndex)
{
    static u16 buzer_timer = BUZER_TIME;

    crSTART(xHandle);
    
    for( ;; ) {

		if (buzer_flag == 1) {
		    BUZZER_ON;
            crDELAY(xHandle, 300 / portTICK_RATE_MS);
		    BUZZER_OFF;
	        crDELAY(xHandle, 1700 / portTICK_RATE_MS);
        
		    if (buzer_timer == 0) {
	        
			    buzer_flag = 0;
			    buzer_timer = BUZER_TIME;
            }
            else {
		        buzer_timer--;
		    }
		}
		else {
		    crDELAY(xHandle, 2000 / portTICK_RATE_MS);
		}
    }

    crEND();
}
*/

/////////////////////////////////////////////////////////////////////////////////////

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


void custom_at_handler(u08 *pData)
{
    
    if (strncmp((char *)pData, "Call Ready", 10) == 0) {
		CARRENT_STATE = STATE_GPRS_CONNECT;
    }
	else if (strncmp((char *)pData, ">", 1) == 0) {
        ModemAnsver = ACK_CAN_SEND;    
	}
	else if (strncmp((char *)pData, "200 OK", 6) == 0) {
        
	}
	else if (strncmp((char *)pData, "OK", 2) == 0) {
        ModemAnsver = ACK_OK;
	}
	else if (strncmp((char *)pData, "ERROR", 5) == 0) {
        ModemAnsver = ACK_ERROR;
	}
    else if (strncmp((char *)pData, "SEND OK", 7) == 0) {
        ModemAnsver = ACK_SEND_OK;
	}
	else if (strncmp((char *)pData, "SEND FAIL", 9) == 0) {
        ModemAnsver = ACK_SEND_FAIL;
	}

	else if (strncmp((char *)pData, "CONNECT OK", 10) == 0) {
        CARRENT_STATE = STATE_GPRS_SEND_DATA;
	}
	else if (strncmp((char *)pData, "STATE: CONNECT OK", 17) == 0) {
        CARRENT_STATE = STATE_GPRS_SEND_DATA;
	}
	else if (strncmp((char *)pData, "ALREADY CONNECT", 15) == 0) {
        CARRENT_STATE = STATE_GPRS_SEND_DATA;
	}
	else if (strncmp((char *)pData, "CLOSE OK", 8) == 0) {
        CARRENT_STATE = STATE_SMS_PREPARE;
	}
	else if (strncmp((char *)pData, "CONNECT FAIL", 12) == 0) {
        CARRENT_STATE = STATE_GPRS_FAIL;
	}
	else if (strncmp((char *)pData, "DEACT OK", 8) == 0) {
        CARRENT_STATE = STATE_GPRS_CONNECT;
	}
    else if (strncmp((char *)pData, "REQUEST DATA", 12) == 0) {
  		u08 tmp_event = 6;
        xQueueSend(xEventsQueue, &tmp_event, 0);
	}
	else if (strncmp((char *)pData, "CLOSED", 6) == 0) {
		CARRENT_STATE = STATE_SMS_PREPARE;
	}
	else if (strncmp((char *)pData, "+CSQ:", 5) == 0) {
        u08 *p;
        p = (u08 *)(strstr((char *)pData, "+CSQ:") + 6);

		if (p != NULL) {
		    *p = (u08) atoin(p, 2);

            if (*p == 99 || *p < 8) { 
		        CARRENT_STATE = STATE_NET_QUALITY_LOW;
		    }
		    else {
		        CARRENT_STATE = STATE_GPRS_CONNECT;    //
		    }
		}     
    }
	else if (strncmp((char *)pData, "Price=", 6) == 0) {

        u08 *p;
        u16 price = 0;
        p = (u08 *)(strstr((char *)pData, "Price=") + 6);
  
        if (p != NULL) {
		    price = atoin(p, 4);

		    if (*cost_litre_coef != price && 500 >= price) {
			    portENTER_CRITICAL();
                NewPrice = price;
			    portEXIT_CRITICAL();
			}
        }
    }

	
}
/////////////////////////////////////////////////////////////////////////////////////


static inline u16 MoneyToWater (u16 money_quantity) 
{
	if (*cost_litre_coef > 0)
	    return (u16)(((((u32)money_quantity * 200) / (*cost_litre_coef)) + 1) >> 1);

	return 0;
}


static inline u16 MoneyToPulse (u16 money_quantity) 
{
	if ((*cost_litre_coef  > 0) && (*pulse_litre_coef > 0))
//  CountPulse = (u16)((((((money * 1024) / (*cost_litre_coef)) * (*pulse_litre_coef)) >> 15) + 1) >> 1);
        return (u16)(((((((u32)money_quantity << 10) / (*cost_litre_coef)) * (*pulse_litre_coef)) >> 15) + 1) >> 1);
	    
	return 0;
}

static inline u16 PulseQuantityToMoney (u16 pulse_quantity) 
{
	if ((*pulse_litre_coef > 0) && (pulse_quantity > 0))
        return (u16)(((((*cost_litre_coef) * 8388608) / (((*pulse_litre_coef) * 65536) / pulse_quantity)) + 1) >> 1);

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

