#include "uart.h"

CircBuffer txBuff;
CircBuffer rxBuff;

uint8_t commandDelimiter;
CommandHandler commandHandler;

void uartBegin(uint16_t baudrate, uint16_t buffersSize) {
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
    circBufferInit(&txBuff, (uint8_t*) malloc(buffersSize), buffersSize);
    circBufferInit(&rxBuff, (uint8_t*) malloc(buffersSize), buffersSize);

    // Setup variables
    commandDelimiter = 0;
    commandHandler = NULL;
}

void setCommandHandler(CommandHandler handler, uint8_t delimiter) {
    if ((handler == NULL) || (delimiter == 0)) {
        commandHandler = NULL;
        commandDelimiter = 0;
    } else {
        commandHandler = handler;
        commandDelimiter = delimiter;
    }
}

inline void disableUARTInterrupts() {
    UCSR0B &= (~ _BV(RXCIE0)) & (~ _BV(UDRIE0));
}

inline void setRxMode() {
    UCSR0B = (UCSR0B & (~ _BV(UDRIE0))) | _BV(RXCIE0);
}

inline void setTxMode() {
    UCSR0B = (UCSR0B & (~ _BV(RXCIE0))) | _BV(UDRIE0);
}

inline bool isUDREmpty() {
    return UCSR0A & _BV(UDRE0);
}

inline void addToTxBuffer(uint8_t data) {
    while (circBufferPush(&txBuff, data) != BUFFER_OK) {
        if (isUDREmpty())
            circBufferPop(&txBuff, &UDR0);
    }
}

void startUARTTransmission() {
    if (isCircBufferHasData(&txBuff)) {
        setTxMode();
        // If the data register is empty, transmit the first byte
        if (isUDREmpty())
            circBufferPop(&txBuff, &UDR0);
    } else {
        setRxMode();
    }
}

void uartWrite(uint8_t* data, uint16_t length) {
    if (length == 0) return;
    disableUARTInterrupts();
    // Copy data to buffer
    for (uint16_t i = 0; i < length; i++)
        addToTxBuffer(data[i]);
    startUARTTransmission();
}

void print(const char* data) {
    disableUARTInterrupts();
    char c;
    while ((c = *(data++)) != 0)
        addToTxBuffer((uint8_t) c);
    startUARTTransmission();
}

void println(const char* data) {
    disableUARTInterrupts();
    char c;
    while ((c = *(data++)) != 0)
        addToTxBuffer((uint8_t) c);
    addToTxBuffer((uint8_t) '\n');
    startUARTTransmission();
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

// USART RX complete interrupt handler.
ISR(RX_ISR) {
    // Receive one byte and put it in the receive buffer
    // Call the command handler if necessary
    uint8_t rcv = UDR0;
    if ((circBufferPush(&rxBuff, rcv) == BUFFER_OK) && (commandDelimiter != 0) && (rcv == commandDelimiter)) {
        (*commandHandler)(&rxBuff);
        // Clear the buffer, as we assume that the command handler has processed all the data
        circBufferClear(&rxBuff);
    }
}

// USART Data Register (UDR0) empty interrupt handler.
ISR(UDRE_ISR) {
    // Transmit a byte or switch to RX mode if the buffer is empty
    if (circBufferPop(&txBuff, &UDR0) == BUFFER_EMPTY)
        setRxMode();
}
