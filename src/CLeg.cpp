//***********************************************************************
// FILE: CLeg.cpp
// AUTHOR: Yuriy Skriyka
// DATE: 09-01-2012 
// DESCRIPTION: The class implements the basic logic in the leg. It provides basic
//    functions to move the leg.
//************************************************************************

#include "CLeg.h"
#include "CServosMgr.h"

//#define HIP_SERVO_CENTER      1500   
#define HIP_SERVO_PUSH        1600
#define HIP_SERVO_DOWN        1800
#define HIP_SERVO_UP          2000

//#define KNEE_SERVO_CENTER     1500
#define KNEE_SERVO_PUSH       1700
#define KNEE_SERVO_DOWN       1800
#define KNEE_SERVO_UP         1500

#define BASE_SERVO_FORWARD    1600     // 1800
#define BASE_SERVO_CENTER     1500
#define BASE_SERVO_BACKWARD   1400     // 1200


#define INVERT(pulse)   (3000 - (pulse))        // 3000 is double middle pulse 
#define SMART_INVERT(pulse)   (mSide == RIGHT) ? (pulse) : (INVERT(pulse))

// ----------------------------------------------------------------------------

void CLeg::init(eSide side, uint8_t baseServo, uint8_t hipServo, uint8_t kneeServo, int8_t trim)
{
   mBaseServo = baseServo;
   mHipServo = hipServo;
   mKneeServo = kneeServo;
   mSide = side;

   mTrim = trim;     // TODO: implement a calibration mechanism


   CServosMgr::setMicros(mBaseServo, SMART_INVERT(BASE_SERVO_CENTER));
   CServosMgr::setMicros(mHipServo, SMART_INVERT(HIP_SERVO_DOWN + mTrim));  
   CServosMgr::setMicros(mKneeServo, SMART_INVERT(KNEE_SERVO_DOWN));

}

// ----------------------------------------------------------------------------

void CLeg::up(void)
{
   CServosMgr::setMicros(mHipServo, SMART_INVERT(HIP_SERVO_UP + mTrim));  
   CServosMgr::setMicros(mKneeServo, SMART_INVERT(KNEE_SERVO_UP));
}

// ----------------------------------------------------------------------------

void CLeg::down(void)
{
   CServosMgr::setMicros(mHipServo, SMART_INVERT(HIP_SERVO_DOWN + mTrim));  
   CServosMgr::setMicros(mKneeServo, SMART_INVERT(KNEE_SERVO_DOWN));
}

// ----------------------------------------------------------------------------

void CLeg::push(void)
{
   CServosMgr::setMicros(mHipServo, SMART_INVERT(HIP_SERVO_PUSH + mTrim));  
   CServosMgr::setMicros(mKneeServo, SMART_INVERT(KNEE_SERVO_PUSH));
}

// ----------------------------------------------------------------------------

void CLeg::forward(void)
{
   CServosMgr::setMicros(mBaseServo, SMART_INVERT(BASE_SERVO_FORWARD));
}

// ----------------------------------------------------------------------------

void CLeg::center(void)
{
   CServosMgr::setMicros(mBaseServo, SMART_INVERT(BASE_SERVO_CENTER));
}

// ----------------------------------------------------------------------------

void CLeg::backward(void)
{
   CServosMgr::setMicros(mBaseServo, SMART_INVERT(BASE_SERVO_BACKWARD));
}

