#ifndef __CSERVOSMGR_H__
#define __CSERVOSMGR_H__
//***********************************************************************
// FILE: CServosMgr.h
// AUTHOR: Yuriy Skriyka
// DATE: 07-01-2012 
// DESCRIPTION: The class implements forming the signal to controll several hobby servos. 
//    The algorithm is based on Servo class implementation from arduino.
// The configuration of servos (the number of servos) is fully static.
//************************************************************************

#include <inttypes.h>
#include <avr/interrupt.h>

// forward declaration of the interrupt
extern "C" void TIMER1_COMPA_vect(void) __attribute__ ((signal));


// The class is fully statical. All the public methods are statical.
class CServosMgr
{
private:
   CServosMgr();     // hide constructor to prevent constructing the object

public:
   
   // more than 12 is possible but the whole servo refresh rate will decrease 
   // because the pause between controlling pulses will be more than 20ms
   static const uint8_t MAX_SERVOS = 19;     

   static void init(void);

   // The method sets pulse width for the specified servo number
   // Note: The method enables dissabled servo
   static void setMicros(const uint8_t servo, uint16_t pulse_witdh);

   // The method sets pulse width for all the servos in the system
   // Note: The method doesn't enable disabled servos
   static void setMicrosAll(uint16_t pulse_width);

   // Disables the specified servo
   // To enable it again the setMicros should be called for the servo
   static void disable(const uint8_t servo); 

private:
   friend void TIMER1_COMPA_vect(void);   // the interrupt is a friend

   static volatile int8_t mCurrServo;      
   static uint16_t mPulses[MAX_SERVOS];   // the values of width of servos in ticks


   // Calculates the sum of pulse widths of all servos
   static uint16_t getTotalPulseWidth(void);
};




#endif // __CSERVOMGR_H__