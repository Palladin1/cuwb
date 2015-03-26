/*! \file i2ceeprom.c \brief Interface for standard I2C EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'i2ceeprom.c'
// Title		: Interface for standard I2C EEPROM memories
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.04.23
// Revised		: 2003.04.23
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "global.h"
#include <util/delay.h>

#include "i2c.h"
#include "i2ceeprom.h"

#include "portmacro.h"


// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// functions

u16 i2ceepromInit(u16 memAddr)
{	
	u16 ret;
	u08 retBuff[2];
	i2ceepromReadBloc(memAddr, retBuff, 2);
	ret = retBuff[0];
	ret = ((ret << 8) + retBuff[1]);
return ret;
}

u08 i2ceepromReadByte(u16 memAddr)
{
	u08 packet[2];
	// prepare address
	packet[0] = (memAddr >> 8);
	packet[1] = (memAddr & 0x00FF);
//cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	i2cMasterSendNI(EEPROM_ADRESS_I2C, 2, packet);

	// retrieve the data at this memory address
	i2cMasterReceiveNI(EEPROM_ADRESS_I2C, 1, packet);
//sei();
portEXIT_CRITICAL();
	// return data
	return packet[0];
}


void i2ceepromWriteByte(u16 memAddr, u08 data) {

    u08 packet[3];
    // prepare address + data
    packet[0] = (u08)(memAddr>>8);
    packet[1] = (u08)(memAddr&0x00FF);
    packet[2] = data;
//    cli();
portENTER_CRITICAL();
    // send memory address we wish to access to the memory chip
    // along with the data we wish to write

	i2cMasterSendNI(EEPROM_ADRESS_I2C, 3, packet);

//    sei();
portEXIT_CRITICAL();
}

/*
void i2ceepromWriteBloc(u16 memAddr, u08 *data, u08 length)
{
	u08 packet[length+3];
	// prepare address + data
	packet[0] = (u08)(memAddr>>8);
	packet[1] = (u08)(memAddr&0x00FF);
	for (u08 count = 0; count<=length; count++) {
		packet[count+2] = data[count];
	}
	// send memory address we wish to access to the memory chip
	// along with the data we wish to write
	i2cMasterSendNI(EEPROM_ADRESS_I2C, (length+2), packet);
}
*/

void i2ceepromWriteBloc(u16 memAddr, u08 *data, u08 length) {
	
    for (u08 count = 0; count < length; count++) {
portENTER_CRITICAL();
	    i2ceepromWriteByte((memAddr + count), data[count]);
portEXIT_CRITICAL();        
		_delay_ms(5);
    }
}


void i2ceepromReadBloc(u16 memAddr, u08 *data, u08 length)
{
	u08 packet[2];
	// prepare address
	packet[0] = (memAddr >> 8);
	packet[1] = (memAddr & 0x00FF);
//cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	i2cMasterSendNI(EEPROM_ADRESS_I2C, 2, packet);
	// retrieve the data at this memory address
	i2cMasterReceiveNI(EEPROM_ADRESS_I2C, length, data);
	// return data
//sei();
portEXIT_CRITICAL();
}
