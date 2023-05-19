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

void circBufferInit(CircBuffer* buffer, uint8_t* data, uint8_t size);
bool isCircBufferEmpty(CircBuffer* buffer);
BufferStatus circBufferPush(CircBuffer* buffer, uint8_t data);
BufferStatus circBufferPop(CircBuffer* buffer, uint8_t* data);
BufferStatus circBufferPopArray(CircBuffer* buffer, uint8_t* data, uint8_t length);
void circBufferClear(CircBuffer* buffer);

#endif
