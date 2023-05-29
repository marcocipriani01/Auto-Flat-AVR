#ifndef SETTINGS_H
#define SETTINGS_H

#include <inttypes.h>
#include <avr/eeprom.h>

#include "utils.h"
#include "config.h"
#include "alnitak.h"

typedef struct {
    uint8_t brightness;
#if SERVO_ENABLE == true
    uint16_t servoDelay;
    uint16_t openVal;
    uint16_t closedVal;
    ShutterStatus shutterStatus;
#endif
} Settings;

extern Settings settings;

uint8_t calcSettingsChecksum();
void loadSettings();
void saveSettings();

#endif
