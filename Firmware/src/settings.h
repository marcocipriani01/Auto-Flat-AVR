#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdbool.h>
#include <inttypes.h>
#include <avr/eeprom.h>

#include "utils.h"
#include "config.h"
#include "alnitak.h"

typedef struct {
    volatile uint8_t brightness;
    volatile bool lightOn;
#if SERVO_ENABLE == true
    volatile uint16_t servoStep;
    volatile uint16_t openVal;
    volatile uint16_t closedVal;
    volatile ShutterStatus shutterStatus;
#endif
} Settings;

extern volatile Settings settings;

uint8_t calcSettingsChecksum();
void loadSettings();
void saveSettings();

#endif
