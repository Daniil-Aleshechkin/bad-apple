#include "stdbool.h"
#include "stdint.h"

#ifndef COM_146464_MATRIX 
#define COM_146464_MATRIX
 
#define DISPLAY_WIDTH 32 ///< The width of the LED display in pixels.
#define DISPLAY_HEIGHT 32 ///< The height of the LED display in pixels.
 
/**
 * @struct pixel
 * @brief Represents an RGB color pixel.
 *
 * Defines the structure for an RGB color pixel, with separate byte values for red, green, and blue components.
 */
struct pixel {
    bool r; ///< Red component of the pixel.
    bool g; ///< Green component of the pixel.
    bool b; ///< Blue component of the pixel.
};

/**
 * @brief Buffers a single pixel's color at a specified position.
 * 
 * Buffers the color of a single pixel in the display buffer at the specified (x, y) position.
 * The display buffer holds the pixel data to be displayed upon the next refresh.
 * 
 * @param p The pixel color to buffer.
 * @param x The x position of the pixel in the display.
 * @param y The y position of the pixel in the display.
 */
void bufferPixel(struct pixel p, int x, int y);

/**
 * @brief Refreshes the LED display with buffered pixel data.
 * 
 * Transfers the buffered pixel data to the COM_14646 LED display using GPIO, effectively
 * refreshing the display with new pixel colors. This function handles the timing and
 * data format required by the COM_14646 LED display.
 */
void refreshDisplay(void);

/**
 * @brief Initializes the COM_14646 LED display.
 * 
 * Prepares the GPIO configurations required for controlling the COM_14646 LED display.
 * This function must be called before any operations to buffer or display pixel data.
 */
void displayInit(void);

/**
 * @brief Clears the display buffer.
 * 
 * Resets the display buffer to a default state, effectively clearing any buffered pixel data.
 * This does not immediately affect the display; `refreshDisplay()` must be called to update the display.
 */
void clearBuffer(void);

/**
 * @brief Buffer a frame
 * 
 * Resets the display buffer to a default state, effectively clearing any buffered pixel data.
 * This does not immediately affect the display; `refreshDisplay()` must be called to update the display.
 * @param 
 */
void bufferFrame(uint8_t frame[128]);

#endif