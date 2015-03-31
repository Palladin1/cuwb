#ifndef  REGISTRATOR_H
#define  REGISTRATOR_H

#define  R_TIMEOUT_WAIT_ACK      200
#define  R_TIMEOUT_WAIT_CHACK    1000

#define  RCMD_ALLOW    7Ch
#define  RCMD_SELL     7Bh
#define  RCMD_CENSEL   6Bh     

#define  RANSVER_NAK    15h
#define  RANSVER_SYN    16h
#define  RANSVER_AK     14h

#define  RDATA_SOH    01h
#define  RDATA_EOT    04h
#define  RDATA_ENQ    05h
#define  RDATA_ETX    03h
#define  RDATA_SEPARATOR    ';'

#define  SEQ_VALUE_LOWER   0x20
#define  SEQ_VALUE_UPPER   0x7F
#define  SEQ_VALUE_MAX    (SEQ_VALUE_UPPER - SEQ_VALUE_LOWER)

#ifndef u08
typedef unsigned char u08;
#endif
#ifndef u16
typedef unsigned int u16;
#endif
#ifndef u32
typedef unsigned long u32;
#endif

typedef enum REGISTRATOR_STATUS {
            WAIT,    
            ERROR,
            OK
        };

#endif  //REGISTRATOR_H