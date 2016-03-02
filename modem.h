#ifndef MODEM_H
#define MODEM_H


#define PIN_MODEM_STATUS    PIND
#define PORT_MODEM_POWERKEY PORTD

#define NUMBER_PIN_MODEM_STATUS    0x10
#define NUMBER_PORT_MODEM_POWERKEY 0x05

#define STATUS_GSM (PIN_MODEM_STATUS & NUMBER_PIN_MODEM_STATUS)
#define PWRKEY_ON  (PORT_MODEM_POWERKEY &=~ (1 << NUMBER_PORT_MODEM_POWERKEY))
#define PWRKEY_OFF (PORT_MODEM_POWERKEY |= (1 << NUMBER_PORT_MODEM_POWERKEY))


typedef enum {
	    
    ACK_NO,
	ACK_OK,
	ACK_ERROR,
	ACK_CAN_SEND,
	ACK_SEND_OK,
	ACK_SEND_FAIL

} MODEM_ANSVER;


unsigned char ModemStatus(void);
unsigned char ModemSetSetings(void);
unsigned char ModemSMSSend(unsigned char *number, unsigned char *text_sms);

unsigned char ModemSendCom (const char *com_buff, unsigned int com_deluy_time_ms);
unsigned char ModemSendData (const char *data_buff, unsigned int data_deluy_time_ms);

void ModemAnsverSet (const MODEM_ANSVER ansver) ;

#endif //MODEM_H
