/**
 * @file USART_STM32.h
 * @brief USART communication functions for STM32 microcontrollers.
 *
 * Provides functions for initializing and handling USART communication on STM32 microcontrollers.
 * Includes functionalities for sending and receiving data, as well as a specialized function for
 * sending Tetris game board states via USART.
 */

#ifndef USART_STM32
#define USART_STM32

/**
 * @brief Initializes USART communication.
 * 
 * Sets up USART peripheral with predefined configuration parameters for baud rate, parity,
 * word length, and other communication standards. This function must be called before
 * any USART communication can take place.
 */
void usartInit(void);

/**
 * @brief Sends a single data byte over USART.
 * 
 * @param data The data byte to be sent.
 */
void sendData(int data);

/**
 * @brief Reads a single data byte from USART.
 * 
 * Attempts to read a byte of data received over USART. If no data is available,
 * the behavior of this function depends on the USART configuration (it may block
 * until data is received or return immediately).
 * 
 * @return The read data byte, or an error code if no data is available.
 */
int readData(bool* hasCompleted);

/**
 * @brief Sends the characters of a printed Tetris board over USART.
 * 
 * Iterates over a 2D array representing the Tetris game board, sending each character
 * over USART. This function can be used to transmit the current state of the Tetris game
 * to a terminal or other display mechanism capable of receiving text over USART.
 * 
 * @param printedBoard A 2D array of characters representing the Tetris game board.
 */
void sendTetrisChars(char** printedBoard);

#endif /* USART_STM32 */
