//***********************************************************************
// FILE: CBody.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 09-01-2012 
// DESCRIPTION: The class is a containger for legs.
//************************************************************************

#include "CBody.h"

void CBody::init(void)
{
   mLegs[0].init(CLeg::RIGHT, 18, 17, 16, 30);
   mLegs[1].init(CLeg::RIGHT, 11, 9, 10, -10);
   mLegs[2].init(CLeg::RIGHT, 14, 13, 12, 0);

   mLegs[3].init(CLeg::LEFT, 15, 0, 1, 20);
   mLegs[4].init(CLeg::LEFT, 2, 3, 4, 110);
   mLegs[5].init(CLeg::LEFT, 5, 6, 7, -80);


   mLegs[0].up();
   mLegs[1].up();
   mLegs[2].up(); 

   mLegs[3].up(); 
   mLegs[4].up(); 
   mLegs[5].up(); 
}

// ----------------------------------------------------------------------------

void CBody::test(void)
{
   static uint8_t i = 0;

   if (i==0)
   {
      mLegs[0].down();
      mLegs[1].down();
      mLegs[2].down(); 

      mLegs[3].down(); 
      mLegs[4].down(); 
      mLegs[5].down(); 

      mLegs[0].center();
      mLegs[1].center();
      mLegs[2].center(); 

      mLegs[3].center(); 
      mLegs[4].center(); 
      mLegs[5].center(); 
   }
   else if (i==1)
   {
      mLegs[0].up();
      mLegs[1].down();
      mLegs[2].up(); 

      mLegs[3].down(); 
      mLegs[4].up(); 
      mLegs[5].down(); 
   }
   else if (i==2)
   {
      mLegs[0].forward();
      mLegs[1].backward();
      mLegs[2].forward(); 

      mLegs[3].backward(); 
      mLegs[4].forward(); 
      mLegs[5].backward(); 
   }
   else if (i==3)
   {
      mLegs[0].down();
      mLegs[1].down();
      mLegs[2].down(); 

      mLegs[3].down(); 
      mLegs[4].down(); 
      mLegs[5].down(); 
   }
   else if (i==4)
   {
      mLegs[0].down();
      mLegs[1].up();
      mLegs[2].down(); 

      mLegs[3].up(); 
      mLegs[4].down(); 
      mLegs[5].up(); 
   }
   else if (i==5)
   {
      mLegs[0].backward();
      mLegs[1].forward();
      mLegs[2].backward(); 

      mLegs[3].forward(); 
      mLegs[4].backward(); 
      mLegs[5].forward(); 
   }
   else if (i==6)
   {
      mLegs[0].down();
      mLegs[1].down();
      mLegs[2].down(); 

      mLegs[3].down(); 
      mLegs[4].down(); 
      mLegs[5].down(); 
      
      i = 0;
   }
   else
   {
      
   }

   i++;
}


void CBody::test2(void)
{
   mLegs[0].push();
   mLegs[1].push();
   mLegs[2].push(); 

   mLegs[3].push(); 
   mLegs[4].push(); 
   mLegs[5].push(); 
}