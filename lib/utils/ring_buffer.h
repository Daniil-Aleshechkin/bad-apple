#include "stdint.h"

#ifndef RING_BUFFER
#define RING_BUFFER

#define BUFFER_SIZE 7
#define FRAME_SIZE 128

typedef struct {
    uint8_t buffer[BUFFER_SIZE][FRAME_SIZE];
    int head;
    int tail;
    int size;
} RingBuffer;

void insert_frame(RingBuffer* buffer, uint8_t frame[FRAME_SIZE]);
uint8_t* pop_frame(RingBuffer* buffer);

#endif