#include <stdlib.h>
#include "ring_buffer.h"
#include "string.h"

void insert_frame(RingBuffer* buffer, uint8_t frame[FRAME_SIZE]) {
    buffer->size++;
    memcpy(buffer->buffer[buffer->head], frame, FRAME_SIZE);
    buffer->head = (buffer->head+1)%BUFFER_SIZE;
}

uint8_t* pop_frame(RingBuffer* buffer) {
    uint8_t* frame = buffer->buffer[buffer->tail];
    buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
    buffer->size--;
    return frame;
}