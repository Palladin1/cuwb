/*
*********************************************************************************************************
*
*                                            INCLUDES
*
*
* Filename      : includes.h
* Version       : V1.00
* Programmer(s) : Palladin
*********************************************************************************************************
*/

#ifndef  INCLUDES_H
#define  INCLUDES_H


/*
*********************************************************************************************************
*                                         STANDARD LIBRARIES
*********************************************************************************************************
*/
#include <stdio.h>
#include <string.h>
//#include <string.h>

/*
*********************************************************************************************************
*                                              AVR
*********************************************************************************************************
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>



/*
*********************************************************************************************************
*                                                 OS
*********************************************************************************************************
*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//#include "timers.h"
#include "croutine.h"


/*
*********************************************************************************************************
*                                                PRG
*********************************************************************************************************
*/
#include "global.h"		    // include our global settings
#include "portsIO.h"
#include "rprintf.h"      	// include printf function library
#include "command.h"
#include "uart2.h"		    // include uart function library
#include "timer128.h"		// include uart function library
#include "inteepr.h"
#include "modem.h"
#include "gsm_buffer.h"

#include "CUWBcfg.h"

#include  "registrator.h"

#include  "indicator.h"


/*
*********************************************************************************************************
*                                               TCPIP
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 FS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               PROBE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 USB
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDES END
*********************************************************************************************************
*/

#endif /* INCLUDES_H */
