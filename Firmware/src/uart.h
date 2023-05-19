#ifndef UART_H
#define UART_H

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "config.h"

#ifndef F_CPU
#warning "F_CPU not defined, using 16MHz by default"
#define F_CPU 16000000UL
#endif

#ifdef __AVR_ATmega2560__
#define RX_ISR USART0_RX_vect
#define UDRE_ISR USART0_UDRE_vect
#else
#define RX_ISR USART_RX_vect
#define UDRE_ISR USART_UDRE_vect
#endif

#define MAX_BUFF_SIZE 32

typedef struct {
    uint8_t buffer[MAX_BUFF_SIZE];
    uint8_t size;
    uint8_t index;
} Buffer;

extern Buffer txBuff;
extern Buffer rxBuff;

typedef enum {
    Rx = 0,
    Tx = 1
} UartMode;

extern UartMode uartMode;

extern uint8_t commandDelimiter;
typedef void (*CommandHandler)(uint8_t* data, uint8_t length);
extern CommandHandler commandHandler;

void uartBegin(uint16_t baudrate);

void setCommandDelimiter(uint8_t delimiter);
void setCommandHandler(CommandHandler handler);

void setRxMode();
void setTxMode();

void uartWrite(uint8_t* data, uint8_t length);

#endif
