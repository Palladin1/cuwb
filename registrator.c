#include  "separator.h"
#include  <stdlib.h>


#define  R_PWD_LEN            6+1 
#define  R_ERR_CODE_LEN       4+1
#define  R_CRC_LEN            4+1
#define  R_SEND_DATA_LEN    120+1
#define  R_RECIV_DATA_LEN    20+1
 

struct registrator_status {
    u08 S0;
    u08 S1;
    u08 S2;
    u08 S3;
    u08 S4;
    u08 S5;
};

struct registrator_send_message {
    u08 soh;
    u08 len;
    u08 seq;
    u08 cmd;
    u08 pwd[R_PWD_LEN];
    u08 data[R_SEND_DATA_LEN];
    u08 enq;
    u08 bcc[R_CRC_LEN];
    u08 etx;
};
    
struct registrator_recive_message {
    u08 soh;
    u08 len;
    u08 seq;
    u08 cmd;
    u08 error_code[R_ERR_CODE_LEN];
    u08 data[R_RECIV_DATA_LEN];
    u08 eot;
    struct registrator_status status;
    u08 enq;
    u08 bcc[R_CRC_LEN];
    u08 etx;
};

static struct registrator_send_message send_message; 
static struct registrator_receive_message receive_message;

static REGISTRATOR_STATUS registrator_status;
static u32 timer_var;


void RegistratorInit (void)
{
    int i;
    
    registrator_status = WAIT;
    timer_var = 0;
    
    send_message.soh = RDATA_SOH;
    send_message.enq = RDATA_ENQ;
    send_message.etx = RDATA_ETX;
} 

void RegistratorDataSet (u08 cmd, u32 data[], u08 ndata) 
{
    u08 i, offset;
    
    offset = 0;
    
    for (i = 0; i < ndata; i++) {
        itoa(data[i], send_message.data[offset], 10);  
        offset += strlen(send_message.data);
        send_message.data[offset] = RDATA_SEPARATOR;
        offset++;
    }
    
    send_message.data[offset] = '\0';
}

REGISTRATOR_STATUS RegistratorStatusGet (void) 
{
    return registrator_status;
}
    

void crc (u08 * crc, u32 d) 
{
    u32 crc_cnt = 0;
}