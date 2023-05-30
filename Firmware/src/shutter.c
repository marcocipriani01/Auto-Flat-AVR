#include "shutter.h"

volatile bool servoHigh;
volatile uint16_t currentServoVal;
volatile uint16_t targetServoVal;
volatile uint16_t servoInterpolationTicks;
volatile ShutterStatus shutterStatus = CLOSED;

void initServo(ShutterStatus initialStatus) {    
    servoHigh = false;
    TCCR1A = 0;                        // Normal counting mode
    TCCR1B = TIMER1_CLOCK_SOURCE;      // Prescaler 8
    TCNT1 = 0;                         // Clear timer count
    TIFR1 |= _BV(OCF1A);               // Clear pending interrupts
    OCR1A = 0xFFFF;                    // Set the Output Compare Register to the maximum value
    TIMSK1 |= _BV(OCIE1A);             // Enable the output compare interrupt
    DDRD |= _BV(PD5);                  // Set PD5 as output
    uint16_t pulseWidth = (initialStatus == OPEN) ? settings.openVal : settings.closedVal;
    currentServoVal = targetServoVal = us_TO_TICKS(pulseWidth);
    servoInterpolationTicks = 0;
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

ISR(TIMER1_COMPA_vect) {
    if (servoHigh) {
        PORTD &= ~_BV(PD5);
        servoHigh = false;
        OCR1A = 0xFFFF;
        TCNT1 = 0;
    } else {
        servoInterpolationTicks++;
        if (servoInterpolationTicks >= (us_TO_TICKS(settings.servoDelay) / 2)) {
            if (currentServoVal > targetServoVal) {
                currentServoVal -= us_TO_TICKS(SERVO_STEP_SIZE);
                if (currentServoVal <= targetServoVal) {
                    currentServoVal = targetServoVal;
                    shutterStatus = OPEN;
                }
            } else if (currentServoVal < targetServoVal) {
                currentServoVal += us_TO_TICKS(SERVO_STEP_SIZE);
                if (currentServoVal >= targetServoVal) {
                    currentServoVal = targetServoVal;
                    shutterStatus = CLOSED;
                }
            }
            servoInterpolationTicks = 0;
        }
        
        servoHigh = true;
        PORTD |= _BV(PD5);
        OCR1A = currentServoVal;
        TCNT1 = 0;
    }
}
