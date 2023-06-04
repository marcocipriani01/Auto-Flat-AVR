#include "settings.h"

volatile Settings settings;

uint8_t calcSettingsChecksum() {
    uint8_t sum = 0;
    uint8_t* src = (uint8_t*)&settings;
    for (uint8_t i = 0; i < sizeof(Settings); i++) {
        sum += src[i];
    }
    return sum;
}

void loadSettings() {
    uint8_t* dst = (uint8_t*)&settings;
    uint8_t i = 0;
    for (; i < sizeof(Settings); i++) {
        dst[i] = eeprom_read_byte((uint8_t*) i);
    }
    if (calcSettingsChecksum() == eeprom_read_byte((uint8_t*) i)) {
        // Checksum matches, but make sure data is valid
#if SERVO_ENABLE == true
        settings.servoStep = constrain(settings.servoStep, SERVO_STEP_MIN, SERVO_STEP_MAX);
        settings.closedVal = constrain(settings.closedVal, SERVO_CLOSED_MIN, SERVO_CLOSED_MAX);
        settings.openVal = constrain(settings.openVal, SERVO_OPEN_MIN, SERVO_OPEN_MAX);
        if (!((settings.shutterStatus == CLOSED) || (settings.shutterStatus == OPEN)))
            settings.shutterStatus = CLOSED;
#endif
        if (settings.brightness < 10)
            settings.brightness = 255;
    } else {
        // Checksum doesn't match, load default settings
#if SERVO_ENABLE == true
        settings.servoStep = SERVO_STEP_DEFAULT;
        settings.openVal = SERVO_OPEN_DEFAULT;
        settings.closedVal = SERVO_CLOSED_DEFAULT;
        settings.shutterStatus = CLOSED;
#endif
        settings.brightness = 255;
        for (uint8_t i = 0; i < sizeof(Settings); ++i) {
            eeprom_update_byte((uint8_t*) i, dst[i]);
        }
        eeprom_update_byte((uint8_t*) sizeof(Settings), calcSettingsChecksum());
    }
}

void saveSettings() {
#if SERVO_ENABLE == true
    settings.servoStep = constrain(settings.servoStep, SERVO_STEP_MIN, SERVO_STEP_MAX);
    settings.closedVal = constrain(settings.closedVal, SERVO_CLOSED_MIN, SERVO_CLOSED_MAX);
    settings.openVal = constrain(settings.openVal, SERVO_OPEN_MIN, SERVO_OPEN_MAX);
#endif
    uint8_t* src = (uint8_t*)&settings;
    for (uint8_t i = 0; i < sizeof(Settings); ++i) {
        eeprom_update_byte((uint8_t*) i, src[i]);
    }
    eeprom_update_byte((uint8_t*) sizeof(Settings), calcSettingsChecksum());
}
