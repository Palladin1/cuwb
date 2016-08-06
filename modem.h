#ifndef MODEM_H
#define MODEM_H


#define PIN_MODEM_STATUS    PIND
#define PORT_MODEM_POWERKEY PORTD

#define NUMBER_PIN_MODEM_STATUS    0x10
#define NUMBER_PORT_MODEM_POWERKEY 0x05

#define STATUS_GSM (PIN_MODEM_STATUS & NUMBER_PIN_MODEM_STATUS)
#define PWRKEY_ON  (PORT_MODEM_POWERKEY &=~ (1 << NUMBER_PORT_MODEM_POWERKEY))
#define PWRKEY_OFF (PORT_MODEM_POWERKEY |= (1 << NUMBER_PORT_MODEM_POWERKEY))



#define  MODEM_PWR_GPIO    D, 6, 1, 0 /* name - A, B, C, D, etc., num - fron 0 to 7, init state - 0 or 1, dir - output 1, inpur 0 */

#define  GPIO_DDDR(nm) (DDR##nm)
#define  GPIO_PORT(nm) (PORT##nm)
#define  GPIO_PIN(nm) (PIN##nm)

#define  __GPIO_INIT(name, num, state, dir)     (state)? (GPIO_DDDR(name) |= (1 << num)) : (GPIO_DDDR(name) &= ~(1 << num)); (dir) ? (GPIO_PORT(name) |= (1 << num)) : (GPIO_PORT(name) &= ~(1 << num))   
#define  __GPIO_SET(name, num, state, dir)      (GPIO_PORT(name) |= (1 << num))
#define  __GPIO_CLEAR(name, num, state, dir)    (GPIO_PORT(name) &= ~(1 << num))
#define  __GPIO_IS_SET(name, num, state, dir)   (GPIO_PIN(name) & (1 << num))

#define  GPIO_INIT(...)      __GPIO_INIT(__VA_ARGS__)
#define  GPIO_SET(...)       __GPIO_SET(__VA_ARGS__)
#define  GPIO_CLEAR(...)     __GPIO_CLEAR(__VA_ARGS__)
#define  GPIO_IS_SET(...)    __GPIO_SET(__VA_ARGS__)


typedef enum {
	    
    ACK_NO,
	ACK_OK,
	ACK_ERROR,
	ACK_CAN_SEND,
	ACK_SEND_OK,
	ACK_SEND_FAIL

} MODEM_ANSVER;


void ModemInit (void);
void ModemPowerOn (void);
void ModemPowerOff (void);
unsigned char ModemIsPower (void);

unsigned char ModemStatus(void);
unsigned char ModemSetSetings(void);
unsigned char ModemSMSSend(unsigned char *number, unsigned char *text_sms);

unsigned char ModemSendCom (const char *com_buff, unsigned long com_deluy_time_ms);
unsigned char ModemSendData (const char *data_buff, unsigned long data_deluy_time_ms);

void ModemAnsverSet (const MODEM_ANSVER ansver) ;

#endif //MODEM_H
