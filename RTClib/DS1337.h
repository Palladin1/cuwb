/*! \file i2ceeprom.h \brief Interface for standard I2C EEPROM memories. */
//*****************************************************************************
//
// File Name	: 'i2ceeprom.h'
// Title		: Interface for standard I2C EEPROM memories
// Author		: Pascal Stang - Copyright (C) 2003
// Created		: 2003.04.23
// Revised		: 2003.04.23
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
///	\ingroup driver_hw
/// \defgroup i2ceeprom Interface for standard I2C EEPROM memories (i2ceeprom.c)
/// \code #include "i2ceeprom.h" \endcode
/// \par Overview
///		This library provides functions for reading and writing standard
///	24Cxxx/24LCxxx I2C EEPROM memories.  Memory sizes up to 64Kbytes are
///	supported.  Future revisions may include page-write support.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef DS1337_H
#define DS1337_H

#include "global.h"

//definitions

#define DS1337_ADRESS_I2C    0xD0
#define DS1337_REG_SECONDS   0x00 
#define DS1337_REG_MINUTES   0x01 
#define DS1337_REG_HOUR      0x02
#define DS1337_REG_DAY       0x03
#define DS1337_REG_DATE      0x04
#define DS1337_REG_MONTH     0x05
#define DS1337_REG_YEAR      0x06
#define DS1337_REG_CONTROL   0x0E
#define DS1337_REG_STATUS    0x0F


// functions

//! Initialize I2C EEPROM interface
void DS1337Init(void);

//! In the I2C EEPROM at [i2cAddr], read a byte from memory location [memAddr]
u08 DS1337ReadByte(u08 regAddr);

//! In the I2C EEPROM at [i2cAddr], write a byte [data] to the memory location [memAddr]
void DS1337WriteByte(u08 regAddr, u08 data);

void DS1337ReadDatta(u08* p_dataBuf);

void DS1337WriteDatta(u08* p_dataBuf);


#endif
