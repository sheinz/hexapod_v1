#ifndef __CLEG_H__
#define __CLEG_H__
//***********************************************************************
// FILE: CLeg.h
// AUTHOR: Yuriy Skriyka
// DATE: 09-01-2012 
// DESCRIPTION: The class implements the basic logic in the leg. It provides basic
//    functions to move the leg.
//************************************************************************

#include <inttypes.h>

class CLeg
{
public:

   enum eSide
   {
      LEFT,
      RIGHT
   };


   CLeg() {};     // empty default constructor

   // Init the leg and pass the indexes of the servos
   void init(eSide side, uint8_t baseServo, uint8_t hipServo, uint8_t kneeServo, int8_t trim);

   // Lift the leg up
   void up(void);

   // Put the leg down (to the ground and do not push)
   void down(void);

   // Put the leg lower than ground level. This leg will lift the entire body.
   void push(void);

   // Move the leg forward
   void forward(void);

   // Center the leg
   void center(void);

   // Move the leg backward
   void backward(void);

private:

   // The index of the servo that is attached to the main body
   uint8_t mBaseServo;

   // The index of the servo that is attached to the base servo and the leg itself
   uint8_t mHipServo;

   // The index of the servo which is located in the middle of the leg
   uint8_t mKneeServo;

   // The correction in order to keep all legs leveled
   int8_t mTrim;

   // Determines the side where the leg is located
   eSide mSide;
};

#endif   // __CLEG_H__