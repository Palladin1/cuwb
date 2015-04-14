//----- Include Files ---------------------------------------------------------
#include <avr/io.h>			// include I/O definitions (port names, pin names, etc)
#include <avr/eeprom.h>
#include <avr/wdt.h>

#include "global.h"		// include our global settings
#include "portsIO.h"
#include "uart2.h"		// include uart function library
#include "rprintf.h"	// include printf function library
#include "DS1337.h"	    // include DS1337 function library
#include "i2c.h"	    // include i2c function library
#include "i2cconf.h"	// include i2conf configure library
#include "i2ceeprom.h"  // include i2ceeprom function library
#include "extint.h"
#include "eeprTable.h"
#include "inteepr.h"

#include "gsm_buffer.h"
#include "CUWBcfg.h"

void InitPortsIO (void) {
	
    u16 EepromAdr;
//=====================================
//		чтение EEPROM
    EepromAdr = CostLitreCoefEEPROMAdr;													
	eeprom_busy_wait();
//	cli();
	eeprom_read_block(EEPROM_DATA,(uint16_t *)*(&EepromAdr), 54);

//	EepromAdr = 0;													
//	eeprom_busy_wait();
//	eeprom_read_word ((uint16_t *)*(&EepromAdr));
//    sei();

	cost_litre_coef    	 = (uint16_t *) &EEPROM_DATA[0];
	pulse_litre_coef 	 = (uint16_t *) &EEPROM_DATA[1];
	pump_on_time_coef	 = (uint16_t *) &EEPROM_DATA[2];		
	pump_off_time_coef   = (uint16_t *) &EEPROM_DATA[3];
	bill_time_pulse_coef = (uint16_t *) &EEPROM_DATA[4];		
	coin_time_pulse_coef = (uint16_t *) &EEPROM_DATA[5];
	ext_eepr_data_adr	 = (uint16_t *) &EEPROM_DATA[6];

	vodomat_number		 = (uint16_t *) &EEPROM_DATA[7];

	pfone_number_1		 = (uint16_t *) &EEPROM_DATA[8];
	pfone_number_2	     = (uint16_t *) &EEPROM_DATA[9];
	pfone_number_3	     = (uint16_t *) &EEPROM_DATA[10];
	pfone_number_4	     = (uint16_t *) &EEPROM_DATA[11];
	pfone_number_5	     = (uint16_t *) &EEPROM_DATA[12];
	pfone_number_6 	     = (uint16_t *) &EEPROM_DATA[13];
	pfone_number_7	     = (uint16_t *) &EEPROM_DATA[14];

	board_version	     = (uint16_t *) &EEPROM_DATA[15];
    sms_water_level	     = (uint16_t *) &EEPROM_DATA[16];

	amount_water         = (uint32_t *) &EEPROM_DATA[17]; 
	day_maney_cnt        = (uint32_t *) &EEPROM_DATA[19];
    max_size_barrel      = (uint32_t *) &EEPROM_DATA[21]; 
	ext_eepr_cur_adr     = (uint16_t *) &EEPROM_DATA[23];  

	report_interval      = (uint16_t *) &EEPROM_DATA[24];
    lower_report_limit   = (uint16_t *) &EEPROM_DATA[25];
    upper_report_limit   = (uint16_t *) &EEPROM_DATA[26];

    EepromAdr = CollectionManeyEEPROMAdr;
    eeprom_read_block((u16 *)&CollectoinCountManey,(uint32_t *)*(&EepromAdr), 4);


/*Check version of softvare and set if this need*/    
	u16 CurrSoftVer;
	EepromAdr = SoftVersionEEPROMAdr;
    eeprom_read_block((u16 *)&CurrSoftVer,(uint16_t *)*(&EepromAdr), 2);
    if (SOFTVARE_VERSION != CurrSoftVer)
	    IntEeprDwordWrite(SoftVersionEEPROMAdr, SOFTVARE_VERSION);


	
//=====================================
//Default setings for ports

				//                   | valve3    valve2     valve1 |  pump   |  namber of indikator       |
DDRA  = 0XFF;	//	DDRA |= (1<<DDA7)|(1<<DDA6)|(1<<DDA5)|(1<<DDA4)|(1<<DDA3)|(1<<DDA2)|(1<<DDA1)|(1<<DDA0);
if(*board_version == 1) {
PORTA = 0X70;	// PORTA |= (0<<PA7) |(1<<PA6) |(1<<PA5) |(1<<PA4) |(0<<PA3) |(0<<PA2) |(0<<PA1) |(0<<PA0);
}
else {
PORTA = 0X78;	// PORTA |= (0<<PA7) |(1<<PA6) |(1<<PA5) |(1<<PA4) |(1<<PA3) |(0<<PA2) |(0<<PA1) |(0<<PA0);
}

				//     	 |key door   | key rest|key stop |key strt |inh bill |escrow   | registrator present |sts bill| 
DDRB  =	0X0E;	//	DDRB |= (0<<DDB7)|(0<<DDB6)|(0<<DDB5)|(0<<DDB4)|(1<<DDB3)|(1<<DDB2)|(0<<DDB1)            |(0<<DDB0);
PORTB =	0XF5;	// PORTB |= (1<<PB7) |(1<<PB6) |(1<<PB5) |(1<<PB4) |(0<<PB3) |(1<<PB2) |(1<<PB1)             |(1<<PB0);
//DDRB  =	0X0E;	//	DDRB |= (0<<DDB7)|(0<<DDB6)|(0<<DDB5)|(0<<DDB4)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(0<<DDB0);
//PORTB =	0XF5;	// PORTB |= (1<<PB7) |(1<<PB6) |(1<<PB5) |(1<<PB4) |(0<<PB3) |(1<<PB2) |(0<<PB1) |(1<<PB0);

				//     	    	 				    	 | reserv key        | buzzer  | 
DDRC  =	0XE7;	//	DDRC |= (1<<DDC7)|(1<<DDC6)|(1<<DDC5)|(0<<DDC4)|(0<<DDC3)|(1<<DDC2)|(1<<DDC1)|(1<<DDC0);
PORTC =	0X18;	// PORTC |= (0<<PC7) |(0<<PC6) |(0<<PC5) |(1<<PC4) |(1<<PC3) |(0<<PC2) |(0<<PC1) |(0<<PC0);

				//     	    	 | wp mem  | status  | pwr_key|
DDRD  =	0XE0;	//	DDRD |= (1<<DDD7)|(1<<DDD6)|(1<<DDD5)|(0<<DDD4);
PORTD =	0X20;	// PORTD |= (0<<PD7) |(0<<PD6) |(1<<PD5) |(0<<PD4);

				//     	    |cnt bill|cnt water| acceler |cnt coin |inh coin |water prs|
DDRE  =	0X08;	//	DDRE |= (0<<DDE7)|(0<<DDE6)|(0<<DDE5)|(0<<DDE4)|(1<<DDE3)|(0<<DDE2);
//PORTE =	0X6C;	// PORTE |= (0<<PE7) |(1<<PE6) |(1<<PE5) |(0<<PE4) |(1<<PE3) |(1<<PE2);
PORTE =	0XF4;	// PORTE |= (1<<PE7) |(1<<PE6) |(1<<PE5) |(1<<PE4) |(0<<PE3) |(1<<PE2);

				//     	    	 |	    data for indikation                                           | 
DDRF  =	0XFF;	//	DDRF |= (1<<DDF7)|(1<<DDF6)|(1<<DDF5)|(1<<DDF4)|(1<<DDF3)|(1<<DDF2)|(1<<DDF1)|(1<<DDF0);
PORTF =	0X00;	// PORTF |= (0<<PF7) |(0<<PF6) |(0<<PF5) |(0<<PF4) |(0<<PF3) |(0<<PF2) |(0<<PF1) |(0<<PF0);

				//       |pwr 5V     |pwr 12V  |
DDRG  =	0X07;	//	DDRG |= (0<<DDG4)|(0<<DDG3)|(1<<DDG2)|(1<<DDG1)|(1<<DDG0);
PORTG =	0X18;	// PORTG |=(1<<PG4)  |(1<<PG3) |(0<<PG2) |(0<<PG1) |(0<<PG0);

//==================================================
    // initialize the I2C
	i2cInit();

	DS1337Init();

	// initialize the I2CEEPROM 24c512
	ADR_LAST_DATTA = i2ceepromInit((*ext_eepr_data_adr));
	if (ADR_LAST_DATTA == 0xFFFF) {
		ADR_LAST_DATTA = 0x0000;
    }


//==================================================

void Count_Pulse_Water (void) {

	if (CountPulse != 0) {
		CountPulse--;
	}

	if (CountPulse == 0) {
		if (CountPulseFault != 255) {
			CountPulseFault++;
		}
	}
	else {
		CountPulseFault = 0;
	}
	
}

	//! Configure external interrupt trigger
    extintConfigure(EXTINT6, EXTINT_EDGE_ANY);

	//! Attach a user function to an external interrupt
	extintAttach(EXTINT6, Count_Pulse_Water);
//==================================================

void Warning_from_Axellerometr (void) {
    Sygnal_Get_Axellerometr = 1;
}

	//! Configure external interrupt trigger
//	extintConfigure(EXTINT5, EXTINT_EDGE_FALLING);
	extintConfigure(EXTINT5, EXTINT_EDGE_RISING);
	//! Attach a user function to an external interrupt
	extintAttach(EXTINT5, Warning_from_Axellerometr);

//==================================================

}

// UART1 Receiver interrupt service routine
void Uart1_Resiv(u08 udrdata1) {
//   uart0SendByte(udrdata1);
	
    GSM_RxBuf_Char_Put(udrdata1);
}

void Uart1Enable (void) {
    uart1Init();
	uartSetBaudRate(1, 19200);
	uartSetRxHandler(1, Uart1_Resiv);

	// make all rprintf statements use uart for output
  	rprintfInit(uart1SendByte);
}

void Uart1Disable (void) {
    uart1DeInit();
}

/* The UART 0 initialization */
void Uart0Enable (void (*getbyte)(u08 c),  u16 baudrate) {
    uart0Init();
	uartSetBaudRate(0, baudrate);
	uartSetRxHandler(0, getbyte);
}

void Uart0Disable (void) {
    uart0DeInit();
}
