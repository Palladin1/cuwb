#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#include "inteepr.h"

//#include "portmacro.h"

void IntEeprWordWrite (unsigned int eepr_adr, unsigned int eepr_data) {
    
//	portENTER_CRITICAL();
	eeprom_busy_wait();
	eeprom_write_word ((uint16_t *)*(&eepr_adr), (uint16_t) eepr_data);
//	portEXIT_CRITICAL();			
}

unsigned int IntEeprWordRead (unsigned int eepr_adr) {
    
	unsigned int eepr_data = 0;
    
//	portENTER_CRITICAL();
	eeprom_busy_wait();
	eepr_data = eeprom_read_word ((uint16_t *)*(&eepr_adr));
//	portEXIT_CRITICAL();

return (eepr_data);
}

void IntEeprDwordWrite (unsigned int eepr_adr, unsigned long eepr_data) {
    
//	portENTER_CRITICAL();
	eeprom_busy_wait();
	eeprom_write_dword ((uint32_t *)*(&eepr_adr), (uint32_t) eepr_data);
//	portEXIT_CRITICAL();			
}

unsigned long IntEeprDwordRead (unsigned int eepr_adr) {
    
	unsigned long eepr_data = 0;

//	portENTER_CRITICAL();
    eeprom_busy_wait();
    eepr_data = eeprom_read_dword ((uint32_t *)*(&eepr_adr));
//    portEXIT_CRITICAL();

return (eepr_data);
}

void IntEeprBlockRead (unsigned int buff_adr, unsigned int eepr_adr, unsigned char size) {
 
//    portENTER_CRITICAL();
	eeprom_busy_wait();
    eeprom_read_block ((uint8_t *)*(&buff_adr), (uint16_t *)*(&eepr_adr), size);
//	portEXIT_CRITICAL();
}
