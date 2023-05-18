#include "uart.h"

void uartBegin(uint16_t baudrate) {
    // Set the baudrate
    uint16_t ubrr = (F_CPU / (baudrate * 16UL)) - 1;
    UBRR0L = (uint8_t) ubrr;
    UBRR0H = (uint8_t) (ubrr >> 8);

    // Enable the TX and RX pins on the ATmega328P, enable the RX interrupt
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
    // The TXCIE0 bit is enabled only when transmitting data
    // This solves the problem of the single data buffer for both TX and RX
    uartMode = Rx;

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
}

void setCommandDelimiter(uint8_t delimiter) {
    commandDelimiter = delimiter;
}

void setCommandHandler(CommandHandler handler) {
    commandHandler = handler;
}

void setRxMode() {
    // Disable TX interrupt, enable RX interrupt
    UCSR0B = (UCSR0B & (~(1 << TXCIE0))) | (1 << RXCIE0);
    uartMode = Rx;
}

void setTxMode() {
    
}

void uartWrite(uint8_t* data, uint8_t length) {
    if ((length == 0) || (length > MAX_BUFF_SIZE)) return;
    // Copy data to buffer
    for (uint8_t i = 0; i < length; i++) {
        txBuff.buffer[i] = data[i];
    }
    txBuff.size = length;
    txBuff.index = 0;
    // Switch to Tx mode
    // Disable RX interrupt, enable TX interrupt
    UCSR0B = (UCSR0B & (~(1 << RXCIE0))) | (1 << TXCIE0);
    if (UCSR0A & (1 << UDRE0)) {
        // If the data register is empty, transmit the first byte
        uartMode = Tx;
        UDR0 = txBuff.buffer[0];
        txBuff.index++;
    } else {
        // If the data register is not empty, wait for USART_RX_vect
        uartMode = SwitchToTx;
    }
}

// USART RX complete
ISR(USART_RX_vect) {
    // Receive one byte and put it in the receive buffer
    uint8_t rcv = rxBuff.buffer[rxBuff.index] = UDR0;
    rxBuff.index++;
    if (rxBuff.index >= MAX_BUFF_SIZE) {
        // Buffer overflow
        rxBuff.index = 0;
    }

    // Call the command handler if necessary
    if ((commandDelimiter != 0xFF) && (rcv == commandDelimiter) && (commandHandler != NULL))
        (*commandHandler)(rxBuff.buffer, rxBuff.index);

    // Switch to TX mode if necessary
    if ((uartMode == SwitchToTx) && (UCSR0A & (1 << UDRE0))) {
        uartMode = Tx;
        UDR0 = txBuff.buffer[0];
        txBuff.index++;
    }
}

// USART Data Register (UDR0) empty
ISR(USART_UDRE_vect) {
    if ((uartMode == Rx) || (uartMode == SwitchToTx)) {
        // Receive one byte and put it in the receive buffer
        uint8_t rcv = rxBuff.buffer[rxBuff.index] = UDR0;
        rxBuff.index++;
        if ((commandDelimiter != 0xFF) && (rcv == commandDelimiter) && (commandHandler != NULL))
            (*commandHandler)(rxBuff.buffer, rxBuff.index);
        if (rxBuff.index >= MAX_BUFF_SIZE) {
            // Buffer overflow
            rxBuff.index = 0;
        }
        if (uartMode == SwitchToTx) {
            // Switch to Tx mode
            setTxMode();
        }
    } else {
        
    }
}
