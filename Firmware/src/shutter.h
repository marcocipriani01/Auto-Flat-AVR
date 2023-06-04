#ifndef SERVO_H
#define SERVO_H

#include "config.h"
#if SERVO_ENABLE == true

#include <stdbool.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "utils.h"
#include "alnitak.h"
#include "settings.h"

#define SERVO_PRESCALER 8
#if SERVO_PRESCALER != 1 && SERVO_PRESCALER != 8 && SERVO_PRESCALER != 64 && SERVO_PRESCALER != 256 && SERVO_PRESCALER != 1024
#error "Invalid prescaler value"
#endif
#if SERVO_PRESCALER == 1
#define TIMER1_CLOCK_SOURCE _BV(CS10)
#elif SERVO_PRESCALER == 8
#define TIMER1_CLOCK_SOURCE _BV(CS11)
#elif SERVO_PRESCALER == 64
#define TIMER1_CLOCK_SOURCE (_BV(CS11) | _BV(CS10))
#elif SERVO_PRESCALER == 256
#define TIMER1_CLOCK_SOURCE _BV(CS12)
#elif SERVO_PRESCALER == 1024
#define TIMER1_CLOCK_SOURCE _BV(CS12) | _BV(CS10)
#endif

#define CLOCKS_PER_us (F_CPU / 1000000L)
#define us_TO_TICKS(us) ((CLOCKS_PER_us * us) / SERVO_PRESCALER)

/**
 * The current servo value, in ticks of the timer.
*/
extern volatile uint16_t currentServoVal;

/**
 * The target servo value, in ticks of the timer.
*/
extern volatile uint16_t targetServoVal;

/**
 * The current status of the shutter (MOVING, CLOSED, OPEN).
*/
extern volatile ShutterStatus shutterStatus;

/**
 * Initializes the timer that sends signals to the servo motor.
*/
void initServo(ShutterStatus initialStatus);

/**
 * Sets the shutter to either open or closed.
*/
void setShutter(ShutterStatus val);

/**
 * Manually sets the target servo pulse width in 1μs.
*/
void setServoTarget(uint16_t pulseWidth);

#endif
#endif
