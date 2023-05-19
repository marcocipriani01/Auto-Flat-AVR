#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdbool.h>
#include <inttypes.h>

typedef enum {
    BUFFER_OK,
    BUFFER_FULL,
    BUFFER_EMPTY
} BufferStatus;

typedef struct {
    uint8_t* data;
    uint8_t head;
    uint8_t tail;
    uint8_t size;
} CircBuffer;

void circBufferInit(CircBuffer* buffer, uint8_t* data, uint8_t size) {
    buffer->data = data;
    buffer->head = 0;
    buffer->tail = 0;
    buffer->size = size;
}

bool isCircBufferEmpty(CircBuffer* buffer) {
    return (buffer->head == buffer->tail);
}

BufferStatus circBufferPush(CircBuffer* buffer, uint8_t data) {
    // `next` is where head will point to after the push
    uint8_t next = buffer->head + 1;
    if (next >= buffer->size)
        next = 0;
    // If head + 1 == tail, the circular buffer is full
    if (next == buffer->tail)
        return BUFFER_FULL;

    buffer->data[buffer->head] = data;
    buffer->head = next;
    return BUFFER_OK;
}

BufferStatus circBufferPop(CircBuffer* buffer, uint8_t* data) {
    // If head == tail, there's no data
    if (buffer->head == buffer->tail)
        return BUFFER_EMPTY;
    // `next` is where head will point to after the pop
    uint8_t next = buffer->tail + 1;
    if(next >= buffer->size)
        next = 0;

    *data = buffer->data[buffer->tail];
    buffer->tail = next;
    return BUFFER_OK;
}

#endif
