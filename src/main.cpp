
#include <inttypes.h>
#include <avr/interrupt.h>
#include "CServosMgr.h"
#include "CTimer.h"
#include "CTaskMgr.h"
#include "CADC.h"

#include "CBody.h"
#include <util/delay.h>

#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <avr/pgmspace.h>

extern "C"
{
#include "uart.h"
}

#undef FDEV_SETUP_STREAM 
#define FDEV_SETUP_STREAM(p, g, f) { 0, 0, f, 0, 0, p, g, 0 }

static FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_WRITE);




class SimpleTask : public ITask
{
private:

   CBody mBody;

public:

   virtual void init(void)
   {
      mBody.init();
      
      // down
      //CServosMgr::setMicros(16, 1900);
      //CServosMgr::setMicros(17, 1600);

      // up
      //CServosMgr::setMicros(16, 2000);
      //CServosMgr::setMicros(17, 2000);

      // push
      //CServosMgr::setMicros(16, 1800);
      //CServosMgr::setMicros(17, 1400);
   };


   virtual void onExecute(void)
   {

      uint8_t voltage = CADC::getBattVolt();

      printf("Voltage: %d\n", voltage);

      if (voltage < 58)
      {
         Timer.beep(true);
      }
      else
      {
         Timer.beep(false);
      }

      mBody.test();
   };
};


class CHeadTest : public ITask
{
public:
	virtual void init(void)
	{
		mPosition = 1500;
		mGoingUp = true;
		CServosMgr::setMicros(8, mPosition);
	}

	virtual void onExecute(void)
	{
		if (mGoingUp)
		{
			mPosition += 25;
		}
		else
		{
			mPosition -= 25;
		}

		CServosMgr::setMicros(8, mPosition);

		if (mPosition >= 2000)
		{
			mGoingUp = false;
		}
		else if (mPosition <= 1000)
		{
			mGoingUp = true;
		}
	}

private:
	uint16_t mPosition;
	bool mGoingUp;
};

// ----------------------------------------------------------------------------

SimpleTask simpleTask;
CHeadTest headTest;

int main(void)
{
   CServosMgr::init();
   Timer.init();
   TaskMgr.init();
   CADC::init();

   uart_init();

   stdout = stdin = &uart_str;

   sei();

   simpleTask.init();
   headTest.init();

   TaskMgr.Add(&simpleTask, 100);
   TaskMgr.Add(&headTest, 10);

   Timer.sleep(5000);
   while (true)
   {
      TaskMgr.processTasks();
   }

   return 0;
}

extern "C" void __cxa_pure_virtual() { while (1); }
