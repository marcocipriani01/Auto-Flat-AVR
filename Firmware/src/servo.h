#ifndef SERVO_H
#define SERVO_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define SERVO_PRESCALER 8
#if SERVO_PRESCALER != 1 && SERVO_PRESCALER != 8 && SERVO_PRESCALER != 64 && SERVO_PRESCALER != 256 && SERVO_PRESCALER != 1024
#error "Invalid prescaler value"
#endif
#define CLOCKS_PER_us (F_CPU / 1000000L)
#define us_TO_TICKS(us) ((CLOCKS_PER_us * us) / SERVO_PRESCALER)
#define TICKS_TO_us(ticks) ((ticks * SERVO_PRESCALER) / CLOCKS_PER_us)

extern volatile uint16_t servoTicks;
extern volatile uint8_t servoHigh;

uint8_t calcPrescalerBits(uint16_t prescaler);
void initServo(uint16_t pulseWidth);
void setServoPulseWidth(uint16_t pulseWidth);

#endif
