/*! \file extint.c \brief External-Interrupt function library. */
//*****************************************************************************
//
// File Name	: 'extint.c'
// Title		: External-Interrupt function library
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 5/10/2002
// Revised		: 11/16/2004
// Version		: 1.0
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// Notes:	This library provides convenient standardized configuration and
//			access to external interrupts.  The library is designed to make
//			it possible to write code that uses external interrupts without
//			digging into the processor datasheets to find register names and
//			bit-defines.  The library also strives to allow code which uses
//			external interrupts to more easily cross-compile between different
//			microcontrollers.
//
//			NOTE: Using this library has certain advantages, but also adds
//			overhead and latency to interrupt servicing.  If the smallest
//			code size or fastest possible latency is needed, do NOT use this
//			library; link your interrupts directly.
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>

#include "global.h"
#include "extint.h"

// Global variables
typedef void (*voidFuncPtr)(void);
volatile static voidFuncPtr ExtIntFunc[EXTINT_NUM_INTERRUPTS];

// functions

//! initializes extint library
void extintInit(void)
{
	u08 intNum;
	// detach all user functions from interrupts
	for(intNum=0; intNum<EXTINT_NUM_INTERRUPTS; intNum++)
		extintDetach(intNum);

}

//! Configure external interrupt trigger
// NOTE: this function is not complete!!!
void extintConfigure(u08 interruptNum, u08 configuration)
{

///////////////////////////////////////////////////////
if(interruptNum == EXTINT0)
	{
		
	}
	#ifdef SIG_INTERRUPT1
	else if(interruptNum == EXTINT1)
	{
		
	}
	#endif
	#ifdef SIG_INTERRUPT2
	else if(interruptNum == EXTINT2)
	{
		
	}
	#endif
///////////////////////////////////////////////////////



	if(interruptNum == EXTINT0)
	{
        #ifdef ATmega128
            EIMSK &= ~(1<<0);
		    EICRA &= ~((1<<ISC01) | (1<<ISC00));
		    EICRA |= configuration;
		    EIMSK |= (1<<0);    
        #else
		    MCUCR &= ~((1<<ISC01) | (1<<ISC00));
		    MCUCR |= configuration;
		#endif

	}
	#ifdef SIG_INTERRUPT1
	else if(interruptNum == EXTINT1)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<1);
		    EICRA &= ~((1<<ISC11) | (1<<ISC10));
		    EICRA |= (configuration<<2);
		    EIMSK |= (1<<1);
		#else
		    MCUCR &= ~((1<<ISC11) | (1<<ISC10));
		    MCUCR |= configuration<<2;
		#endif
	}
	#endif
	#ifdef SIG_INTERRUPT2
	else if(interruptNum == EXTINT2)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<2);
		    EICRA &= ~((1<<ISC21) | (1<<ISC20));
		    EICRA |= (configuration<<4);
		    EIMSK |= (1<<2);
	    #else
            if(configuration == EXTINT_EDGE_RISING)
			    sbi(MCUCSR, ISC2);
		    else
			    cbi(MCUCSR, ISC2);
		#endif
	}
	#endif
	#ifdef SIG_INTERRUPT3
	else if(interruptNum == EXTINT3)
	{
        #ifdef ATmega128
		    EIMSK &= ~(1<<3);
		    EICRA &= ~((1<<ISC31) | (1<<ISC30));
		    EICRA |= (configuration<<6);
		    EIMSK |= (1<<3);
		#endif
	}
	#endif
	#ifdef SIG_INTERRUPT4
	else if(interruptNum == EXTINT4)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<4);
		    EICRB &= ~((1<<ISC41) | (1<<ISC40));
		    EICRB |= (configuration);
		    EIMSK |= (1<<4);
	    #endif
	}
	#endif
	#ifdef SIG_INTERRUPT5
	else if(interruptNum == EXTINT5)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<5);
		    EICRB &= ~((1<<ISC51) | (1<<ISC50));
			if (EIFR  & (1 << 5)) 
			     EIFR  |= (1<<5);
			if (configuration != EXTINT_DEACTIVATE) {
		    EICRB |= (configuration<<2);
		    EIMSK |= (1<<5);
			}
	    #endif
	}
	#endif
	#ifdef SIG_INTERRUPT6
	else if(interruptNum == EXTINT6)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<6);
		    EICRB &= ~((1<<ISC61) | (1<<ISC60));
		    EICRB |= configuration<<4;
		    EIMSK |= (1<<6);
	    #endif
	}
	#endif
	#ifdef SIG_INTERRUPT7
	else if(interruptNum == EXTINT7)
	{
		#ifdef ATmega128
		    EIMSK &= ~(1<<7);
		    EICRB &= ~((1<<ISC71) | (1<<ISC70));
		    EICRB |= (configuration<<6);
		    EIMSK |= (1<<7);
	    #endif
	}
	#endif
	// need to handle a lot more cases
	// and differences between processors.
	// looking for clean way to do it...
}

//! Attach a user function to an external interrupt
void extintAttach(u08 interruptNum, void (*userHandler)(void) )
{
	// make sure the interrupt number is within bounds
	if(interruptNum < EXTINT_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		ExtIntFunc[interruptNum] = userHandler;
	}
}

//! Detach a user function from an external interrupt
void extintDetach(u08 interruptNum)
{
	// make sure the interrupt number is within bounds
	if(interruptNum < EXTINT_NUM_INTERRUPTS)
	{
		// set the interrupt function to run
		// the supplied user's function
		ExtIntFunc[interruptNum] = 0;
	}
}

//! Interrupt handler for INT0
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT0)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT0])
		ExtIntFunc[EXTINT0]();
}

#ifdef SIG_INTERRUPT1
//! Interrupt handler for INT1
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT1)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT1])
		ExtIntFunc[EXTINT1]();
}
#endif

#ifdef SIG_INTERRUPT2
//! Interrupt handler for INT2
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT2)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT2])
		ExtIntFunc[EXTINT2]();
}
#endif

#ifdef SIG_INTERRUPT3
//! Interrupt handler for INT3
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT3)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT3])
		ExtIntFunc[EXTINT3]();
}
#endif

#ifdef SIG_INTERRUPT4
//! Interrupt handler for INT4
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT4)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT4])
		ExtIntFunc[EXTINT4]();
}
#endif

#ifdef SIG_INTERRUPT5
//! Interrupt handler for INT5
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT5)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT5])
		ExtIntFunc[EXTINT5]();
}
#endif

#ifdef SIG_INTERRUPT6
//! Interrupt handler for INT6
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT6)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT6])
		ExtIntFunc[EXTINT6]();
}
#endif

#ifdef SIG_INTERRUPT7
//! Interrupt handler for INT7
EXTINT_INTERRUPT_HANDLER(SIG_INTERRUPT7)
{
	// if a user function is defined, execute it
	if(ExtIntFunc[EXTINT7])
		ExtIntFunc[EXTINT7]();
}
#endif

