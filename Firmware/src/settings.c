#include "settings.h"

Settings settings;

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
    if (calcSettingsChecksum() != eeprom_read_byte((uint8_t*) i)) {
        // Checksum matches, but make sure data is valid
#if SERVO_ENABLE == true
        settings.servoDelay = constrain(settings.servoDelay, SERVO_DELAY_MIN, SERVO_DELAY_MAX);
        settings.closedVal = constrain(settings.closedVal, SERVO_CLOSED_MAX, SERVO_CLOSED_MIN);
        settings.openVal = constrain(settings.openVal, SERVO_OPEN_MAX, SERVO_OPEN_MIN);
        if (!((settings.shutterStatus == CLOSED) || (settings.shutterStatus == OPEN)))
            settings.shutterStatus = CLOSED;
#endif
    } else {
        // Checksum doesn't match, load default settings
#if SERVO_ENABLE == true
        settings.servoDelay = SERVO_DELAY_DEFAULT;
        settings.openVal = SERVO_OPEN_DEFAULT;
        settings.closedVal = SERVO_CLOSED_DEFAULT;
        settings.shutterStatus = CLOSED;
#endif
        saveSettings();
    }
}

void saveSettings() {
    settings.servoDelay = constrain(settings.servoDelay, SERVO_DELAY_MIN, SERVO_DELAY_MAX);
    settings.closedVal = constrain(settings.closedVal, SERVO_CLOSED_MAX, SERVO_CLOSED_MIN);
    settings.openVal = constrain(settings.openVal, SERVO_OPEN_MAX, SERVO_OPEN_MIN);
    uint8_t* src = (uint8_t*)&settings;
    for (uint8_t i = 0; i < sizeof(Settings); ++i) {
        eeprom_update_byte((uint8_t*) i, src[i]);
    }
}
