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
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    // The TXCIE0 bit is enabled only when transmitting data
    // This solves the problem of the single data buffer for both TX and RX

    // Set the data frame to 8 bits
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

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
    UCSR0B = (UCSR0B & (~(1 << UDRIE0))) | _BV(RXCIE0);
}

inline void setTxMode() {
    // Disable RX interrupt, enable UDR0 interrupt
    UCSR0B = (UCSR0B & (~(1 << RXCIE0))) | _BV(UDRIE0);
}

void uartWrite(uint8_t* data, uint8_t length) {
    if (length == 0) return;
    // Copy data to buffer
    cli();
    for (uint8_t i = 0; i < length; i++) {
        if (txBuff.size >= MAX_BUFF_SIZE) break;
        txBuff.buffer[txBuff.size++] = data[i];
    }
    sei();

    // Switch to TX mode
    setTxMode();
    // If the data register is empty, transmit the first byte
    if (UCSR0A & (1 << UDRE0))
        UDR0 = txBuff.buffer[txBuff.index++];
}

void uartPrint(const char* data) {
    // Copy data to buffer
    char c;
    cli();
    while (((c = *data++) != '\0') && (txBuff.size < MAX_BUFF_SIZE)) {
        txBuff.buffer[txBuff.size++] = c;
    }
    sei();

    // Switch to TX mode
    setTxMode();
    // If the data register is empty, transmit the first byte
    if (UCSR0A & (1 << UDRE0))
        UDR0 = txBuff.buffer[txBuff.index++];
}

void uartPrintln(const char* data) {
    // Copy data to buffer
    char c;
    cli();
    while (((c = *data++) != '\0') && (txBuff.size < MAX_BUFF_SIZE)) {
        txBuff.buffer[txBuff.size++] = c;
    }
    txBuff.buffer[txBuff.size++] = '\n';
    sei();

    // Switch to TX mode
    setTxMode();
    // If the data register is empty, transmit the first byte
    if (UCSR0A & (1 << UDRE0))
        UDR0 = txBuff.buffer[txBuff.index++];
}

void uartPrintInt(int val) {
    char buffer[10];
    itoa(val, buffer, 10);
    uartPrint(buffer);
}

void uartPrintlnInt(int val) {
    char buffer[10];
    itoa(val, buffer, 10);
    uartPrintln(buffer);
}

// USART RX complete
ISR(RX_ISR) {
    // Receive one byte and put it in the receive buffer
    uint8_t rcv = rxBuff.buffer[rxBuff.index++] = UDR0;
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
