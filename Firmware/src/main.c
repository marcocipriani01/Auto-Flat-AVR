#include "main.h"

bool lightOn = false;
uint8_t brightness = 255;           // The brightness value of the panel as requested by the computer
uint8_t targetBrightness = 0;       // The target brightness value (used for the fade effect)
uint8_t currentBrightness = 0;      // The current brightness value (used for the fade effect)

#if SERVO_ENABLE == true
ShutterStatus lastShutterStatus = CLOSED;
#endif

int main(void) {
    // UART begin
    uartBegin(BAUDRATE, UART_BUFFERS_SIZE);
    setCommandHandler(onCommandReceived, '\r');

    // Load settings from EEPROM
    loadSettings();

    // EL panel, PWM on pin PD3
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);    // Phase-Correct PWM mode, non-inverting, timer 2
    TCCR2B = _BV(CS21) | _BV(CS20);                     // Prescaler 32, 980Hz
    DDRD |= _BV(PD3);                                   // Set pin PD3 as output
    setPanelBrigthness(settings.brightness);            // Load brightness from settings

    // Servo motor
    initServo(lastShutterStatus = settings.shutterStatus);

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1) {
        // Check if the shutter status has changed, then save it to EEPROM
        if (lastShutterStatus != shutterStatus) {
            if (shutterStatus == CLOSED) {
                settings.shutterStatus = shutterStatus;
                saveSettings();
                // If the shutter is closed, the light can be turned on
                if (lightOn) targetBrightness = brightness;
            } else if (shutterStatus == OPEN) {
                settings.shutterStatus = shutterStatus;
                saveSettings();
            }
            lastShutterStatus = shutterStatus;
        }

        // EL panel fade effect
        if (currentBrightness > targetBrightness) {
            setPanelBrigthness(--currentBrightness);
            _delay_ms(EL_PANEL_FADE_DELAY);
        } else if (currentBrightness < targetBrightness) {
            setPanelBrigthness(++currentBrightness);
            _delay_ms(EL_PANEL_FADE_DELAY);
        } else {
            sleep_mode();
        } 
    }
    return 0;
}

inline void setPanelBrigthness(uint8_t brightness) {
    OCR2B = brightness;
}

void onCommandReceived(CircBuffer* buffer) {
    uint8_t b;
    while (circBufferPop(buffer, &b) != BUFFER_EMPTY) {
        if (b != '>') continue;
        if (circBufferPop(buffer, &b) == BUFFER_EMPTY) return;
        char temp[20] = {};

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
             *      ii  = deviceId
             *      yyy = value that brightness was set from 000-255
            */
            case 'B': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 3) == BUFFER_EMPTY) return;
#if EL_PANEL_LOG_SCALE == true
                // Apply a logaritmic scale to the brightness value
                brightness = constrain((int) round(exp(log(256.0) * (((double) atoi(temp)) / 255.0)) - 1.0), 0, 255);
#else
                brightness = constrain(atoi(temp), 0, 255);
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
             *  Set the servo open value (unofficial command)
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
             *  Set the servo closed value (unofficial command)
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
             * Set the servo speed (unofficial command)
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
