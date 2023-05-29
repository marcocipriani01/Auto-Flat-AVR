#include "uart.h"

CircBuffer txBuff;
CircBuffer rxBuff;

uint8_t commandDelimiter;
CommandHandler commandHandler;

void uartBegin(uint16_t baudrate) {
    // Set the baudrate
    uint16_t ubrr = (F_CPU / (baudrate * 16UL)) - 1;
    UBRR0L = (uint8_t) ubrr;
    UBRR0H = (uint8_t) (ubrr >> 8);

    // Enable the TX and RX pins on the ATmega328P, enable the RX interrupt
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);

    // Set the data frame to 8 bits
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

    // Initialize buffers
    // Warning: malloc() is used here, so the buffers must be freed if the UART is disabled
    circBufferInit(&txBuff, (uint8_t*) malloc(MAX_BUFF_SIZE), MAX_BUFF_SIZE);
    circBufferInit(&rxBuff, (uint8_t*) malloc(MAX_BUFF_SIZE), MAX_BUFF_SIZE);

    // Setup variables
    commandDelimiter = 0;
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

void setTxMode() {
    // Disable RX interrupt, enable UDR0 interrupt
    UCSR0B = (UCSR0B & (~(1 << RXCIE0))) | _BV(UDRIE0);
    // If the data register is empty, transmit the first byte
    if (UCSR0A & (1 << UDRE0)) {
        uint8_t b;
        if (circBufferPop(&txBuff, &b) == BUFFER_OK)
            UDR0 = b;
    }
}

void uartWrite(uint8_t* data, uint16_t length) {
    if (length == 0) return;
    // Copy data to buffer
    for (uint16_t i = 0; i < length; i++) {
        // If the buffer is full, wait until there is space to avoid loss of data
        while (circBufferPush(&txBuff, data[i]) == BUFFER_FULL)
            _NOP();
    }
    setTxMode();
}

void print(const char* data) {
    // Copy data to buffer
    char c;
    while ((c = *(data++)) != 0) {
        // If the buffer is full, wait until there is space to avoid loss of data
        while (circBufferPush(&txBuff, (uint8_t) c) == BUFFER_FULL)
            _NOP();
    }
    setTxMode();
}

void println(const char* data) {
    // Copy data to buffer
    char c;
    while ((c = *(data++)) != 0) {
        while (circBufferPush(&txBuff, (uint8_t) c) == BUFFER_FULL)
            _NOP();
    }
    while (circBufferPush(&txBuff, (uint8_t) '\n') == BUFFER_FULL)
        _NOP();
    setTxMode();
}

void printInt(int val) {
    char buffer[10];
    itoa(val, buffer, 10);
    print(buffer);
}

void printlnInt(int val) {
    char buffer[10];
    itoa(val, buffer, 10);
    println(buffer);
}

// USART RX complete
ISR(RX_ISR) {
    // Receive one byte and put it in the receive buffer
    uint8_t rcv = UDR0;
    circBufferPush(&rxBuff, rcv);

    // Call the command handler if necessary
    if ((commandDelimiter != 0) && (rcv == commandDelimiter) && (commandHandler != NULL)) {
        (*commandHandler)(&rxBuff);
        // Clear the buffer, as we assume that the command handler has processed all the data
        circBufferClear(&rxBuff);
    }
}

// USART Data Register (UDR0) empty
ISR(UDRE_ISR) {
    uint8_t b;
    circBufferPop(&txBuff, &b);
    UDR0 = b;
    // Switch to RX mode if the buffer is empty
    if (isCircBufferEmpty(&txBuff))
        setRxMode();
}
