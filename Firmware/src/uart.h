#ifndef UART_H
#define UART_H

#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "circular_buffer.h"

#ifdef __AVR_ATmega2560__
#define RX_ISR USART0_RX_vect
#define UDRE_ISR USART0_UDRE_vect
#else
#define RX_ISR USART_RX_vect
#define UDRE_ISR USART_UDRE_vect
#endif

extern CircBuffer txBuff;
extern CircBuffer rxBuff;

typedef void (*CommandHandler)(CircBuffer* buffer);
extern uint8_t commandDelimiter;
extern CommandHandler commandHandler;

/**
 * Initialize the UART module with the given baudrate and buffer size.
*/
void uartBegin(uint16_t baudrate, uint16_t buffersSize);

/**
 * Registers a command handler in the UART module.
 * The command handler is called when the delimiter byte is received.
 * If the delimiter is 0 or the handler is NULL, the command handler is disabled.
*/
void setCommandHandler(CommandHandler handler, uint8_t delimiter);

/**
 * Disable RX and UDR0 interrupts
*/
void disableUARTInterrupts();

/**
 * Disable UDR0 interrupt, enable RX interrupt
*/
void setRxMode();

/**
 * Disable RX interrupt, enable UDR0 interrupt
*/
void setTxMode();

/**
 * Check if the UDR0 (USART Data Register) register is empty
*/
bool isUDREmpty();

/**
 * Add a byte to the transmit buffer.
 * If the buffer is full, it starts sending bytes synchronously to avoid loss of data.
 * It is non-blocking if there is enough space in the buffer.
*/
void addToTxBuffer(uint8_t data);

/**
 * Start the transmission of the data in the transmit buffer.
 * If the transmit buffer is empty, it switches to receive mode.
*/
void startUARTTransmission();

/**
 * Write raw data to the UART.
 * It is non-blocking if there is enough space in the buffer.
*/
void uartWrite(uint8_t* data, uint16_t length);

/**
 * Prints a string to the UART interface.
 * It is non-blocking if there is enough space in the buffer.
*/
void print(const char* data);

/**
 * Prints a string to the UART interface, followed by a newline character.
 * It is non-blocking if there is enough space in the buffer.
*/
void println(const char* data);

/**
 * Prints an integer to the UART interface.
 * It is non-blocking if there is enough space in the buffer.
*/
void printInt(int val);

/**
 * Prints an integer to the UART interface, followed by a newline character.
 * It is non-blocking if there is enough space in the buffer.
*/
void printlnInt(int val);

#endif
