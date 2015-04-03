#include  "registrator.h"
#include  <stdlib.h>

#include "fifo_buffer.h"
 
#define  R_PWD_LEN             6 
#define  R_ERR_CODE_LEN        4
#define  R_CRC_LEN             4
#define  R_STATUS_LEN          6
#define  R_SEND_DATA_LEN      20
#define  R_RECEIV_DATA_LEN    20

#define  RECEIVE_BUF_MAX    50
 
typedef enum FRAME_STATE {
    FRAME_WAIT,
    FRAME_LEN,
    FRAME_SEQ,
    FRAME_CMD,
    FRAME_ERROR_CODE,
    FRAME_DATA, 
    FRAME_STATUS,
    FRAME_CRC,    
    FRAME_END 
} frame_state;

struct registrator_send_message {
    u08 len;
    u08 seq;
    u08 cmd;
    u08 pwd[R_PWD_LEN];
    u08 data[R_SEND_DATA_LEN];
    u08 data_len;
    u08 bcc[R_CRC_LEN];
};
    
struct registrator_receive_message {
    u08 len;
    u08 seq;
    u08 cmd;
    u08 error_code[R_ERR_CODE_LEN];
    u08 data[R_RECEIV_DATA_LEN];
    u08 data_len;
    u08 status[R_STATUS_LEN];
    u08 bcc[R_CRC_LEN];
};


u08 registrator_frame_get (u08 c);
void registrator_frame_send (void);     
       
u08 increase_seq (u08 cur);
u08 make_data_type_n (u08 *to, u32 d);
u08 make_data_type_m (u08 *to, u32 d);
u08 make_data_type_q (u08 *to, u32 d);
void set_point (u08 *s, u08 pos);

void makecrc (u08 * crc);

void sendnstr (u08 *s, u08 len);


static struct registrator_send_message send_message; 
static struct registrator_receive_message receive_message;

static REGISTRATOR_STATUS registrator_status;
static u32 timer_var;

static u08 should_send_data;

static FIFO_BUFFER RegistratorRXBuffer;
static unsigned char ReceiveBuffer[RECEIVE_BUF_MAX];


void RegistratorCharPut (unsigned char c) {
    FifoBufPut(&RegistratorRXBuffer, c);    
}

void RegistratorInit (void)
{
    u08 i;
    registrator_status = WAIT_CONNECTION;
    should_send_data = 0;
    timer_var = 0;
    
    send_message.cmd = 0;
    send_message.seq = 0;
    send_message.data[0] = '\0';
    
    /* set standart pwd */ 
    for (i = 0; i < R_PWD_LEN; i++) {
       send_message.pwd[i] = '0';
    }
    
    FifoBufInit(&RegistratorRXBuffer, ReceiveBuffer, sizeof ReceiveBuffer / sizeof(ReceiveBuffer[0]));
} 


void RegistratorProcessing (u08 time_correcting)
{
    u08 ans;
    static enum processing_state { 
        P_IDDLE,
        P_REQUEST,
        P_ANSWER
    } state = P_IDDLE;
   
    switch (state) {
        case P_IDDLE: {
             if (should_send_data == 1) {
			     should_send_data = 0;
			     state = P_IDDLE;
                 registrator_status = WAIT_CONNECTION;    
			 }
             break;
        }
        case P_REQUEST: {
             timer_var = 0;
             registrator_frame_send();
             state = P_ANSWER;
             break;
        }
        case P_ANSWER: {
             ans = 0;
             
             while (FifoBufDataCnt(&RegistratorRXBuffer)) {
                 ans = registrator_frame_get(FifoBufGet(&RegistratorRXBuffer));
              
             }
             
             if (ans == RANSVER_SYN) {
                 timer_var = 0;
             }
             else if (ans == RANSVER_NAK) {
                 timer_var = 0;
                 send_message.seq = increase_seq(send_message.seq);
                 state = P_REQUEST;                     
             }
             else if (ans == RANSVER_AK) {
                 timer_var = 0;
                 send_message.seq = increase_seq(send_message.seq);
                 registrator_status = OK_CONNECTION;
                 
                 (should_send_data == 1) ? should_send_data = 0 : 0;
                 state = P_IDDLE;                     
             }
             else if (timer_var == R_TIMEOUT_WAIT) {
                 registrator_status = ERROR_CONNECTION;
                 state = P_REQUEST;    
             }
             
             timer_var++;
             break;
        }
//      default : {
//           break;
//      }
    }
}

REGISTRATOR_STATUS RegistratorStatusGet (void) 
{
    return registrator_status;
}
            

void RegistratorDataSet (u08 cmd, void * data[]) 
{
    u08 offset;
    
//    while (should_send_data)
//        ;
    
    switch (cmd) {
        case RCMD_SELL_START: {
             send_message.cmd = RCMD_SELL_START;
             send_message.data[0] = '\0';
                
             should_send_data = 1;
             break;
        }
        case RCMD_SELL_END: {
             send_message.cmd = RCMD_SELL_END;
             offset = make_data_type_n(&send_message.data[0], *(u32 *)data[0]);
             send_message.data[offset++] = RDATA_SEPARATOR;
             offset += make_data_type_m(&send_message.data[offset], *(u32 *)data[1]);
             send_message.data[offset++] = RDATA_SEPARATOR;
             offset += make_data_type_q(&send_message.data[offset], *(u32 *)data[2]);
             send_message.data[offset++] = RDATA_SEPARATOR;
             send_message.data[offset] = '\0';
             send_message.data_len = offset;
             
                
             should_send_data = 1;
             break;
        }
        case RCMD_SELL_CANCELL: {
             send_message.cmd = RCMD_SELL_CANCELL;
             offset = make_data_type_n(&send_message.data[0], *(u32 *)data[0]);
             send_message.data_len = offset;
             
             should_send_data = 1;
             break;
        }
/*      default : {
            break;
        }
*/        
    }
}


