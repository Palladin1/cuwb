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

#include "i2c.h"
#include "DS1337.h"

#include "portmacro.h"

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

// functions
void DS1337Init(void)
{
	u08 status;
portENTER_CRITICAL();
//	i2cInit(); //Init in main
	status = DS1337ReadByte(DS1337_REG_STATUS);
//cli();

	if (status & 0x80)
	    DS1337WriteByte(DS1337_REG_CONTROL, (status & 0x7F));
//sei();
portEXIT_CRITICAL();
}

u08 DS1337ReadByte(u08 regAddr)
{
	u08 packet[1];
	// prepare address
	packet[0] = regAddr;
//	cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	i2cMasterSendNI(DS1337_ADRESS_I2C, 1, packet);
	// retrieve the data at this memory address
	i2cMasterReceiveNI(DS1337_ADRESS_I2C, 1, packet);
//	sei();
portEXIT_CRITICAL();
	// return data
	return packet[0];
}

void DS1337WriteByte(u08 regAddr, u08 data)
{
	u08 packet[2];
	// prepare address + data
	packet[0] = regAddr;
	packet[1] = data;
//cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	// along with the data we wish to write
	i2cMasterSendNI(DS1337_ADRESS_I2C, 2, packet);
//	sei();
portEXIT_CRITICAL();
}

void DS1337ReadDatta(u08* p_dataBuf)
{	
	u08 packet[8];
	packet[0] = 0;
//cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	i2cMasterSendNI(DS1337_ADRESS_I2C, 1, packet);
	// retrieve the data at this memory address
	i2cMasterReceiveNI(DS1337_ADRESS_I2C, 7, p_dataBuf);
//sei();
portEXIT_CRITICAL();
}


void DS1337WriteDatta(u08* p_dataBuf)
{
	u08 packet[8];
	// prepare address + data
	packet[0] = 0;
	for(u08 count = 0; count <= 6; count++) {
		packet[count + 1] = p_dataBuf[count];
	}
//cli();
portENTER_CRITICAL();
	// send memory address we wish to access to the memory chip
	// along with the data we wish to write
	i2cMasterSendNI(DS1337_ADRESS_I2C, 8, packet);

//sei();
portEXIT_CRITICAL();
}
