#include  "separator.h"
#include  <stdlib.h>


#define  R_PWD_LEN            6 
#define  R_ERR_CODE_LEN       4
#define  R_CRC_LEN            4
#define  R_STATUS_LEN         6
#define  R_SEND_DATA_LEN     20
#define  R_RECIV_DATA_LEN    20
 

struct registrator_status {
    u08 S0;
    u08 S1;
    u08 S2;
    u08 S3;
    u08 S4;
    u08 S5;
};

struct registrator_send_message {
    u08 len;
    u08 seq;
    u08 cmd;
    u08 pwd[R_PWD_LEN];
    u08 data[R_SEND_DATA_LEN];
    u08 data_len;
    u08 bcc[R_CRC_LEN];
};
    
struct registrator_recive_message {
    u08 len;
    u08 seq;
    u08 cmd;
    u08 error_code[R_ERR_CODE_LEN];
    u08 data[R_RECIV_DATA_LEN];
    u08 data_len;
    u08 status[R_STATUS_LEN];
    u08 bcc[R_CRC_LEN];
};

static struct registrator_send_message send_message; 
static struct registrator_receive_message receive_message;

static REGISTRATOR_STATUS registrator_status;
static u32 timer_var;


void RegistratorInit (void)
{
    u08 i;
    registrator_status = WAIT;
    timer_var = 0;
    
    send_message.cmd = 0;
    send_message.seq = 0;
    send_message.data[0] = '\0';
    
    /* set standart pwd */ 
    for (i = 0; i < R_PWD_LEN; i++) {
       send_message.pwd[i] = '0';
    }
} 


