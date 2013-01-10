#ifndef __TIMER_H__
#define __TIMER_H__
//***********************************************************************
// FILE: CTimer.h
// AUTHOR: Yuriy Skriyka
// DATE: 29-08-2010 
// DESCRIPTION: The timer class. Implements a driver over the hardware timer.
//************************************************************************

#include <inttypes.h>

class CTimer;
extern CTimer Timer;

class CTimer
{
public:

   void init(void);

   uint32_t getFullTimer(void);

   uint16_t getLoTimer(void);

   uint16_t getHourTimer(void);

   void sleep(uint16_t ms);

   // turns on/off the beeper
   void beep(bool enable);

private:


};

#endif
