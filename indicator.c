//----- Include Files ---------------------------------------------------------
#include  <avr/io.h>
#include  "indicator.h"


/*                                                                               0     1     2     3     4     5     6     7     8     9     -  */
static const  u08 Indicator_Digit_Data_Code[INDICATOR_SYMBOL_SHOW_MAX]     = { 0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x01 };
static const  u08 Indicator_Digit_Number_Code[INDICATOR_DIGIT_NUMBER_MAX]  = { 0x00, 0x04, 0x02, 0x06, 0x01, 0x05, 0x03, 0x07 };

static  u08 Indicator_Data[INDICATOR_DIGIT_NUMBER_MAX] = {0};

void IndicatorInit (void)
{

    u08 i;
	for (i = 0; i < INDICATOR_DIGIT_NUMBER_MAX; i++) {
	    Indicator_Data[i] = 0;
	}

}


inline void IndicatorDataShow (void)
{
    static u08 digit_number_cnt = 0;

	//PORTF = 0x00;

//    PORTA = ((PORTA & 0xF8) | LCD_NUMBER[digit_number_cnt]);
//	PORTF = (LCD_DATTA[LcdDatta[digit_number_cnt]]);	
	INDICATOR_DIGIT_NUMBER_PORT = ((INDICATOR_DIGIT_NUMBER_PORT & 0xF8) | Indicator_Digit_Number_Code[digit_number_cnt]);
	INDICATOR_DIGIT_DATA_PORT = (Indicator_Digit_Data_Code[Indicator_Data[digit_number_cnt]]);


	digit_number_cnt++;
	if (digit_number_cnt >= INDICATOR_DIGIT_NUMBER_MAX) { 
	    digit_number_cnt = 0x00;
    }  
}


inline void IndicatorDataConvert (u08 *data_buf, const u16 indicator_first, const u16 indicator_second) 
{

/*
    itoa(led_maney & 0x1FFF, &LcdDatta[0], 10);
	
	itoa(led_water & 0x1FFF, &LcdDatta[4], 10);
*/

	data_buf[0] = (u08)( indicator_first / 1000);
	data_buf[1] = (u08)((indicator_first / 100) % 10);
	data_buf[2] = (u08)((indicator_first % 100) / 10);
	data_buf[3] = (u08)((indicator_first % 100) % 10);

	data_buf[4] = (u08)( indicator_second / 1000);
	data_buf[5] = (u08)((indicator_second / 100) % 10);
	data_buf[6] = (u08)((indicator_second % 100) / 10);
	data_buf[7] = (u08)((indicator_second % 100) % 10);
}


inline void IndicatorDataWrite (const u08 *data_buf)
{
    u08 i;

	for (i = 0; i < INDICATOR_DIGIT_NUMBER_MAX; i++) {
	    Indicator_Data[i] = data_buf[i];
	}
}
