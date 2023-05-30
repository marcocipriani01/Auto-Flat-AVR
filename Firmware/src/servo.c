#include "servo.h"

volatile bool servoHigh;
volatile uint16_t targetServoTicks;
volatile uint16_t currentServoTicks;
volatile double servoSmoothFactor;

void initServo(uint16_t pulseWidth, double smoothFactor) {
    servoHigh = false;
    TCCR1A = 0;                        // Normal counting mode
    TCCR1B = TIMER1_CLOCK_SOURCE;      // Prescaler 8
    TCNT1 = 0;                         // Clear timer count
    TIFR1 |= _BV(OCF1A);               // Clear pending interrupts
    OCR1A = 0xFFFF;                    // Set the Output Compare Register to the maximum value
    TIMSK1 |= _BV(OCIE1A);             // Enable the output compare interrupt
    DDRD |= _BV(PD5);                  // Set PD5 as output
    setServoPulseWidth(pulseWidth);
    currentServoTicks = targetServoTicks;
    servoSmoothFactor = smoothFactor;
}

void setServoPulseWidth(uint16_t pulseWidth) {
    targetServoTicks = us_TO_TICKS(pulseWidth);
}

ISR(TIMER1_COMPA_vect) {
    if (servoHigh) {
        PORTD &= ~_BV(PD5);
        servoHigh = false;
        OCR1A = 0xFFFF;
        TCNT1 = 0;
    } else {
        PORTD |= _BV(PD5);
        servoHigh = true;
        currentServoTicks = (uint16_t) ((currentServoTicks * (1.0 - servoSmoothFactor)) + (targetServoTicks * servoSmoothFactor));
        if (abs(currentServoTicks - targetServoTicks) < 2) {
            currentServoTicks = targetServoTicks;
        }
        OCR1A = currentServoTicks;
        TCNT1 = 0;
    }
}
