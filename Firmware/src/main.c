#include "main.h"

bool lightOn = false;
// The value of the brightness of the panel as requested by the PC
uint8_t brightness = 255;
// The target value of the brightness of the panel (used for the fade effect)
uint8_t targetBrightness = 0;
// The current value of the brightness of the panel (used for the fade effect)
uint8_t currentBrightness = 0;

#if SERVO_ENABLE == true
ShutterStatus shutterStatus = CLOSED;
uint16_t targetServoVal = 0;
uint16_t currentServoVal = 0;
#endif

int main(void) {
    // Serial begin
    uartBegin(BAUDRATE);
    setCommandDelimiter('\r');
    setCommandHandler(onCommandReceived);

    // Load settings from EEPROM
    loadSettings();

    // EL panel, PWM on pin PD3
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);    // Phase-Correct PWM mode, non-inverting, timer 2
    TCCR2B = _BV(CS21) | _BV(CS20);                     // Prescaler 32, 980Hz
    DDRD |= _BV(PD3);                                   // Set pin PD3 as output
    setPanelBrigthness(settings.brightness);            // Load brightness from settings

    // Servo motor
    if (settings.shutterStatus == OPEN) {
        initServo(settings.openVal);
        currentServoVal = targetServoVal = settings.openVal;
        shutterStatus = OPEN;
    } else {
        initServo(settings.closedVal);
        currentServoVal = targetServoVal = settings.closedVal;
    }

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    _delay_ms(100);

    while (1) {
        char temp[120];
        sprintf(temp, "This is a very long text to the the circular buffer of the UART library! Very very long text indeed.\n");
        print(temp);
        _delay_ms(1000);
    }
    

    while (1) {
        //bool canSleep = true;
        bool canSleep = false;

        // EL panel fade effect
        if (currentBrightness > targetBrightness)
            setPanelBrigthness(--currentBrightness);
        else if (currentBrightness < targetBrightness)
            setPanelBrigthness(++currentBrightness);
        else
            canSleep &= true;

        if (currentServoVal > targetServoVal) {
            currentServoVal -= SERVO_STEP_SIZE;
            setServoPulseWidth(currentServoVal);
            if (currentServoVal <= targetServoVal) {
                currentServoVal = targetServoVal;
                shutterStatus = OPEN;
                settings.shutterStatus = OPEN;
                saveSettings();
            }
            _delay_ms(settings.servoDelay);
        } else if (currentServoVal < targetServoVal) {
            currentServoVal += SERVO_STEP_SIZE;
            setServoPulseWidth(currentServoVal);
            if (currentServoVal >= targetServoVal) {
                currentServoVal = targetServoVal;
                shutterStatus = CLOSED;
                settings.shutterStatus = CLOSED;
                saveSettings();
                if (lightOn) targetBrightness = brightness;
            }
            _delay_ms(settings.servoDelay);
        }

        // Enter sleep mode
        if (canSleep)
            sleep_mode();
        else
            _delay_ms(EL_PANEL_FADE_DELAY);
    }
    return 0;
}

inline void setPanelBrigthness(uint8_t brightness) {
    OCR2B = brightness;
}

#if SERVO_ENABLE == true
void setShutter(ShutterStatus val) {
    if ((val == OPEN) && (shutterStatus != OPEN)) {
        targetServoVal = settings.openVal;
        shutterStatus = MOVING;
    } else if ((val == CLOSED) && (shutterStatus != CLOSED)) {
        targetServoVal = settings.closedVal;
        shutterStatus = MOVING;
    }
}
#endif

