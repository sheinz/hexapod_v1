#ifndef __CSERVOCTRL_HPP__
#define __CSERVOCTRL_HPP__

#include <inttypes.h>

class CServoCtrl;  // forward declaration
extern CServoCtrl ServoCtrl; // only one object, and it is defined here

// ----------------------------------------------------------------------------

const static uint8_t    MAX_SERVOS = 19;  // the number of servos
const static uint8_t    MAX_SERVO_VALUE = 0xFF; 
const static uint8_t    MIN_SERVO_VALUE = 0x00; 


class CServoCtrl
{
private:

   void sortServoValueList(void);

public:

   void init(void);

   void setServo(uint8_t servo_index, uint8_t servo_value);

   void setAllServo(uint8_t servo_value);

   uint8_t getServo(uint8_t servo_index);
   
};



#endif // __CSERVOCTRL_HPP__