#ifndef PORTSIO_H
#define PORTSIO_H

void InitPortsIO (void);

extern void Uart0_Resiv (u08 udrdata);

void Uart1Enable (void);
void Uart1Disable (void);

void Uart0Enable (void (*getbyte)(u08 c),  u16 baudrate);
void Uart0Disable (void);

//==================================


volatile u16 CountPulse;
volatile u08 CountPulseFault;
volatile u08 Sygnal_Get_Axellerometr;

u16 ADR_LAST_DATTA;
u16	EEPROM_DATA[54];

volatile u32 CollectoinCountManey;

#define  MAX_RX_SIZE_BUFF 20
volatile u08 BUF_UART_RX[MAX_RX_SIZE_BUFF];
volatile u08 rx;
volatile u08 Fl_RxReciv;

//==================================
#define	read_maney_count		0x3a
#define	write_maney_count		0x3b
#define	read_pulse_fault_count	0x4a
#define	write_pulse_count		0x4b
#define	read_int_eeprom			0x5a
#define	write_int_eeprom		0x5b
#define	read_ext_eeprom			0x5c
#define	arase_ext_eeprom		0x5d
#define	set_the_rtc 			0x6b
#define read_from_rtc			0x6a

//==================================
#define BTN_START (PINB & 0x20)          
#define BTN_STOP  (PINB & 0x10)			

#define BTN_RESERV1 (PINC & 0x10)		
#define BTN_RESERV2 (PINC & 0x08)		
	
#define BTN_DOOR  (PINB & 0x80)			
#define BTN_RESET (PINB & 0x40)			

#define WATER_PRESENT (PINE & 0x04)		

#define COUNT_COIN    (PINE & 0x10)		
#define ACCELEROMETER (PINE & 0x20)     

#define COUNT_BILL        (PINE & 0x80)	
#define STATUS_COUNT_BILL (PINB & 0x01)	

#define COUNT_WATER (PINE & 0x40)		


#define STATUS_PWR_12V (PING & 0x08)		
#define STATUS_PWR_5V  (PING & 0x10)		

//#define STATUS_GSM (PIND & 0x10)			

//#define PUMP_ON  PORTA &= ~(1<<3)
//#define PUMP_OFF PORTA |=  (1<<3)
#define PUMP_ON  PORTA |=  (1<<3)
#define PUMP_OFF PORTA &= ~(1<<3)

#define KLAPAN1_ON  PORTA&=~(1<<4)
#define KLAPAN1_OFF PORTA|=(1<<4)

#define KLAPAN2_ON  PORTA&=~(1<<5)
#define KLAPAN2_OFF PORTA|=(1<<5)

#define KLAPAN3_ON  PORTA&=~(1<<6)
#define KLAPAN3_OFF PORTA|=(1<<6)


#define INHIBIT_COIN_ON  PORTE&=~(1<<3)
#define INHIBIT_COIN_OFF PORTE|=(1<<3)

#define EN_ESCROW_BILL_ON  PORTB|=(1<<2)
#define EN_ESCROW_BILL_OFF PORTB&=~(1<<2)

#define INHIBIT_BILL_ON  PORTB&=~(1<<3)
#define INHIBIT_BILL_OFF PORTB|=(1<<3)

//#define PWRKEY_ON  PORTD&=~(1<<5)
//#define PWRKEY_OFF PORTD|=(1<<5)

#define WP_MEMORY_ON  PORTD|=(1<<6)
#define WP_MEMORY_OFF PORTD&=~(1<<6)


#define BUZZER_ON  PORTC |=  (1<<2)
#define BUZZER_OFF PORTC &= ~(1<<2)


#define  BTN_REGISTRATOR_PRESENT    (PINB & 0x02)	

////////////////////////////////////////////////////////////////////////////

unsigned int		*cost_litre_coef;
unsigned int		*pulse_litre_coef;
unsigned int		*pump_on_time_coef;
unsigned int		*pump_off_time_coef;
unsigned int		*bill_time_pulse_coef;		
unsigned int		*coin_time_pulse_coef;		
unsigned int		*ext_eepr_data_adr;		
unsigned int		*vodomat_number;
unsigned int		*pfone_number_1;
unsigned int		*pfone_number_2;
unsigned int		*pfone_number_3;
unsigned int		*pfone_number_4;
unsigned int		*pfone_number_5;
unsigned int		*pfone_number_6;
unsigned int		*pfone_number_7;

unsigned int		*board_version;
unsigned int		*sms_water_level;

unsigned long		*amount_water;
unsigned long		*day_maney_cnt;
unsigned long		*max_size_barrel;
unsigned int		*ext_eepr_cur_adr;

unsigned int		*report_interval;
unsigned int		*lower_report_limit;
unsigned int		*upper_report_limit;

unsigned int		*lock_param;

//unsigned long		*collection_maney;

#define				CostLitreCoefEEPROMAdr			0x20
#define				PulseLitreCoefEEPROMAdr			0x22

#define				PumpOnTimeCoefEEPROMAdr			0x24
#define				PumpOffTimeCoefEEPROMAdr 		0x26

#define				BillTimePulseCoefEEPROMAdr		0x28
#define				CoinTimePulseCoefEEPROMAdr		0x2a
#define             ExtEeprDataAdrEEPROMAdr			0x2c

#define             VodomatNumberEEPROMAdr			0x2e
#define             PfoneNumber1EEPROMAdr			0x30
#define             PfoneNumber2EEPROMAdr			0x32
#define             PfoneNumber3EEPROMAdr			0x34
#define             PfoneNumber4EEPROMAdr			0x36
#define             PfoneNumber5EEPROMAdr			0x38
#define             PfoneNumber6EEPROMAdr			0x3a
#define             PfoneNumber7EEPROMAdr			0x3c
#define             BoardVersionEEPROMAdr			0x3e
#define             SMSWaterLevelEEPROMAdr			0x40

#define             AmountWaterEEPROMAdr			0x42
#define             DayManeyCntEEPROMAdr			0x46
#define             MaxSizeBarrelEEPROMAdr			0x4a
#define             ExtEeprCarAdrEEPROMAdr	    	0x4e

#define             RepIntervalEEPROMAdr	        0x50
#define             LowRepLimitEEPROMAdr            0x52
#define             UpRepLimitEEPROMAdr		        0x54

#define             ApnUserPassAdressEEPROMAdr   	0x56
#define             ServerNameEEPROMAdr 			0x92
#define             ScriptNameEEPROMAdr 			0xb0
#define             PasswordEEPROMAdr 	    		0xc0
#define             CollectionManeyEEPROMAdr 		0xca

#define             LockParamEEPROMAdr              0xffc 
#define             SoftVersionEEPROMAdr            0xffe


#endif
