#include "servo.h"

volatile uint16_t servoTicks;
volatile uint8_t servoHigh;

uint8_t calcPrescalerBits(uint16_t prescaler) {
    switch (prescaler) {
        case 1:
            return _BV(CS10);
        case 8:
            return _BV(CS11);
        case 64:
            return _BV(CS11) | _BV(CS10);
        case 256:
            return _BV(CS12);
        case 1024:
            return _BV(CS12) | _BV(CS10);
    }
    return 0;
}

void initServo(uint16_t pulseWidth) {
    servoHigh = 0;
    TCCR1A = 0;                                   // Normal counting mode
    TCCR1B = calcPrescalerBits(SERVO_PRESCALER);  // Prescaler
    TCNT1 = 0;                                    // Clear timer count
    TIFR1 |= _BV(OCF1A);                          // Clear pending interrupts
    OCR1A = 0xFFFF;                               // Set the Output Compare Register to the maximum value
    TIMSK1 |= _BV(OCIE1A);                        // Enable the output compare interrupt
    DDRD |= _BV(PD5);                             // Set PD5 as output
    setServoPulseWidth(pulseWidth);
}

void setServoPulseWidth(uint16_t pulseWidth) {
    servoTicks = us_TO_TICKS(pulseWidth);
}

ISR(TIMER1_COMPA_vect) {
    if (servoHigh == 0) {
        PORTD |= _BV(PD5);
        servoHigh = 1;
        OCR1A = servoTicks;
        TCNT1 = 0;
    } else {
        PORTD &= ~_BV(PD5);
        servoHigh = 0;
        OCR1A = 0xFFFF;
        TCNT1 = 0;
    }
}
