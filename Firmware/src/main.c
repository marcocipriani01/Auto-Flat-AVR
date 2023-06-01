#include "main.h"

bool lightOn = false;
uint8_t targetBrightness = 0;

#if ENCODER_ENABLE == true
bool lastEncoderClk = false;
bool lastEncoderBtn = false;
#endif

int main(void) {
    // UART begin
    uartBegin(BAUDRATE, UART_BUFFERS_SIZE);
    setCommandHandler(onCommandReceived, '\r');

    // Load settings from EEPROM
    loadSettings();

#if SERVO_ENABLE == true
    // Servo motor
    initServo(settings.shutterStatus);
#endif

    // EL panel, PWM on pin PD3
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);      // Phase-Correct PWM mode, non-inverting, timer 2
    TCCR2B = _BV(CS21) | _BV(CS20);                       // Prescaler 32, 980Hz
    DDRD |= _BV(PD3);                                     // Set pin PD3 as output
    setPanelBrigthness(settings.brightness);              // Load brightness from settings

    // Rotary brightness encoder
#if ENCODER_ENABLE == true
    DDRB &= (~ _BV(PB0)) & (~ _BV(PB1)) & (~ _BV(PB2));   // Set PB0, PB1 and PB2 as input
    PORTB |= _BV(PB0);                                    // Enable pull-up resistor on PB0 (encoder button)
    PCICR |= _BV(PCIE0);                                  // Enable pin change interrupt PCIE0
    PCMSK0 |= _BV(PCINT1);                                // Enable pin change interrupt on PB1 (encoder clock)
#endif

    // Timer 0, used for the brightness fade effect and the encoder button
    TCCR0A = 0x00;                                        // Normal mode, timer 0
    TCCR0B = _BV(CS02) | _BV(CS00);                       // Prescaler 1024, 64μs period at 16MHz clock
    OCR0A = (F_CPU / 1024) / (255 * 2);                   // OCR0A value to call the ISR every ~2ms. This makes the fade effect last 500ms
#if ENCODER_ENABLE == true
    TIFR0 |= _BV(OCF0A) | _BV(TOV0);                      // Clear timer 0 Output Compare Match A flag and overflow flag
    TIMSK0 |= _BV(OCIE0A) | _BV(TOIE0);                   // Enable timer 0 Output Compare Match interrupt and overflow interrupt
#else
    TIFR0 |= _BV(OCF0A);                                  // Clear timer 0 Output Compare Match A flag
    TIMSK0 |= _BV(OCIE0A);                                // Enable timer 0 Output Compare Match interrupt
#endif

    // Enable global interrupts and sleep mode
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1) {
        // Check if the shutter status has changed, then save it to EEPROM
        if ((shutterStatus != MOVING) && (shutterStatus != settings.shutterStatus)) {
            if (shutterStatus == CLOSED) {
                settings.shutterStatus = shutterStatus;
                saveSettings();
                // If the shutter is closed, the light can be turned on
                if (lightOn) targetBrightness = settings.brightness;
            } else if (shutterStatus == OPEN) {
                settings.shutterStatus = shutterStatus;
                saveSettings();
            }
        }

        // Nothing to do, go to sleep
        sleep_mode();
    }
    return 0;
}

// Timer 0 compare match A interrupt
// Called every ~2ms
// Used for the brightness fade effect
ISR(TIMER0_COMPA_vect){
    if (OCR2B > targetBrightness)
        OCR2B--;
    else if (OCR2B < targetBrightness)
        OCR2B++;
}

#if ENCODER_ENABLE == true
// Timer 0 overflow interrupt
// Called every ~16ms
// Used to read the encoder button
ISR(TIMER0_OVF_vect){
    bool btn = PINB & _BV(PB0);
    if ((!btn) && (lastEncoderBtn)) {
        lightOn = !lightOn;
        setPanelBrigthness(settings.brightness);
    }
    lastEncoderBtn = btn;
}
#endif

// Pin change interrupt 0
// Called when the encoder clock changes
ISR(PCINT0_vect) {
    bool currClk = PINB & _BV(PB1);
    if (currClk && (!lastEncoderClk)) {
        if (PINB & _BV(PB2))
            setPanelBrigthness(constrain(settings.brightness + ENCODER_BRIGHTNESS_STEP, 0, 255));
        else
            setPanelBrigthness(constrain(settings.brightness - ENCODER_BRIGHTNESS_STEP, 0, 255));
    }
    lastEncoderClk = currClk;
}

inline void setPanelBrigthness(uint8_t brightness) {
    settings.brightness = brightness;
#if SERVO_ENABLE == true
    if (lightOn && (shutterStatus == CLOSED))
#else
    if (lightOn)
#endif
        targetBrightness = brightness;
    else
        targetBrightness = 0;
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
                OCR2B = targetBrightness = 0;
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
                    targetBrightness = settings.brightness;
#else
                targetBrightness = settings.brightness;
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
                setPanelBrigthness(constrain((int) round(exp(log(256.0) * (((double) atoi(temp)) / 255.0)) - 1.0), 0, 255));
#else
                setPanelBrigthness(constrain(atoi(temp), 0, 255));
#endif    
                sprintf(temp, "*B%d%03d\n", DEVICE_ID, settings.brightness);
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
                if (shutterStatus == OPEN)
                    setServoTarget(settings.openVal);
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
                if (shutterStatus == CLOSED)
                    setServoTarget(settings.closedVal);
                break;
            }

            /*
             *  Set the servo speed (unofficial command)
             *  Request: >Zxxx\r
             *      xxx = servo speed, 0-10
             *  Return : *Qiiyyy\n
             *      ii  = deviceId
             *      yyy = the servo step delay
            */
            case 'Z': {
                if (circBufferPopArray(buffer, (uint8_t*) temp, 2) == BUFFER_EMPTY) break;
                settings.servoStep = linearMap(atoi(temp), 0, 10, SERVO_STEP_MIN, SERVO_STEP_MAX);
                sprintf(temp, "*Z%d%02d\n", DEVICE_ID, settings.servoStep);
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
                    servoSpeed = linearMap(settings.servoStep, SERVO_STEP_MIN, SERVO_STEP_MAX, 0, 10),
                    shutterVal = (uint8_t) shutterStatus;
#else
                uint8_t openVal = 0,
                    closedVal = 0,
                    servoSpeed = 0,
                    shutterStatus = 0;
#endif
                sprintf(temp, "*T%03d%03d%02d%d%d%03d\n", openVal, closedVal, servoSpeed, shutterVal, lightOn, settings.brightness);
                print(temp);
                break;
            }
        }
    }
}