void onCommandReceived(CircBuffer* buffer) {
    uint8_t b;
    while (circBufferPop(buffer, &b) != BUFFER_EMPTY) {
        if (b != '>') continue;
        if (circBufferPop(buffer, &b) == BUFFER_EMPTY) return;
        char temp[20];

        switch ((char) b) {
            /*
             *  Ping device
             *  Request: >POOO\r
             *  Return : *PiiOOO\n
             *      ii = deviceId
            */
            case 'P': {
                sprintf(temp, "*P%dOOO\n", DEVICE_ID);
                print(temp);
                break;
            }

#if SERVO_ENABLE == true
            /*
             *  Open shutter
             *  Request: >OOOO\r
             *  Return : *OiiOOO\n
             *      ii = deviceId
            */
            case 'O': {
                sprintf(temp, "*O%dOOO\n", DEVICE_ID);
                print(temp);
                setShutter(OPEN);
                targetBrightness = 0;
                currentBrightness = 0;
                setPanelBrigthness(0);
                break;
            }

            /*
             *  Close shutter
             *  Request: >COOO\r
             *  Return : *CiiOOO\n
             *      ii = deviceId
            */
            case 'C': {
                sprintf(temp, "*C%dOOO\n", DEVICE_ID);
                print(temp);
                setShutter(CLOSED);
                break;
            }
#endif

            /*
             *  Turn light on
             *  Request: >LOOO\r
             *  Return : *LiiOOO\n
             *      ii = deviceId
            */
            case 'L': {
                sprintf(temp, "*L%dOOO\n", DEVICE_ID);
                print(temp);
                lightOn = true;
#if SERVO_ENABLE == true
                if (shutterStatus == CLOSED)
                    targetBrightness = brightness;
#else
                targetBrightness = brightness;
#endif
                break;
            }

            /*
             *  Turn light off
             *  Request: >DOOO\r
             *  Return : *DiiOOO\n
             *      ii = deviceId
            */
            case 'D': {
                sprintf(temp, "*D%dOOO\n", DEVICE_ID);
                print(temp);
                lightOn = false;
                targetBrightness = 0;
                break;
            }

            /*
             *  Set brightness
             *  Request: >Bxxx\r
             *      xxx = brightness value from 000-255
             *  Return : *Biiyyy\n
             *      ii = deviceId
             *      yyy = value that brightness was set from 000-255
            */
            case 'B': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 3) == BUFFER_EMPTY) return;
#if EL_PANEL_LOG_SCALE == true
                brightness = (int) round(exp(log(256.0) * (((double) atoi(temp)) / 255.0)) - 1.0);
                brightness = constrain(brightness, 0, 255);
#else
                brightness = atoi(temp);
#endif
#if SERVO_ENABLE == true
                if (lightOn && (shutterStatus == CLOSED))
#else
                if (lightOn)
#endif
                    targetBrightness = brightness;
                sprintf(temp, "*B%d%03d\n", DEVICE_ID, brightness);
                print(temp);
                break;
            }

            /*
             *  Get brightness
             *  Request: >JOOO\r
             *  Return : *Jiiyyy\n
             *      ii = deviceId
             *      yyy = current brightness value from 000-255
            */
            case 'J': {
                sprintf(temp, "*J%d%03d\n", DEVICE_ID, targetBrightness);
                print(temp);
                break;
            }

            /*
             *  Get device status
             *  Request: >SOOO\r
             *  Return : *SiiMLC\n
             *      ii = deviceId
             *      M  = motor status (0 stopped, 1 running)
             *      L  = light status (0 off, 1 on)
             *      C  = Cover Status (0 moving, 1 closed, 2 open, 3 error)
            */
            case 'S': {
#if SERVO_ENABLE == true
                sprintf(temp, "*S%d%d%d%d\n", DEVICE_ID, (shutterStatus == MOVING), lightOn, shutterStatus);
#else
                sprintf(temp, "*S%d0%d0\n", DEVICE_ID, lightOn);
#endif
                print(temp);
                break;
            }

            /*
             *  Get firmware version
             *  Request: >VOOO\r
             *  Return : *Vii001\n
             *      ii = deviceId
            */
            case 'V': {
                sprintf(temp, "*V%d%03d\n", DEVICE_ID, FIRMWARE_VERSION);
                print(temp);
                break;
            }

#if SERVO_ENABLE == true
            /*
             *  Get the servo open value (unofficial command)
             *  Request: >Qxxx\r
             *      xxx = servo open value, 0-100
             *  Return : *Qiiyyy\n
             *      ii  = deviceId
             *      yyy = the servo pulse width for the open position
            */
            case 'Q': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 3) == BUFFER_EMPTY) break;
                settings.openVal = linearMap(atoi(temp), 0, 100, SERVO_OPEN_MAX, SERVO_OPEN_MIN);
                sprintf(temp, "*Q%d%04d\n", DEVICE_ID, settings.openVal);
                print(temp);
                break;
            }

            /*
             *  Get the servo closed value (unofficial command)
             *  Request: >Kxxx\r
             *      xxx = servo closed value, 0-100
             *  Return : *Kiiyyy\n
             *      ii  = deviceId
             *      yyy = the servo pulse width for the closed position
            */
            case 'K': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 3) == BUFFER_EMPTY) break;
                settings.closedVal = linearMap(atoi(temp), 0, 100, SERVO_CLOSED_MAX, SERVO_CLOSED_MIN);
                sprintf(temp, "*K%d%04d\n", DEVICE_ID, settings.closedVal);
                print(temp);
                break;
            }

            /*
             * Get the servo speed (unofficial command)
             * Request: >Zxxx\r
             *    xxx = servo speed, 0-10
             *  Return : *Qiiyyy\n
             *      ii  = deviceId
             *      yyy = the servo step delay
            */
            case 'Z': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 2) == BUFFER_EMPTY) break;
                settings.servoDelay = linearMap(atoi(temp), 0, 10, SERVO_DELAY_MAX, SERVO_DELAY_MIN);
                sprintf(temp, "*Z%d%02d\n", DEVICE_ID, settings.servoDelay);
                print(temp);
                break;
            }

            /*
             *  Save settings to EEPROM (unofficial command)
             *  Request: >YOOO\r
             *  Return : *YiiOOO\n
             *      ii = deviceId
            */
            case 'Y': {
                saveSettings();
                sprintf(temp, "*Y%dOOO\n", DEVICE_ID);
                print(temp);
                break;
            }
#endif

            /*
             *  Get settings (unofficial command)
             *  Request: >TOOO\r
             *  Return : *Tuuuvvvxxywzzz\n
             *      uuu = servo open value, 0-100
             *      vvv = servo closed value, 0-100
             *      xx  = servo speed, 0-10
             *      y   = shutter status, 0 = moving, 1 = closed, 2 = open
             *      w   = light status, 0-1
             *      zzz = brightness value, 0-255
            */
            case 'T': {
#if SERVO_ENABLE == true
                uint8_t openVal = linearMap(settings.openVal, SERVO_OPEN_MAX, SERVO_OPEN_MIN, 0, 100),
                    closedVal = linearMap(settings.closedVal, SERVO_CLOSED_MAX, SERVO_CLOSED_MIN, 0, 100),
                    servoDelay = linearMap(settings.servoDelay, SERVO_DELAY_MAX, SERVO_DELAY_MIN, 0, 10),
                    shutterVal = (uint8_t) shutterStatus;
#else
                uint8_t openVal = 0,
                    closedVal = 0,
                    servoDelay = 0,
                    shutterStatus = 0;
#endif
                sprintf(temp, "*T%03d%03d%02d%d%d%03d\n", openVal, closedVal, servoDelay, shutterVal, lightOn, brightness);
                print(temp);
                break;
            }
        }
    }
}
