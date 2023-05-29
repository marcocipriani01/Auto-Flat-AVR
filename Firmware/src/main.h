#ifndef MAIN_H
#define MAIN_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <util/delay.h>

#include <avr/sleep.h>

#include "uart.h"
#include "servo.h"
#include "utils.h"
#include "config.h"
#include "alnitak.h"

void timer2init();
void setPanelBrigthness(uint8_t brightness);

void onCommandReceived(CircBuffer* buffer);

#endif