u08 registrator_frame_get (u08 c) 
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
             receive_message.len = CONVERT_TO_DIGIT(c);            
             cur_state = (receive_message.len == 0) ? FRAME_WAIT : FRAME_SEQ;
             break;
        }
        case FRAME_SEQ: {
             crc_cnt += c;
             receive_message.seq = (SEQ_VALUE_IS_CORRECT(c) != 0) ? CONVERT_TO_DIGIT(c) : 0; 
             cur_state = (receive_message.seq == 0) ? FRAME_WAIT : FRAME_CMD;
             break;
        }
        case FRAME_CMD: {
             crc_cnt += c;
             receive_message.cmd = CONVERT_TO_DIGIT(c);
             cur_state = (receive_message.cmd != send_message.cmd) ? FRAME_WAIT : FRAME_ERROR_CODE;
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
             else if (i < R_RECEIV_DATA_LEN) {
                 receive_message.status[i] = c;
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
             else if (i < R_STATUS_LEN) {
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
                 
               u08 temp_crc[R_CRC_LEN+1];
			   memset(temp_crc, receive_message.bcc, R_CRC_LEN);
               temp_crc[R_CRC_LEN] = '\0';
               u16 z;
			   z =atoi(temp_crc);


        //!!!         if (atoi(receive_message.bcc) == (u16)(crc_cnt & 0x0000FFFF)) {
		         if (z == (u16)(crc_cnt & 0x0000FFFF)) {
                     ret = RANSVER_NAK;
                 }
                 cur_state = FRAME_WAIT;
             }
             else if (i < R_CRC_LEN) {
                 receive_message.bcc[i] = c;
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


void registrator_frame_send (void)
{
    u08 c;
    
    send_message.len = 1 + 1 + 1 + R_PWD_LEN + send_message.data_len + 1;
    
    c = RDATA_SOH;
    sendnstr(c, 1);    
    sendnstr(CONVERT_FOR_SEND(send_message.len), 1);
    sendnstr(CONVERT_FOR_SEND(send_message.seq), 1);
    sendnstr(send_message.cmd, 1);    
    sendnstr(send_message.pwd, R_PWD_LEN);
    c = RDATA_SEPARATOR;
    sendnstr(c, 1);
    sendnstr(send_message.data, send_message.data_len);
    c = RDATA_ENQ;
    sendnstr(c, 1);
    sendnstr(send_message.data, send_message.data_len);
    
    makecrc(send_message.bcc);
    sendnstr(send_message.bcc, R_CRC_LEN);
    
    c = RDATA_ENQ;
    sendnstr(c, 1);
}
        
            
u08 increase_seq (u08 cur)
{
    return (cur++ < SEQ_VALUE_MAX) ? cur : 0;  
}     
 

u08 make_data_type_n (u08 *to, u32 d)
{
    ltoa(d, to, 10);
    return strlen(to);
}

u08 make_data_type_m (u08 *to, u32 d)
{
    ltoa(d, to, 10);
    set_point(to, 2);
    return strlen(to);
}

u08 make_data_type_q (u08 *to, u32 d)
{
    u08 len = 0;
    
    ltoa(d, to, 10);
    
    len = strlen(to);
    to[len++] = '0';
    to[len] = '\0';    
    set_point(to, 3);
    len++;
    
    return len;
}

void set_point (u08 *s, u08 pos)
{
    u08 i;
    
    i = 0;
    while (*s != '\0') {
        s++;
        i++;
    }
    
    if (i >= pos)
        while (pos-- > 0)
            *(s+1) = *s--;
    
    *s = '.';    
} 
 
void makecrc (u08 * crc) 
{
    u08 i;
    u32 crc_temp = 0;
    crc_temp = RDATA_SOH + RDATA_SEPARATOR + RDATA_ENQ + RDATA_ETX;
    
    for (i = 0; i < R_PWD_LEN; i++)
        crc_temp += send_message.pwd[i];
        
    for (i = 0; i < send_message.data_len; i++)
        crc_temp += send_message.data[i];
    
    crc_temp &= 0x0000FFFF;

    i = R_CRC_LEN;
    while (i-- > 0) {
        send_message.bcc[i] = crc_temp % 10 + '0';
        crc_temp /= 10;
    }
}

void sendnstr (u08 *s, u08 len)
{
    while (len-- > 0)
        putbyte(*s++);
}


/*
u08 is_should_send_flag (void)
{
    u08 ret = should_send_flag;
    should_send_flag = 0;
    return ret; 
}

void set_should_send_flag (void)
{
    while (should_send_flag)
        should_send_flag = 1;
}
*/
