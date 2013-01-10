//***********************************************************************
// FILE: CADC.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 15-01-2012 
// DESCRIPTION: The class implements a driver for Analog to digital converter.
//************************************************************************

#include "CADC.h"
#include <avr/io.h>

#define BATTERY_VOLTAGE_OFFSET      0

int8_t CADC::mCorrection;

// ----------------------------------------------------------------------------

void CADC::init(void)
{
   ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);   // ADC prescaler 128

   ADMUX |= (1 << REFS0);   // AVCC as reference voltage

   ADMUX |= (1 << ADLAR);  // Left adjust result

   ADMUX |= 0x01;        // select ADC1 as ADC input

   ADCSRA |= (1 << ADATE);  // Free running mode

   ADCSRA |= (1 << ADEN);  // Enable ADC

   ADCSRA |= (1 << ADSC);  // Start single conversion

   mCorrection = 0;     // default correction;
}

// ----------------------------------------------------------------------------

void CADC::setCorrection(int8_t correction)
{
   mCorrection = correction;
}

// ----------------------------------------------------------------------------

uint8_t CADC::getBattVolt(void)
{
   uint8_t adc_value = ADCH;     //ADC=155 equals 6.0V  and each step of ADC is 0.04V
   float voltage;

   voltage = 60 + ((float)(adc_value - 155) * 0.4);      

   return (uint8_t)voltage;
}

