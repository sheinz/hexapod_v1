#ifndef __CBODY_H__
#define __CBODY_H__
//***********************************************************************
// FILE: CBody.h
// AUTHOR: Yuriy Skriyka
// DATE: 09-01-2012 
// DESCRIPTION: The class is a container for legs.
//************************************************************************

#include <inttypes.h>
#include "CLeg.h"

class CBody
{
public:

   CBody() {};

   void init(void);

   void test(void);

   void test2(void);

private:

   static const uint8_t LEGS_NUMBER = 6;

   // The array of legs
   CLeg mLegs[LEGS_NUMBER];

};



#endif   // __CBODY_H__
