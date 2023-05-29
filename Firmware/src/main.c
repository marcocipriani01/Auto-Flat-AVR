#include "main.h"

bool lightOn = false;
// The value of the brightness of the panel as requested by the PC
uint8_t brightness = 255;
// The target value of the brightness of the panel (used for the fade effect)
uint8_t targetBrightness = 0;
// The current value of the brightness of the panel (used for the fade effect)
uint8_t currentBrightness = 0;

#ifdef SERVO_PIN
int motorStatus = STOPPED;
int coverStatus = CLOSED;
int motorDirection = NONE;
#endif

int main(void) {
    // Serial begin
    uartBegin(BAUDRATE);
    setCommandDelimiter('\r');
    setCommandHandler(onCommandReceived);

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    // EL panel brightness adjustment, PWM on pin 3
    DDRD |= _BV(PD3);
    timer2init();

    // Servo motor
    initServo(550);

    uint16_t pulseWidth = 550;
    while (1) {
        pulseWidth += 10;
        if (pulseWidth > 2500) pulseWidth = 550;
        setServoPulseWidth(pulseWidth);
        uartPrintlnInt(servoCounter);
        _delay_ms(50);
    }

    while (1) {
        bool canSleep = true;

        // EL panel fade effect
        if (currentBrightness > targetBrightness) {
            OCR2B = --currentBrightness;
            uartPrintlnInt(currentBrightness);
        }
        else if (currentBrightness < targetBrightness) {
            OCR2B = ++currentBrightness;
            uartPrintlnInt(currentBrightness);
        }
        else
            canSleep &= true;
        


        // Enter sleep mode
        if (canSleep)
            sleep_mode();
        else
            _delay_ms(EL_PANEL_FADE_DELAY);
    }
    return 0;
}

void timer2init() {
    // Phase-Correct PWM mode, non-inverting, timer 2
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
    // Prescaler 64, 490Hz
    //TCCR2B = _BV(CS22);
    // Prescaler 32, 980Hz
    TCCR2B = _BV(CS21) | _BV(CS20);
    // 0% PWM
    OCR2B = 0;
}

inline void setPanelBrigthness(uint8_t brightness) {
    OCR2B = brightness;
}

#ifdef SERVO_PIN
void setShutter(ShutterStatus val) {
    /*if (val == OPEN && coverStatus != OPEN) {
        motorDirection = OPENING;
        targetVal = settings.openVal;
    } else if (val == CLOSED && coverStatus != CLOSED) {
        motorDirection = CLOSING;
        targetVal = settings.closedVal;
    }*/
}
#endif

void onCommandReceived(CircBuffer* buffer) {
    uint8_t b;
    while (circBufferPop(buffer, &b) != BUFFER_EMPTY) {
        if (b != '>') continue;
        if (circBufferPop(buffer, &b) == BUFFER_EMPTY) return;
        switch ((char) b) {
            /*
              Ping device
              Request: >POOO\r
              Return : *PiiOOO\n
              id = deviceId
            */
            case 'P': {
                char temp[9];
                sprintf(temp, "*P%dOOO\n", DEVICE_ID);
                uartWrite((uint8_t*) temp, strlen(temp));
                break;
            }

            /*
              Turn light on
              Request: >LOOO\r
              Return : *LiiOOO\n
              id = deviceId
            */
            case 'L': {
                char temp[9];
                sprintf(temp, "*L%dOOO\n", DEVICE_ID);
                uartWrite((uint8_t*) temp, strlen(temp));
                lightOn = true;
#ifdef SERVO_PIN
                if (coverStatus == CLOSED)
                    targetBrightness = brightness;
#else
                targetBrightness = brightness;
#endif
                break;
            }

            /*
              Turn light off
              Request: >DOOO\r
              Return : *DiiOOO\n
              id = deviceId
            */
            case 'D': {
                char temp[9];
                sprintf(temp, "*D%dOOO\n", DEVICE_ID);
                uartWrite((uint8_t*) temp, strlen(temp));
                lightOn = false;
                targetBrightness = 0;
                break;
            }

            /*
              Set brightness
              Request: >Bxxx\r
               xxx = brightness value from 000-255
              Return : *Biiyyy\n
              id = deviceId
               yyy = value that brightness was set from 000-255
            */
            case 'B': {
                char data[3];
                if (circBufferPopArray(buffer, (uint8_t*) data, 3) == BUFFER_EMPTY) return;
#if LOG_SCALE
                brightness = (int) (exp(log(256.0) * ((double)(atoi(data) - 3) / 255.0)) - 1.0);
                brightness = constrain(brightness, 0, 255);
#else
                brightness = atoi(data);
#endif
#ifdef SERVO_PIN
                if (lightOn && (coverStatus == CLOSED))
#else
                if (lightOn)
#endif
                    targetBrightness = brightness;
                char temp[9];
                sprintf(temp, "*B%d%03d\n", DEVICE_ID, brightness);
                uartWrite((uint8_t*) temp, strlen(temp));
                break;
            }

#ifdef SERVO_PIN
            case 'O': {
                char temp[9];
                sprintf(temp, "*O%dOOO\n", DEVICE_ID);
                setShutter(OPEN);
                uartWrite((uint8_t*) temp, strlen(temp));
                // Turn off the panel when the shutter is open
                targetBrightness = 0;
                setPanelBrigthness(0);
                break;
            }
#endif 

            default:
                break;
        }
    }
}
