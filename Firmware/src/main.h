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

#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>

#include <avr/sleep.h>

#include "uart.h"
#include "utils.h"
#include "config.h"
#include "alnitak.h"
#include "settings.h"
#if SERVO_ENABLE == true
#include "shutter.h"
#endif

void setPanelBrigthness(uint8_t brightness);

void onCommandReceived(CircBuffer* buffer);

#endif
