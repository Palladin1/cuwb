#ifndef  REGISTRATOR_H
#define  REGISTRATOR_H

#define  REGISTRATOR_TIMEOUT    200

#define  RCMD_ALLOW    7Ch
#define  RCMD_SELL     7Bh
#define  RCMD_CENSEL   6Bh     

#define  RANSVER_NAK    15h
#define  RANSVER_SYN    16h

#define  RDATA_SOH    01h
#define  RDATA_EOT    04h
#define  RDATA_ENQ    05h
#define  RDATA_ETX    03h
#define  RDATA_SEPARATOR    ';'

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