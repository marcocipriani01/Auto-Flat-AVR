#include "servo.h"

volatile bool servoHigh;
volatile uint16_t currentServoVal;
volatile uint16_t targetServoVal;
volatile uint16_t servoInterpolationStep;
volatile ShutterStatus shutterStatus = CLOSED;

void initServo(uint16_t pulseWidth) {
    servoHigh = false;
    TCCR1A = 0;                        // Normal counting mode
    TCCR1B = TIMER1_CLOCK_SOURCE;      // Prescaler 8
    TCNT1 = 0;                         // Clear timer count
    TIFR1 |= _BV(OCF1A);               // Clear pending interrupts
    OCR1A = 0xFFFF;                    // Set the Output Compare Register to the maximum value
    TIMSK1 |= _BV(OCIE1A);             // Enable the output compare interrupt
    DDRD |= _BV(PD5);                  // Set PD5 as output
    currentServoVal = targetServoVal = us_TO_TICKS(pulseWidth);
    servoInterpolationStep = 0;
}

inline void setServoTarget(uint16_t pulseWidth) {
    targetServoVal = us_TO_TICKS(pulseWidth);
}

void setShutter(ShutterStatus val) {
    if ((val == OPEN) && (shutterStatus != OPEN)) {
        setServoTarget(settings.openVal);
        shutterStatus = MOVING;
    } else if ((val == CLOSED) && (shutterStatus != CLOSED)) {
        setServoTarget(settings.closedVal);
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
        PORTD |= _BV(PD5);
        servoHigh = true;

        servoInterpolationStep++;
        if (servoInterpolationStep >= (us_TO_TICKS(settings.servoDelay) / 2)) {
            if (currentServoVal > targetServoVal) {
                currentServoVal -= us_TO_TICKS(SERVO_STEP_SIZE);
                if (currentServoVal <= targetServoVal) {
                    currentServoVal = targetServoVal;
                    shutterStatus = OPEN;
                    //settings.shutterStatus = OPEN;
                    //saveSettings();
                }
            } else if (currentServoVal < targetServoVal) {
                currentServoVal += us_TO_TICKS(SERVO_STEP_SIZE);
                if (currentServoVal >= targetServoVal) {
                    currentServoVal = targetServoVal;
                    shutterStatus = CLOSED;
                    //settings.shutterStatus = CLOSED;
                    //saveSettings();
                    //if (lightOn) targetBrightness = brightness;
                }
            }
            servoInterpolationStep = 0;
        }
        
        OCR1A = currentServoVal;
        TCNT1 = 0;
    }
}
