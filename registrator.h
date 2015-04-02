#ifndef  REGISTRATOR_H
#define  REGISTRATOR_H

#define  R_TIMEOUT_WAIT    200
//#define  R_TIMEOUT_WAIT_CHACK    1000

#define  RCMD_SELL_START     7Ch
#define  RCMD_SELL_END       7Dh
#define  RCMD_SELL_CANCELL   6Bh     

#define  RANSVER_NAK    15h
#define  RANSVER_SYN    16h
#define  RANSVER_AK     14h

#define  RDATA_SOH    01h
#define  RDATA_EOT    04h
#define  RDATA_ENQ    05h
#define  RDATA_ETX    03h
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

typedef enum REGISTRATOR_STATUS {
            WAIT_CONNECTION,    
            ERROR_CONNECTION,
            OK_CONNECTION
        };
  
void RegistratorCharPut (unsigned char c);  
        
void RegistratorInit (void);
void RegistratorProcessing (u08 *registrator_receive_buf, u08 time_correcting);
void RegistratorDataSet (u08 cmd, void * data[]);
REGISTRATOR_STATUS RegistratorStatusGet (void);
extern void (* putbyte)(u08);

#endif  //REGISTRATOR_H