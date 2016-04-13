#ifndef  REGISTRATOR_H
#define  REGISTRATOR_H

#include  "avrlibtypes.h"


#define  R_TIMEOUT_WAIT    2000


#define  RCMD_SELL_START          0x7C
#define  RCMD_SELL_END            0x7D
#define  RCMD_SELL_CANCELL        0x6B 
#define  RCMD_DATA_TIME_GET       0x21                                    /* Get the date and time without seconds */
#define  RCMD_DATA_TIMES_GET      0x3B                                    /* Get the date and time with seconds */  
#define  RCMD_MODEM_STATUS        0xEA

//#define  RCMD_CASH_GET_PUT        0x77
//#define  RCMD_DAY_REPORT_PRINT    0xA1
#define  RCMD_ADD_EXCLUDE_SUM     0x6E


#define  RANSVER_NAK    0x15
#define  RANSVER_SYN    0x16
#define  RANSVER_AK     0x14

#define  RDATA_SOH    0x01
#define  RDATA_EOT    0x04
#define  RDATA_ENQ    0x05
#define  RDATA_ETX    0x03
#define  RDATA_SEPARATOR    ';'

#define  ROPERATION_CANCEL_SELL      0ul
#define  RZREPORT_WITH_TAPE_RESET    0u
#define  ROPERATOR_NUMBER_DEFAULT    1u


#define  SEQ_VALUE_LOWER            0x20
#define  SEQ_VALUE_UPPER            0x7F
#define  SEQ_VALUE_MAX              (SEQ_VALUE_UPPER - SEQ_VALUE_LOWER)
#define  SEQ_VALUE_IS_CORRECT(v)    (SEQ_VALUE_LOWER <= (v) && (v) <= SEQ_VALUE_UPPER) ? 1 : 0

#define  NULL_IN_MESSAGE    0x20
#define  CONVERT_FOR_SEND(d)       ((d) + NULL_IN_MESSAGE)  
#define  CONVERT_TO_DIGIT(c)       ((c) >= NULL_IN_MESSAGE) ? (c) - NULL_IN_MESSAGE : 0  


typedef enum _REGISTRATOR_STATUS {
   
    RR_CONNECTION_NOT_DEFINED,
	RR_CONNECTION_WAIT_ANSVER,
	RR_CONNECTION_ERROR,
	RR_CONNECTION_OK

} REGISTRATOR_STATUS;

typedef struct {
    u08 *dataptr;
	u08 len;
} RegistratorReceivedData;

typedef enum {
    ERROR_CODE,    
    DATA,
    STATUS
} RECEIVED_DATA_TYPE;


typedef enum {
    RR_ERR_NO              = ('0' + '0' + '0' + '0'),
    RR_ERR_STATE_NOT_RIGHT = ('0' + '0' + '0' + 'B')
    
} REGISTRATOR_ERROR_CODE;
 

struct RegistratorDataFinishSale {
    u32 Number;
    u32 Quantity;
    u32 Price;	
};

struct RegistratorDataCancelSale {
    u32 Operation;
};

//struct RegistratorDataMoneyGetPut {
//    u16 DataCode;
//	s32 Quantity;
//};

struct RegistratorSumAddExclude {
    u16 OperatorNumber;
	s64 Sum;
};

struct RegistratorDataModemStatus {
    u32 IsPrint;
};

//struct RegistratorDataReportPrint {
//    u32 Type;
//};

typedef struct RR_Msg {
    union data {
	    struct RegistratorDataFinishSale  ProductInfo;
		struct RegistratorDataCancelSale  OperationNum;
//		struct RegistratorDataMoneyGetPut Money;
        struct RegistratorDataModemStatus ReportPrint;
		struct RegistratorSumAddExclude AddExcludeSum;
//		struct RegistratorDataReportPrint Report;
	} Data;
} RegistratorMsg;


void RegistratorCharPut (unsigned char c);  
        
void RegistratorInit (void);
void RegistratorProcessing (u08 period);
u08  RegistratorDataGet (RegistratorReceivedData * received_data, RECEIVED_DATA_TYPE datatype);
u08  RegistratorDataSet (u08 cmd, RegistratorMsg * msg);
REGISTRATOR_STATUS RegistratorStatusGet (void);
extern void RegistratorSendStr (u08 *s, u08 len);
REGISTRATOR_ERROR_CODE RegistratorErrorCode (RegistratorReceivedData *rr_err);

#endif  /* REGISTRATOR_H */
