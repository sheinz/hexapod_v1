//***********************************************************************
// FILE: CServosMgr.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 07-01-2012 
// DESCRIPTION: The class implements forming the signal to control several hobby servos.
//    The algorithm is based on Servo class implementation from arduino.
// The configuration of servos (the number of servos) is fully static.
//************************************************************************
#include "CServosMgr.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Todo: analyze if it is really necessary
#define TRIM_DURATION       2          // compensation ticks to trim adjust for digitalWrite delays 

#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo  
#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo 
#define DEFAULT_PULSE_WIDTH  1500     // default pulse width when servo is attached
#define REFRESH_INTERVAL    20000     // minimal time to refresh servos in microseconds 

// ----------------------------------------------------------------------------
#define US_TO_TICKS(_us)    (( F_CPU / 1000000L * (_us)) / 8)     // converts microseconds to tick (assumes prescale of 8)  
#define TICKS_TO_US(_ticks) (( (unsigned)(_ticks) * 8) / (F_CPU / 1000000L) ) // converts from ticks back to microseconds


#define SET_SERVO_LOW(number)       if (number < 8)         PORTB &= ~_BV(number);      \
                                    else if (number < 16)   PORTC &= ~_BV(number-8);    \
                                    else if (number < 19)   PORTD &= ~_BV(4 + number-16)     // the last 3 bits start from 4


#define SET_SERVO_HIGH(number)      if (number < 8)         PORTB |= _BV(number);       \
                                    else if (number < 16)   PORTC |= _BV(number-8);     \
                                    else if (number < 19)   PORTD |= _BV(4 + number-16)      // the last 3 bits start from 4

#define SET_SERVO_PINS_OUTPUT()     DDRB = 0xFF; DDRC = 0xFF; DDRD |= _BV(4) | _BV(5) | _BV(6)   // all pins of the port to output

#define STOP_TIMER   TCCR1B = 0
#define START_TIMER  TCCR1B = _BV(CS11)      // prescaler F_CPU/8
// ----------------------------------------------------------------------------

volatile int8_t CServosMgr::mCurrServo;    
uint16_t CServosMgr::mPulses[CServosMgr::MAX_SERVOS];

// ----------------------------------------------------------------------------

inline uint16_t CServosMgr::getTotalPulseWidth(void)
{
   uint16_t pulse_width = 0;

   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      pulse_width += mPulses[i];
   }

   return pulse_width;
}

// ----------------------------------------------------------------------------

ISR(TIMER1_COMPA_vect)
{
   if (CServosMgr::mCurrServo < 0)
   {
      TCNT1 = 0;     // refresh interval completed so reset the timer 
   }
   else
   {
      if (CServosMgr::mCurrServo < CServosMgr::MAX_SERVOS)  
      {
         // set pin LOW
         SET_SERVO_LOW(CServosMgr::mCurrServo);
      }
   }

   CServosMgr::mCurrServo++;    // increment to the next servo

   while (CServosMgr::mCurrServo < CServosMgr::MAX_SERVOS) 
   {
      if (CServosMgr::mPulses[CServosMgr::mCurrServo] != 0)
      {
         //STOP_TIMER;         
         TCNT1 = 0;
         OCR1A = CServosMgr::mPulses[CServosMgr::mCurrServo];
         //START_TIMER;         
         SET_SERVO_HIGH(CServosMgr::mCurrServo);
         break;
      }
      else
      {
         CServosMgr::mCurrServo++;     // skip to the next servo
      }
   }  

   if (CServosMgr::mCurrServo >= CServosMgr::MAX_SERVOS) 
   { 
      uint16_t totalPulseWidth = CServosMgr::getTotalPulseWidth();
      // finished all channels so wait for the refresh period to expire before starting over 
      if (totalPulseWidth <  US_TO_TICKS(REFRESH_INTERVAL) )  // allow a few ticks to ensure the next OCR1A not missed
      {
         //STOP_TIMER;
         TCNT1 = 0;
         OCR1A = (unsigned int)US_TO_TICKS(REFRESH_INTERVAL) - totalPulseWidth;
         //START_TIMER;
      }
      else
      {
         OCR1A = TCNT1 + 4;   // Start next cycle straight away (4 for safety)
      }

      CServosMgr::mCurrServo = -1; // this will get incremented at the end of the refresh period to start again at the first servo
   }
}

// ----------------------------------------------------------------------------

void CServosMgr::init(void)
{
   SET_SERVO_PINS_OUTPUT();

   mCurrServo = -1;
   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      // at start all servos are enabled with default value
      mPulses[i] = US_TO_TICKS(DEFAULT_PULSE_WIDTH);
   }   
   
   TCCR1A = 0;             // normal counting mode 
   START_TIMER;
   TCNT1 = 0;              // clear the timer count 

   //TIFR |= _BV(OCF1A);      // clear any pending interrupts; 
   TIMSK |=  _BV(OCIE1A) ;  // enable the output compare interrupt 
   OCR1A = US_TO_TICKS(1000);    // call interrupt after 1ms
}

// ----------------------------------------------------------------------------

void CServosMgr::setMicros(const uint8_t servo, uint16_t pulse_width)
{
   if(servo < MAX_SERVOS)   // ensure servo is valid
   {  
      if( pulse_width < MIN_PULSE_WIDTH)          // ensure pulse width is valid
      {
         pulse_width = MIN_PULSE_WIDTH;
      }
      else if( pulse_width > MAX_PULSE_WIDTH )
      {
         pulse_width = MAX_PULSE_WIDTH;   
      }

      pulse_width -= TRIM_DURATION;
      pulse_width = US_TO_TICKS(pulse_width);  // convert to ticks after compensating for interrupt overhead

      uint8_t sreg = SREG;
      cli();
      mPulses[servo] = pulse_width;  
      SREG = sreg;   
   }
}

// ----------------------------------------------------------------------------

void CServosMgr::setMicrosAll(uint16_t pulse_width)
{
   if( pulse_width < MIN_PULSE_WIDTH)          // ensure pulse width is valid
   {
      pulse_width = MIN_PULSE_WIDTH;
   }
   else if( pulse_width > MAX_PULSE_WIDTH )
   {
      pulse_width = MAX_PULSE_WIDTH;   
   }

   pulse_width -= TRIM_DURATION;
   pulse_width = US_TO_TICKS(pulse_width);  // convert to ticks after compensating for interrupt overhead

   uint8_t sreg = SREG;
   cli();
   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      if (mPulses[i] != 0)    // set only enabled servos
      {
         mPulses[i] = pulse_width;
      }
   }
   SREG = sreg;  
}

// ----------------------------------------------------------------------------

void CServosMgr::disable(const uint8_t servo)
{
   if(servo < MAX_SERVOS)   // ensure servo is valid
   {
      uint8_t sreg = SREG;
      cli();
      mPulses[servo] = 0;
      SREG = sreg;
   }
}

