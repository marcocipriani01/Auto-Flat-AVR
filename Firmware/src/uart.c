#include "uart.h"

Buffer txBuff;
Buffer rxBuff;

uint8_t commandDelimiter;
CommandHandler commandHandler;

void uartBegin(uint16_t baudrate) {
    // Set the baudrate
    uint16_t ubrr = (F_CPU / (baudrate * 16UL)) - 1;
    UBRR0L = (uint8_t) ubrr;
    UBRR0H = (uint8_t) (ubrr >> 8);

    // Enable the TX and RX pins on the ATmega328P, enable the RX interrupt
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
    // The TXCIE0 bit is enabled only when transmitting data
    // This solves the problem of the single data buffer for both TX and RX

    // Set the data frame to 8 bits
    UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);

    // Clear buffers
    for (uint8_t i = 0; i < MAX_BUFF_SIZE; i++) {
        txBuff.buffer[i] = 0;
        rxBuff.buffer[i] = 0;
    }
    txBuff.size = 0;
    txBuff.index = 0;
    rxBuff.size = 0;
    rxBuff.index = 0;

    // Setup variables
    commandDelimiter = 0xFF;
    commandHandler = NULL;
}

void setCommandDelimiter(uint8_t delimiter) {
    commandDelimiter = delimiter;
}

void setCommandHandler(CommandHandler handler) {
    commandHandler = handler;
}

inline void setRxMode() {
    // Disable UDR0 interrupt, enable RX interrupt
    UCSR0B = (UCSR0B & (~(1 << UDRIE0))) | (1 << RXCIE0);
}

inline void setTxMode() {
    // Disable RX interrupt, enable UDR0 interrupt
    UCSR0B = (UCSR0B & (~(1 << RXCIE0))) | (1 << UDRIE0);
}

void uartWrite(uint8_t* data, uint8_t length) {
    if ((length == 0) || (length > MAX_BUFF_SIZE)) return;
    // Copy data to buffer
    for (uint8_t i = 0; i < length; i++) {
        txBuff.buffer[txBuff.size++] = data[i];
    }

    // Switch to TX mode
    setTxMode();
    if (UCSR0A & (1 << UDRE0)) {
        // If the data register is empty, transmit the first byte
        UDR0 = txBuff.buffer[txBuff.index++];
    }
}

// USART RX complete
ISR(RX_ISR) {
    // Receive one byte and put it in the receive buffer
    uint8_t rcv = rxBuff.buffer[rxBuff.index] = UDR0;
    rxBuff.index++;
    rxBuff.size++;
    if (rxBuff.index >= MAX_BUFF_SIZE) {
        // Buffer overflow
        rxBuff.index = 0;
    }

    // Call the command handler if necessary
    if ((commandDelimiter != 0xFF) && (rcv == commandDelimiter) && (commandHandler != NULL)) {
        (*commandHandler)(rxBuff.buffer, rxBuff.size);
        rxBuff.index = 0;
        rxBuff.size = 0;
    }
}

// USART Data Register (UDR0) empty
ISR(UDRE_ISR) {
    UDR0 = txBuff.buffer[txBuff.index++];
    if (txBuff.index >= txBuff.size) {
        // All data has been transmitted, reset buffer
        txBuff.index = 0;
        txBuff.size = 0;
        // Switch to RX mode
        setRxMode();
    }
}
