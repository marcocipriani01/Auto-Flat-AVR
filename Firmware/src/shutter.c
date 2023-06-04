#include "shutter.h"

volatile uint16_t currentServoVal;
volatile uint16_t targetServoVal;
volatile ShutterStatus shutterStatus = CLOSED;

void initServo(ShutterStatus initialStatus) {
    TCCR1A = 0;                                     // Normal counting mode
    TCCR1B = TIMER1_CLOCK_SOURCE;                   // Prescaler bits
    TCNT1 = 0;                                      // Clear timer count
    TIFR1 |= _BV(OCF1A);                            // Clear pending interrupts
    OCR1A = 0xFFFF;                                 // Set the Output Compare Register to the maximum value
    TIMSK1 |= _BV(OCIE1A);                          // Enable the output compare interrupt
    DDRD |= _BV(PD5);                               // Set PD5 as output
    shutterStatus = initialStatus;
    uint16_t pulseWidth = (initialStatus == OPEN) ? settings.openVal : settings.closedVal;
    currentServoVal = targetServoVal = us_TO_TICKS(pulseWidth);
}

void setShutter(ShutterStatus val) {
    if ((val == OPEN) && (shutterStatus != OPEN)) {
        targetServoVal = us_TO_TICKS(settings.openVal);
        shutterStatus = MOVING;
    } else if ((val == CLOSED) && (shutterStatus != CLOSED)) {
        targetServoVal = us_TO_TICKS(settings.closedVal);
        shutterStatus = MOVING;
    }
}

inline void setServoTarget(uint16_t pulseWidth) {
    targetServoVal = us_TO_TICKS(pulseWidth);
}

ISR(TIMER1_COMPA_vect) {
    if (bit_is_set(PORTD, PD5)) {
        OCR1A = 0xFFFF;
        TCNT1 = 0;
    } else {
        if (currentServoVal > targetServoVal) {
            currentServoVal -= us_TO_TICKS(settings.servoStep);
            if (currentServoVal <= targetServoVal) {
                currentServoVal = targetServoVal;
                if (currentServoVal == us_TO_TICKS(settings.openVal))
                    shutterStatus = OPEN;
            }
        } else if (currentServoVal < targetServoVal) {
            currentServoVal += us_TO_TICKS(settings.servoStep);
            if (currentServoVal >= targetServoVal) {
                currentServoVal = targetServoVal;
                if (currentServoVal == us_TO_TICKS(settings.closedVal))
                    shutterStatus = CLOSED;
            }
        }
        
        OCR1A = currentServoVal;
        TCNT1 = 0;
    }
    // Toggle the servo pin
    PORTD ^= _BV(PD5);
}
