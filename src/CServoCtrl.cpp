#include "CServoCtrl.hpp"

#include <avr/interrupt.h>

#include <stdio.h>


CServoCtrl ServoCtrl;

// ----------------------------------------------------------------------------

#define SERVO_PORT_0    PORTB   
#define SERVO_PORT_1    PORTC
#define SERVO_PORT_2    PORTD

#define SERVO_DDR_0     DDRB
#define SERVO_DDR_1     DDRC
#define SERVO_DDR_2     DDRD

#define PRESCALER_1MS      _BV(CS01) | _BV(CS00)         //prescaler = F_CPU/64
#define PRESCALER_VALUE    _BV(CS01) | _BV(CS00)         //prescaler = F_CPU/64
#define PRESCALER_SLOW     _BV(CS02) | _BV(CS00)        // prescaler = F_CPU/1024

// obsolete
#define INITIAL_1MS_LENGH     0xFF        // 0xFF means 1.024 ms

// it means that 1ms initial pulse is being formed
#define CURRENT_SERVO_INITIAL_1MS   0xFF         

// ----------------------------------------------------------------------------

#define SERVO_DELAY_OFFSET       0


#define  ALL_SERVO_PINS_HIGH     SERVO_PORT_0 = 0xFF; \
                                 SERVO_PORT_1 = 0xFF; \
                                 SERVO_PORT_2 |= (1<<4)|(1<<5)|(1<<6);    // set all servo pins to 1 (* on "port_2" only 4, 5, 6 pins are used)


#define TIMER_OVERFLOW_ENABLE       TIMSK |= _BV(TOIE0); TIMSK &= ~_BV(OCIE0)
#define TIMER_COMPARE_ENABLE        TIMSK |= _BV(OCIE0); TIMSK &= ~_BV(TOIE0)

struct TServoTiming
{
   uint8_t port_0_mask;
   uint8_t port_1_mask;
   uint8_t port_2_mask;

   uint8_t delay;
};

struct TServoValue
{
   uint8_t servo_index;
   uint8_t servo_value;
};

// ----------------------------------------------------------------------------

static TServoTiming servoTimingList[MAX_SERVOS];
static volatile uint8_t currentServo;        // 0..MAX_SERVOS-1
static volatile bool needsUpdate;      // true if mServoValueList has been changed and servoTimingList needs update

static volatile uint8_t errorCounter = 0;


TServoValue servoValueList[MAX_SERVOS];
TServoValue* pServoValueSortedList[MAX_SERVOS];

// ----------------------------------------------------------------------------

inline void updateServoList(void);

ISR(TIMER0_OVF_vect)
{
   TCCR0 = 0;  // stop the timer
   
   if (currentServo == CURRENT_SERVO_INITIAL_1MS)
   {   
      if (needsUpdate)
      {
         updateServoList();		
         needsUpdate = false;	
      }

      ALL_SERVO_PINS_HIGH;
      
      currentServo = 0;  // set first servo
      
      TCNT0 = 0;  // reset timer
      TIMER_OVERFLOW_ENABLE;
      TCCR0 = PRESCALER_1MS;     // start timer for 1ms
   }
   else
   {  
      // first cycle
      OCR0 = servoTimingList[currentServo].delay;

      currentServo++;      
      TCNT0 = 0;  // reset timer
      TIMER_COMPARE_ENABLE;
      TCCR0 = PRESCALER_VALUE;      // start timer
   }
}

// ----------------------------------------------------------------------------

ISR(TIMER0_COMP_vect)
{    
   OCR0 = servoTimingList[currentServo].delay;	
   //if ( OCR0 != 0xFF && TCNT0 >= OCR0)
   //{
   //   TCNT0 = OCR0 - 1;
   //   errorCounter++;
   //}

   SERVO_PORT_0 &= ~(servoTimingList[currentServo-1].port_0_mask);
   SERVO_PORT_1 &= ~(servoTimingList[currentServo-1].port_1_mask);
   SERVO_PORT_2 &= ~(servoTimingList[currentServo-1].port_2_mask);

   currentServo++;

   if ( (OCR0 == 0xFF) || (currentServo >= MAX_SERVOS) )
   {	
      TCCR0 = 0;  // stop the timer
      TIMER_OVERFLOW_ENABLE;
      TCNT0 = 0;  // reset timer
      TCCR0 = PRESCALER_SLOW;    // start timer

      // next pulse will be initial 1ms pulse which is common for all servos
      currentServo = CURRENT_SERVO_INITIAL_1MS;
   }
}

// ----------------------------------------------------------------------------

// Bubble sort
void sortServoList(void)
{
   TServoValue* pTemp;

   for (uint8_t i = (MAX_SERVOS - 1); i > 0; i--)
   {
      for (uint8_t j = 1; j <= i; j++)
      {
         if (pServoValueSortedList[j-1]->servo_value > pServoValueSortedList[j]->servo_value)
         {
            pTemp = pServoValueSortedList[j-1];
            pServoValueSortedList[j-1] = pServoValueSortedList[j];
            pServoValueSortedList[j] = pTemp;
         }
      }
   }
}

// ----------------------------------------------------------------------------

