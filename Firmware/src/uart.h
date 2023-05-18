#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <avr/io.h>

#include "config.h"

#ifndef F_CPU
#warning "F_CPU not defined, using 16MHz by default"
#define F_CPU 16000000UL
#endif

#define MAX_BUFF_SIZE 32

typedef enum UartMode {
    Rx = 0,
    Tx = 1
};

typedef struct{
    uint8_t buffer[MAX_BUFF_SIZE];
    uint8_t size;
    uint8_t index;
} Buffer;

UartMode uartMode;

Buffer txBuff;
Buffer rxBuff;

uint8_t commandDelimiter = 0xFF;
typedef void (*CommandHandler)(uint8_t* data, uint8_t length);
CommandHandler commandHandler = NULL;

void uartBegin(uint16_t baudrate);

void setCommandDelimiter(uint8_t delimiter);
void setCommandHandler(CommandHandler handler);

void setRxMode();
void setTxMode();

void uartWrite(uint8_t* data, uint8_t length);

#endif
