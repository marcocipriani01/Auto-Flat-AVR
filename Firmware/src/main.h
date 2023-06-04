#ifndef MAIN_H
#define MAIN_H

#ifndef F_CPU
#warning "F_CPU not defined, using 16MHz by default"
#define F_CPU 16000000UL
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <avr/sleep.h>

#include "uart.h"
#include "utils.h"
#include "config.h"
#include "alnitak.h"
#include "settings.h"
#if SERVO_ENABLE == true
#include "shutter.h"
#endif

/**
 * Turns on the EL panel if the shutter is closed.
 * Also updates the lightOn global variable if updateGlobalVar is true.
*/
void setLightOn(bool val, bool updateGlobalVar);

/**
 * Sets a new target brightness for the EL panel.
*/
void setPanelBrigthness(uint8_t brightness);

/**
 * Called by the USART UDR interrupt when a command is received.
*/
void onCommandReceived(CircBuffer* buffer);

#endif
