#ifndef __CADC_H__
#define __CADC_H__
//***********************************************************************
// FILE: CADC.h
// AUTHOR: Yuriy Skriyka
// DATE: 15-01-2012 
// DESCRIPTION: The class implements a driver for Analog to digical convertor.
//************************************************************************
#include <inttypes.h>


class CADC
{
public:

   static void init(void);

   // correction = 1 = 100mV; correction = 10 = 1V
   static void setCorrection(int8_t correction);

   // The method returns the current battery voltage in 100mV units 
   // It means that value 65 equals 6.5V
   static uint8_t getBattVolt(void);


private:

   CADC();

   static int8_t mCorrection;

};


#endif  // __CADC_H__