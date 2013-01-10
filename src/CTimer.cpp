//***********************************************************************
// FILE: CTimer.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 29-08-2010 
// DESCRIPTION: The timer class. Implements a driver over the hardware timer.
//************************************************************************
#include "CTimer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

CTimer Timer;

#define BEEP_PIN        PD7  
#define BEEP_PORT       PORTD
#define BEEP_DDR        DDRD
#define TOGGLE_BEEP_PIN()     PORTD ^= _BV(PD7)   


volatile uint32_t t2;
volatile bool sw;		// switch 63 or 62 ticks
volatile uint8_t ticks;
volatile bool beepOn;

SIGNAL(TIMER2_OVF_vect)
{
	if ((sw && (ticks == 61)) ||
	 	((!sw) && (ticks == 62)) )
	{
		sw = !sw;
		ticks=0;
		t2++;

      if (beepOn)
      {
         TOGGLE_BEEP_PIN();      // this will produce 1kHz sound
      }
	}
	else
		ticks++;
}

// -----------------------------------------------

void CTimer::init(void)
{
	TCCR2 = 1;	// the prescaler is disabled
	TIMSK |= 1<<TOIE2;
	
	t2 = 0;
	sw = false;
	ticks = 0;

   BEEP_DDR |= _BV(BEEP_PIN);

   beepOn = false;
}

// -----------------------------------------------

uint32_t CTimer::getFullTimer(void)
{
	uint32_t t;
	uint8_t sreg = SREG;

	cli();
	t = t2;
	
	SREG = sreg;

	return (t);
}

// -----------------------------------------------

uint16_t CTimer::getLoTimer(void)
{
	return ((uint16_t)getFullTimer());
}

// -----------------------------------------------

void CTimer::sleep(uint16_t ms)
{
	uint32_t t = getFullTimer() + ms;

	while (getFullTimer() < t);
}

// -----------------------------------------------

void CTimer::beep(bool enable)
{
   beepOn = enable;
}
