#include "circular_buffer.h"

void circBufferInit(CircBuffer* buffer, uint8_t* data, uint16_t size) {
    buffer->data = data;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = size;
}

bool isCircBufferEmpty(CircBuffer* buffer) {
    return (buffer->head == buffer->tail);
}

bool isCircBufferHasData(CircBuffer* buffer) {
    return (buffer->head != buffer->tail);
}

BufferStatus circBufferPush(CircBuffer* buffer, uint8_t data) {
    // `next` is where head will point to after the push
    uint16_t next = buffer->head + 1;
    if (next >= buffer->size)
        next = 0;
    // If head + 1 == tail, the circular buffer is full
    if (next == buffer->tail)
        return BUFFER_FULL;

    buffer->data[buffer->head] = data;
    buffer->head = next;
    return BUFFER_OK;
}

BufferStatus circBufferPop(CircBuffer* buffer, uint8_t* dest) {
    // If head == tail, there's no data
    if (buffer->head == buffer->tail)
        return BUFFER_EMPTY;
    // `next` is where head will point to after the pop
    uint16_t next = buffer->tail + 1;
    if(next >= buffer->size)
        next = 0;

    *dest = buffer->data[buffer->tail];
    buffer->tail = next;
    return BUFFER_OK;
}

BufferStatus circBufferPopArray(CircBuffer* buffer, uint8_t* dest, uint16_t length) {
    uint8_t b;
    while (circBufferPop(buffer, &b) == BUFFER_OK) {
        *(dest++) = b;
        length--;
        if (length == 0)
            return BUFFER_OK;
    }
    return BUFFER_EMPTY;
}

void circBufferClear(CircBuffer* buffer) {
    buffer->head = 0;
    buffer->tail = 0;
}
