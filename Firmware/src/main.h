#ifndef MAIN_H
#define MAIN_H

#include <avr/sleep.h>

#include "uart.h"
#include "config.h"

void onCommandReceived(uint8_t* data, uint8_t length);

#endif
