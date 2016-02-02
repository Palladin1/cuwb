#include  "registrator.h"
#include  <stdlib.h>
#include  <string.h>

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
    FRAME_CRC    
    //FRAME_END 
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
void RegistratorStatusSet (REGISTRATOR_STATUS status_cur);   
       
u08 increase_seq (u08 cur);
u08 make_data_type_n (u08 *to, u32 d);
u08 make_data_type_m (u08 *to, s32 d);
u08 make_data_type_q (u08 *to, u32 d);
u08 set_point (u08 *s, u08 pos, u08 len_cur);

void makecrc (u08 * crc, u16 bcc_cnt);

void sendnstr (u08 *s, u08 len);


static struct registrator_send_message send_message; 
static struct registrator_receive_message receive_message;

static REGISTRATOR_STATUS RegistratorConnectionStatus;

static u08 should_send_data;

static FIFO_BUFFER RegistratorRXBuffer;
static unsigned char ReceiveBuffer[RECEIVE_BUF_MAX];


void RegistratorCharPut (unsigned char c) {
    FifoBufPut(&RegistratorRXBuffer, c);    
}

void RegistratorInit (void)
{
    u08 i;
	RegistratorStatusSet (RR_CONNECTION_NOT_DEFINED);
    should_send_data = 0;
    
    send_message.cmd = 0;
    send_message.seq = 0;
    send_message.data[0] = '\0';
    
    /* set standart pwd */ 
    for (i = 0; i < R_PWD_LEN; i++) {
       send_message.pwd[i] = '0';
    }
    
    FifoBufInit(&RegistratorRXBuffer, ReceiveBuffer, sizeof ReceiveBuffer / sizeof(ReceiveBuffer[0]));
} 


REGISTRATOR_STATUS RegistratorStatusGet (void)
{
    return RegistratorConnectionStatus;
}

void RegistratorStatusSet (REGISTRATOR_STATUS status_cur)
{
    RegistratorConnectionStatus = status_cur;
}