void RegistratorProcessing (u08 time_correcting)
{
    u08 ans;
    static enum processing_state { 
        P_IDDLE,
        P_REQUEST,
        P_CANSELL,
        P_ANSWER
    } state = IDDLE;
   
    timer_var++;
    
    switch (state) {
        case P_IDDLE: {
             if (time_var == R_TIMEOUT_WAIT_CHACK) {
                 send_message.cmd = ALLOW;
                 timer_var = 0;
                 registrator_status = WAIT;
                 state = P_REQUEST;
             }
             break;
        }
        case P_REQUEST: {
             RegistratorFrameSend();
             state = P_ANSWER;
             break;
        }
        case P_ANSWER: {
             cmd = RegistratorFrameGet(xxx);
             if (cmd == RANSVER_SYN) {
                 timer_var = 0;
             }
             else if (cmd = RANSVER_NAK) {
                 timer_var = 0;
                 send_message.seq = increase_seq(send_message.seq);
                 state = P_REQUEST;                     
             }
             else if (cmd = RANSVER_AK) {
                 timer_var = 0;
                 send_message.seq = increase_seq(send_message.seq);
                 state = P_IDDLE;                     
             }
             state = P_ANSWER;
             break;
        }
        case P_REQUEST: {
             RegistratorFrameSend();
             state = P_ANSWER;
             break;
        }
        
             
            
u08 increase_seq (u08 cur)
{
    return (cur++ < SEQ_VALUE_MAX) ? cur : 0;  
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
    
    send_message.data_len = offset;
}

REGISTRATOR_STATUS RegistratorStatusGet (void) 
{
    return registrator_status;
}
    

typedef enum frame_state {
    FRAME_WAIT,
    FRAME_LEN,
    FRAME_SEQ,
    FRAME_CMD,
    FRAME_ERROR_CODE,
    FRAME_DATA, 
    FRAME_STATUS,
    FRAME_CRC,    
    FRAME_END 
};

u08 RegistratorFrameGet (u08 c) 
{
    static frame_state cur_state;
    static u32 crc_cnt;
    static u08 i;
    u08 ret = 0;
    
    switch (c) {
        case RDATA_SOH: {
             cur_state = FRAME_LEN;
             break;
        }
        case RANSVER_NAK: {
             ret = RANSVER_NAK;
             break;
        }
        case RANSVER_SYN: {
             ret = RANSVER_SYN;
             break;
        }
    }

    switch (cur_state) {
        case FRAME_WAIT: {
             break;
        }
        case FRAME_LEN: {
             i = 0;
             crc_cnt = c;
             receive_message.len = (0x20 <= c) ? (c - 0x20) : 0;            
             cur_state = (receive_message.len == 0) ? FRAME_WAIT : FRAME_SEQ;
             break;
        }
        case FRAME_SEQ: {
             crc_cnt += c;
             receive_message.seq = (0x20 <= c && c <= 0x7F) ? (c - 0x20) : 0; 
             cur_state = (receive_message.seq == 0) ? FRAME_WAIT : FRAME_CMD;
             break;
        }
        case FRAME_CMD: {
             crc_cnt += c;
             receive_message.cmd = (0x20 <= c) ? c : 0;
             cur_state = ()
             break;
        }
        case FRAME_ERROR_CODE: {
             crc_cnt += c;
             if (c == RDATA_SEPARATOR) {
                 i = 0;
                 cur_state = FRAME_DATA;
             }
             else if (i < R_ERR_CODE_LEN) {
                 receive_message.error_code[i] = c;
                 i++;
             }
             else {
                 cur_state = FRAME_WAIT;
             }
             break;
        }
        case FRAME_DATA: {
             crc_cnt += c;
             if (c == RDATA_EOT) {
                 receive_message.data_len = i;
                 i = 0;
                 cur_state = FRAME_STATUS;
             }
             else if (i < R_DATA_LEN) {
                 receive_message.status = c;
                 i++;
             }
             else {
                 receive_message.data_len = 0;
                 cur_state = FRAME_WAIT;
             }
             break;
        }
        case FRAME_STATUS: {
             crc_cnt += c;
             if (c == RDATA_ENQ) {
                 i = 0;
                 cur_state = FRAME_CRC;
             }
             else if (i < R_DATA_LEN) {
                 receive_message.status[i] = c;
                 i++;
             }
             else {
                 cur_state = FRAME_WAIT;
             }
             break;
        }  
        case FRAME_CRC: {
             if (c == RDATA_ETX) {
                 i = 0;
                 
                 if (atoin(receive_message.crc) == (u16)(crc_cnt & 0x0000FFFF)) {
                     ret = RANSVER_NAK;;
                 }
                 cur_state = FRAME_WAIT;
             }
             else if (i < R_CRC_LEN) {
                 receive_message.crc[i] = c;
                 i++;
             }
             else {
                 cur_state = FRAME_WAIT;
             }
             break;
        } 
//        default : {
//             break;
//        }
    }
         
    return ret;
}  

void RegistratorFrameSend (void)
{
    u08 c;
    
    send_message.len = 1 + 1 + 1 + R_PWD_LEN + send_message.data_len + 1;
    
    c = RDATA_SOH;
    sendnstr(c, 1);    
    sendnstr(send_message.len + 0x20, 1);
    sendnstr(send_message.seq + 0x20, 1);
    sendnstr(send_message.cmd, 1);    
    sendnstr(send_message.pwd, R_PWD_LEN);
    c = RDATA_SEPARATOR;
    sendnstr(c, 1);
    sendnstr(send_message.data, send_message.data_len);
    c = RDATA_ENQ;
    sendnstr(c, 1);
    sendnstr(send_message.data, send_message.data_len);
    
    makecrc(send_message.crc);
    sendnstr(send_message.crc, R_CRC_LEN);
    
    c = RDATA_ENQ;
    sendnstr(c, 1);
}
        
    
void makecrc (u08 * crc) 
{
    u08 i;
    u32 crc = 0;
    crc = RDATA_SOH + RDATA_SEPARATOR + RDATA_ENQ + RDATA_ETX;
    
    for (i = 0; i < R_PWD_LEN; i++)
        crc += send_message.pwd[i];
        
    for (i = 0; i < send_message.data_len; i++)
        crc += send_message.data[i];
    
    crc &= 0x0000FFFF;

    i = R_CRC_LEN;
    while (i-- > 0) {
        send_message.crc[i] = crc % 10 + '0';
        crc /= 10;
    }
}

extern void (* putbyte)(u08);
void sendnstr (u08 *s, u08 len)
{
    while (len-- > 0)
        (* putbyte)(*s++);
}