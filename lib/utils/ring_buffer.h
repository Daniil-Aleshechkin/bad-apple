#include "stdint.h"

#ifndef RING_BUFFER
#define RING_BUFFER

/**
 * @file RingBuffer.h
 * @brief Definition and manipulation of a ring buffer for storing byte frames.
 */

#define BUFFER_SIZE 7 ///< The size of the ring buffer (number of frames it can hold).
#define FRAME_SIZE 128 ///< The size of each frame in the ring buffer.

/**
 * @struct RingBuffer
 * @brief Structure to represent a ring buffer.
 *
 * This structure represents a ring buffer that holds a fixed number of byte frames.
 * The buffer is implemented in a way that it can be used in environments with limited memory,
 * efficiently handling insertion and removal operations.
 */
typedef struct {
    uint8_t buffer[BUFFER_SIZE][FRAME_SIZE]; ///< The buffer storing the frames.
    int head; ///< Index of the head frame in the buffer.
    int tail; ///< Index of the tail frame in the buffer.
    int size; ///< Current number of frames in the buffer.
} RingBuffer;

/**
 * @brief Inserts a frame into the ring buffer.
 *
 * This function inserts a frame into the ring buffer. The caller is responsible for checking the ring buffer size before inserting.
 *
 * @param buffer Pointer to the ring buffer where the frame will be inserted.
 * @param frame The frame to be inserted into the buffer. Must be of size FRAME_SIZE.
 */
void insert_frame(RingBuffer* buffer, uint8_t frame[FRAME_SIZE]);

/**
 * @brief Pops the oldest frame from the ring buffer.
 *
 * This function removes and returns the oldest frame from the ring buffer. Do not call this when the buffer is empty, or memory will be leaked.
 *
 * @param buffer Pointer to the ring buffer from which the frame will be popped.
 * @return A pointer to the frame that was removed from the buffer. The caller must
 * handle the memory management of the returned frame.
 */
uint8_t* pop_frame(RingBuffer* buffer);

#endif // RING_BUFFER