void addPortMask(uint8_t servo_index, uint8_t pin_index)
{
   if (pin_index < 8)
   {
      servoTimingList[servo_index].port_0_mask |= 1<<pin_index;
   }
   else if (pin_index < 16)
   {
      servoTimingList[servo_index].port_1_mask |= 1<<(pin_index-8);
   }
   else if (pin_index < 24)
   {
      servoTimingList[servo_index].port_2_mask |= 1<<(4 + pin_index-16);      // In "port_2" the real pins start from 4
   }
}

// ----------------------------------------------------------------------------

void clearPortMasks(void)
{
   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      servoTimingList[i].port_0_mask = 0;
      servoTimingList[i].port_1_mask = 0;
      servoTimingList[i].port_2_mask = 0;
   }
}

// ----------------------------------------------------------------------------

inline void updateServoList(void)
{
   clearPortMasks(); // clear all masks

   servoTimingList[0].delay = pServoValueSortedList[0]->servo_value + SERVO_DELAY_OFFSET;  // first delay is always from the firs servo in the sorted list
   addPortMask(0, pServoValueSortedList[0]->servo_index);      // add mask for the first servo

   uint8_t lastUniqueServoIndex = 0;      // mark it as the first unique
   for (uint8_t i = 1; i < MAX_SERVOS; i++)     // start with the next servo
   {
      if (pServoValueSortedList[lastUniqueServoIndex]->servo_value == pServoValueSortedList[i]->servo_value)
      {     // if the next servo has the same value than just add its port mask 
         addPortMask(lastUniqueServoIndex, pServoValueSortedList[i]->servo_index);
      }
      else
      {  // if the next servo has unique value than go to the next position and mark it as an unique servo
         lastUniqueServoIndex++;
         servoTimingList[lastUniqueServoIndex].delay = pServoValueSortedList[i]->servo_value + SERVO_DELAY_OFFSET;
         addPortMask(lastUniqueServoIndex, pServoValueSortedList[i]->servo_index);
      }
   }

   if ( (lastUniqueServoIndex+1) < MAX_SERVOS ) // there is less records than number of servos
   {     // mark the end of list
      servoTimingList[lastUniqueServoIndex+1].delay = 0xFF;
   }
}

// ----------------------------------------------------------------------------

void CServoCtrl::init(void)
{
   TIMER_OVERFLOW_ENABLE;

   SERVO_DDR_0 = 0xFF;
   SERVO_DDR_1 = 0xFF;
   SERVO_DDR_2 |= (1<<4)|(1<<5)|(1<<6); // only pins 4, 5, 6 are used

   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      servoValueList[i].servo_index = i;
      servoValueList[i].servo_value = 128;      // initial servo position  128 - somewhere in the middle
      pServoValueSortedList[i] = &(servoValueList[i]);
   }
   // don't need to sort the list because all elements are equal
   updateServoList();
   
   needsUpdate = false;

   // start by forming 1ms pulse which is common for all servos  
   currentServo = 0;
   TCNT0 = 0;           // reset timer
   OCR0 = INITIAL_1MS_LENGH;         // overflows in 1ms
   
   ALL_SERVO_PINS_HIGH;    // set front of the 1ms pulse
   
   TCCR0 = PRESCALER_1MS;     // start timer
}

// ----------------------------------------------------------------------------

void CServoCtrl::setServo(uint8_t servo_index, uint8_t servo_value)
{
   if (needsUpdate)
   {
      return;
   }

   if (servo_value > MAX_SERVO_VALUE)
   {
      servo_value = MAX_SERVO_VALUE;
   }
   else if (servo_value < MIN_SERVO_VALUE)
   {
      servo_value = MIN_SERVO_VALUE;
   }

   if (servo_index < MAX_SERVOS)
   {
      servoValueList[servo_index].servo_value = servo_value;
   }
   
   sortServoList();
   needsUpdate = true;

   //for (uint8_t i = 0; i < MAX_SERVOS; i++)
   //{
   //   printf("Index: %d\t", pServoValueSortedList[i]->servo_index);
   //   printf("Value: %d\n", pServoValueSortedList[i]->servo_value);
   //}
   //printf("\n");

   //updateServoList();

   //for (uint8_t i = 0; i < MAX_SERVOS && servoTimingList[i].delay != 0xFF; i++)
   //{
   //   printf("Delay %d\t", servoTimingList[i].delay);
   //   printf("port_0 %X\t", servoTimingList[i].port_0_mask);
   //   printf("port_1 %X\t", servoTimingList[i].port_1_mask);
   //   printf("port_2 %X\n", servoTimingList[i].port_2_mask);
   //}

   printf("Errors: %d\n", errorCounter);

   printf("\n");
}

// ----------------------------------------------------------------------------

void CServoCtrl::setAllServo(uint8_t servo_value)
{
   if (servo_value > MAX_SERVO_VALUE)
   {
      servo_value = MAX_SERVO_VALUE;
   }
   else if (servo_value < MIN_SERVO_VALUE)
   {
      servo_value = MIN_SERVO_VALUE;
   }

   if (needsUpdate)
   {
      return;
   }

   for (uint8_t i = 0; i < MAX_SERVOS; i++)
   {
      servoValueList[i].servo_value = servo_value;
   }

   // don't need to sort
   needsUpdate = true;
}

// ----------------------------------------------------------------------------

uint8_t CServoCtrl::getServo(uint8_t servo_index)
{
   if (servo_index < MAX_SERVOS)
   {
      return servoValueList[servo_index].servo_value;
   }

   return 0;      // index is out of range
}
