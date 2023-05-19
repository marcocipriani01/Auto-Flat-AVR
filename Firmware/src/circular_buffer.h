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
    uint16_t head;
    uint16_t tail;
    uint16_t size;
} CircBuffer;

void circBufferInit(CircBuffer* buffer, uint8_t* data, uint16_t size);
bool isCircBufferEmpty(CircBuffer* buffer);
BufferStatus circBufferPush(CircBuffer* buffer, uint8_t data);
BufferStatus circBufferPop(CircBuffer* buffer, uint8_t* data);
BufferStatus circBufferPopArray(CircBuffer* buffer, uint8_t* data, uint16_t length);
void circBufferClear(CircBuffer* buffer);

#endif
