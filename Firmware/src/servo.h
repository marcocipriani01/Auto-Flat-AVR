#ifndef SERVO_H
#define SERVO_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define SERVO_PRESCALER 8
#define CLOCKS_PER_us (F_CPU / 1000000L)
#define us_TO_TICKS(us) ((CLOCKS_PER_us * us) / SERVO_PRESCALER)
#define TICKS_TO_us(ticks) ((ticks * SERVO_PRESCALER) / CLOCKS_PER_us)

extern volatile uint16_t servoTicks;
extern volatile uint16_t servoCounter;

uint8_t calcPrescalerBits(uint16_t prescaler);
void initServo(uint16_t pulseWidth);
void setServoPulseWidth(uint16_t pulseWidth);

#endif
