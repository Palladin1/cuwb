#ifndef  INDICATOR_H
#define  INDICATOR_H


#include "avrlibtypes.h"


#define  INDICATOR_DIGIT_NUMBER_PORT    PORTA
#define  INDICATOR_DIGIT_DATA_PORT      PORTF

#define  INDICATOR_DIGIT_PORT_MASK    0xF8


#define  INDICATOR_DIGIT_NUMBER_MAX    8U
#define  INDICATOR_SYMBOL_SHOW_MAX    11U


#define  INDICATOR_DATA_VALUE_MAX    9999U


void IndicatorInit (void);
void IndicatorDataShow (void);
void IndicatorDataConvert (u08 *data_buf, const u16 indicator_first, const u16 indicator_second);
void IndicatorDataWrite (const u08 *data_buf);


#endif  /* INDICATOR_H */
