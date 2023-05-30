#ifndef UART_H
#define UART_H

#include <stdlib.h>

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#include "circular_buffer.h"

#ifdef __AVR_ATmega2560__
#define RX_ISR USART0_RX_vect
#define UDRE_ISR USART0_UDRE_vect
#else
#define RX_ISR USART_RX_vect
#define UDRE_ISR USART_UDRE_vect
#endif

#define MAX_BUFF_SIZE 64

extern CircBuffer txBuff;
extern CircBuffer rxBuff;

typedef void (*CommandHandler)(CircBuffer* buffer);
extern uint8_t commandDelimiter;
extern CommandHandler commandHandler;

void uartBegin(uint16_t baudrate);

void setCommandDelimiter(uint8_t delimiter);
void setCommandHandler(CommandHandler handler);

void disableUARTInterrupts();
void setRxMode();
void setTxMode();
bool isUDREmpty();
void addToTxBuffer(uint8_t data);
void startUARTTransmission();

void uartWrite(uint8_t* data, uint16_t length);
void print(const char* data);
void println(const char* data);
void printInt(int val);
void printlnInt(int val);

#endif
