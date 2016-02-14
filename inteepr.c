//#include <avr/io.h>
#include <avr/eeprom.h>
//#include <avr/interrupt.h>

#include "inteepr.h"


void IntEeprWordWrite (unsigned int eepr_adr, unsigned int eepr_data)
{
	eeprom_busy_wait();
	eeprom_write_word((uint16_t *)*(&eepr_adr), (uint16_t) eepr_data);
//    eeprom_read_byte(&0);
}


unsigned int IntEeprWordRead (unsigned int eepr_adr) 
{
	eeprom_busy_wait();
    return eeprom_read_word((uint16_t *)*(&eepr_adr));
//    eeprom_read_byte(&0);
}


void IntEeprDwordWrite (unsigned int eepr_adr, unsigned long eepr_data) 
{
	eeprom_busy_wait();
	eeprom_write_dword((uint32_t *)*(&eepr_adr), (uint32_t) eepr_data);
//    eeprom_read_byte(&0);
}


unsigned long IntEeprDwordRead (unsigned int eepr_adr) 
{
    eeprom_busy_wait();
    return eeprom_read_dword((uint32_t *)*(&eepr_adr));
//    eeprom_read_byte(&0);
}


void IntEeprBlockRead (unsigned int buff_adr, unsigned int eepr_adr, unsigned char size) 
{
	eeprom_busy_wait();
    eeprom_read_block((uint8_t *)*(&buff_adr), (uint16_t *)*(&eepr_adr), size);
//    eeprom_read_byte(&0);
}


void IntEeprBlockWrite (unsigned int buff_adr, unsigned int eepr_adr, unsigned char size) 
{
	eeprom_busy_wait();
	eeprom_write_block((uint8_t *)*(&buff_adr), (uint16_t *)*(&eepr_adr), size);
//    eeprom_read_byte(&0);
}