void RegistratorProcessing (u08 period)
{
    u08 ans;
	static u32 timer_var;
    static enum processing_state { 
        P_IDDLE,
        P_REQUEST,
        P_ANSWER
    } state = P_IDDLE;
   
    switch (state) {
        case P_IDDLE: {
             if (should_send_data == 1) {
			     should_send_data = 0;
			     state = P_REQUEST;
//				 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);   
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
             
             while (ans == 0 && FifoBufDataCnt(&RegistratorRXBuffer)) {
                 ans = registrator_frame_get(FifoBufGet(&RegistratorRXBuffer));
             }
             
			 switch (ans) {
			     case RANSVER_SYN: {
				      timer_var = 0;
				      break;
				 }
				 case RANSVER_NAK: {
				      timer_var = 0;
                      send_message.seq = increase_seq(send_message.seq);
                      state = P_REQUEST;                     
                      break;				 
				 }
				 case RANSVER_AK: {
                      timer_var = 0;
                      send_message.seq = increase_seq(send_message.seq);

					  RegistratorStatusSet(RR_CONNECTION_OK);
                 
//                    (should_send_data == 1) ? should_send_data = 0 : 0;
                      state = P_IDDLE;                     				 
				      break;
			     }
//				 default : {
//                      break;
//				 } 
			 }
			 
			 if (timer_var >= R_TIMEOUT_WAIT) {
               
			     state = P_REQUEST;    

				 RegistratorStatusSet(RR_CONNECTION_ERROR);
             }
             
             timer_var += period;
        }
//      default : {
//           break;
//      }
    }
}


u08 RegistratorDataGet (RegistratorReceivedData * received_data, RECEIVED_DATA_TYPE datatype)
{
    switch (datatype) {
        case ERROR_CODE: {
		     received_data->dataptr = receive_message.error_code;
			 received_data->len = R_CRC_LEN; 
		     break;
		}    
        case DATA: {
		     received_data->dataptr = receive_message.data;
		     received_data->len = receive_message.data_len;
		     break;
		}
        case STATUS: {
		     received_data->dataptr = receive_message.status;
		     received_data->len = R_STATUS_LEN;
			 break;
		}
	}

    return (1);		
}
			
u08 RegistratorDataSet (u08 cmd, void *data[]) 
{
    u08 offset;
    
    if ( (RegistratorStatusGet() != RR_CONNECTION_NOT_DEFINED) && (RegistratorStatusGet() != RR_CONNECTION_OK) ) {
	    return (0);
	}

	send_message.cmd = cmd;
    
    switch (cmd) {
        case RCMD_SELL_START: {
//             send_message.cmd = RCMD_SELL_START;
			 send_message.data_len = 0;
                
             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
        case RCMD_SELL_END: {
 //            send_message.cmd = RCMD_SELL_END;

			 if (data != NULL) {
                 offset = make_data_type_n(&send_message.data[0], (*(u32**)data)[0]);
                 offset += make_data_type_q(&send_message.data[offset], (*(u32**)data)[1]);
                 offset += make_data_type_m(&send_message.data[offset], (*(s32**)data)[2]);

                 send_message.data_len = offset;
			 }
			 else {
			     send_message.data[0] = '\0';
                 send_message.data_len = 0;
			 }
             
             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
        case RCMD_SELL_CANCELL: {
//             send_message.cmd = RCMD_SELL_CANCELL;
             
			 if (data != NULL) {
			     offset = make_data_type_n(&send_message.data[0], *(u32 *)data[0]);

                 send_message.data_len = offset;
			 }
             else {
			     send_message.data[0] = '\0';

                 send_message.data_len = 0;
			 }

             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
		case RCMD_DATA_TIME_GET: {
			 send_message.data_len = 0;
                
             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
		case RCMD_CASH_GET_PUT: {
			 if (data != NULL) {
                 offset = make_data_type_n(&send_message.data[0], (*(u32**)data)[0]);
                 offset += make_data_type_m(&send_message.data[offset], (*(s32**)data)[1]);

                 send_message.data_len = offset;
			 }
			 else {
			     send_message.data[0] = '\0';
                 send_message.data_len = 0;
			 }
                
             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
		case RCMD_MODEM_STATUS: {
			 if (data != NULL) {
			     offset = make_data_type_n(&send_message.data[0], *(u32 *)data[0]);

                 send_message.data_len = offset;
			 }
             else {
			     send_message.data[0] = '\0';

                 send_message.data_len = 0;
			 }
                
             should_send_data = 1;
			 RegistratorStatusSet(RR_CONNECTION_WAIT_ANSVER);
             break;
        }
/*      default : {
            break;
        }
*/        
    }
	
	return (1);
}


REGISTRATOR_ERROR_CODE RegistratorErrorCode (RegistratorReceivedData *rr_err)
{
/*
    u08 i;
	RR_ERROR_CODE code;
    
	code = 0;
	for (i = 0; i < rr_err->len; i++) {
	    code += rr_err->dataptr[i];
	}
    return  code;
*/

    return  (rr_err->dataptr[0] + rr_err->dataptr[1] + rr_err->dataptr[2] + rr_err->dataptr[3]);
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
			 return 0;
             //break;
        }
        case RANSVER_NAK: {
             return RANSVER_NAK;
            // break;
        }
        case RANSVER_SYN: {
             return RANSVER_SYN;
            // break;
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
             cur_state = (receive_message.seq < 0) ? FRAME_WAIT : FRAME_CMD;
             break;
        }
        case FRAME_CMD: {
             crc_cnt += c;
             receive_message.cmd = c;
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
                 receive_message.data[i] = c;
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

 //                i = 0;

                 u08 temp_crc[R_CRC_LEN];

			     
                 //makecrc
				 i = R_CRC_LEN;
				 crc_cnt &= 0x0000FFFF;
                 while (i-- > 0) {
                     temp_crc[i] = crc_cnt % 16 + '0';
                     crc_cnt /= 16;
                 }

		         if (strncmp((char *) temp_crc, (char *) receive_message.bcc, R_CRC_LEN) == 0) {
                     ret = RANSVER_AK;
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
    u08 buf[4];
	u32 crc_temp = 0;
	u08 i;
    
    send_message.len = 5 + R_PWD_LEN + send_message.data_len;
	send_message.len = CONVERT_FOR_SEND(send_message.len);
    
    buf[0] = RDATA_SOH;
    buf[1] = send_message.len;
    buf[2] = CONVERT_FOR_SEND(send_message.seq);
    buf[3] = send_message.cmd;
	crc_temp = buf[1] + buf[2] + buf[3];
	RegistratorSendStr(buf, 4);    
	
	RegistratorSendStr(send_message.pwd, R_PWD_LEN);
	for (i = 0; i < R_PWD_LEN; i++)
        crc_temp += send_message.pwd[i];
        
    buf[0] = RDATA_SEPARATOR;
	crc_temp += buf[0];
	RegistratorSendStr(buf, 1);
	
    RegistratorSendStr(send_message.data, send_message.data_len);
    for (i = 0; i < send_message.data_len; i++)
        crc_temp += send_message.data[i];
    
	buf[0] = RDATA_ENQ;
	crc_temp += buf[0];
    RegistratorSendStr(buf, 1);

    crc_temp &= 0x0000FFFF;    
    makecrc(send_message.bcc, (u16)crc_temp);
    RegistratorSendStr(send_message.bcc, R_CRC_LEN);
    
    buf[0] = RDATA_ETX;
    RegistratorSendStr(buf, 1);
}
        
            
u08 increase_seq (u08 cur)
{
    return (cur++ < SEQ_VALUE_MAX) ? cur : 0;  
}     
 

u08 make_data_type_n (u08 *to, u32 d)
{
    u08 len;

    ltoa(d, (char *)to, 10);
	len = strlen((char *)to);

	to[len] = RDATA_SEPARATOR;
	len++;

    return len;
}

u08 make_data_type_m (u08 *to, s32 d)
{
    u08 len;

    ltoa(d, (char *)to, 10);
	len = strlen((char *)to);
	len = set_point(to, 2, len);
    
	to[len] = RDATA_SEPARATOR;
	len++;
    
	return len;
}

u08 make_data_type_q (u08 *to, u32 d)
{
    u08 len;

    ltoa(d, (char *)to, 10);
    len = strlen((char *)to);
    len = set_point(to, 3, len);
    
	to[len] = RDATA_SEPARATOR;
	len++;
    
	return len;
}

u08 set_point (u08 *s, u08 pos, u08 len_cur)
{
    u08 i;

	if ((len_cur - pos) <= 0) {
	    pos++;
		for (i = 0; i <= pos; i++) {
		    if (len_cur - i >= 0) 
                s[pos - i] = s[len_cur - i];
            else 
               s[pos - i] = '0';
		}
		
		len_cur = pos;
		pos = 1;
	}
	else {
	    pos = len_cur - pos;
	}

    i = len_cur + 1;
    for ( ; len_cur > pos; len_cur--) {
        s[len_cur] = s[len_cur-1];
	}
    
    s[pos] = '.';

	return i;
} 
 
void makecrc (u08 * crc, u16 bcc_cnt) 
{
    u08 i;

    i = R_CRC_LEN;
    while (i-- > 0) {
        send_message.bcc[i] = bcc_cnt % 16 + '0';
        bcc_cnt /= 16;
    }
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
