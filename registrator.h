#ifndef  REGISTRATOR_H
#define  REGISTRATOR_H

#define  R_TIMEOUT_WAIT    200
//#define  R_TIMEOUT_WAIT_CHACK    1000

#define  RCMD_SELL_START     0x7C
#define  RCMD_SELL_END       0x7D
#define  RCMD_SELL_CANCELL   0x6B     

#define  RANSVER_NAK    0x15
#define  RANSVER_SYN    0x16
#define  RANSVER_AK     0x14

#define  RDATA_SOH    0x01
#define  RDATA_EOT    0x04
#define  RDATA_ENQ    0x05
#define  RDATA_ETX    0x03
#define  RDATA_SEPARATOR    ';'

#define  SEQ_VALUE_LOWER            0x20
#define  SEQ_VALUE_UPPER            0x7F
#define  SEQ_VALUE_MAX              (SEQ_VALUE_UPPER - SEQ_VALUE_LOWER)
#define  SEQ_VALUE_IS_CORRECT(v)    (SEQ_VALUE_LOWER <= (v) && (v) <= SEQ_VALUE_UPPER) ? 1 : 0

#define  NULL_IN_MESSAGE    0x20
#define  CONVERT_FOR_SEND(d)       ((d) + NULL_IN_MESSAGE)  
#define  CONVERT_TO_DIGIT(c)       ((c) >= NULL_IN_MESSAGE) ? (c) - NULL_IN_MESSAGE : 0  

#ifndef u08
typedef unsigned char u08;
#endif
#ifndef u16
typedef unsigned int u16;
#endif
#ifndef u32
typedef unsigned long u32;
#endif
#ifndef s32
typedef signed long s32;
#endif

typedef enum _REGISTRATOR_STATUS {
            WAIT_CONNECTION,    
            ERROR_CONNECTION,
            OK_CONNECTION
        } REGISTRATOR_STATUS;
  
void RegistratorCharPut (unsigned char c);  
        
void RegistratorInit (void);
void RegistratorProcessing (u08 time_correcting);
void RegistratorDataSet (u08 cmd, void * data[]);
REGISTRATOR_STATUS RegistratorStatusGet (void);
extern void RegistratorCharSend (u08 c);

#endif  //REGISTRATOR_H
