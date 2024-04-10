#include "cli.h"
#include "stdio.h"
#include "stdint.h"
#include "util_STM32.h"
#include "usart_STM32.h"
#include "stdlib.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "version.h"
#include <ctype.h>

#define COMMAND_STRING_BUFFER_SIZE 30
void CLI_Transmit(const uint8_t* pData, uint16_t size );

const char* PROMPT = "tetris-rtos> ";
const char* STARTING_PROMPT = "Tetris CLI: Type \"Help\" for a list of commands";

enum Commands {
	UNKNOWN,
	HELP,
	QUIT,
	VERSION,
};

void CLI_Receive(uint8_t* pData, char nextCharacter, int* bufferIndex);

void moveCursor(unsigned int);

void moveCursor(unsigned int line) {
	char ansiMoveCode[9];
	snprintf(ansiMoveCode, sizeof(ansiMoveCode), "\x1b[%d;0H", line);
	CLI_Transmit((const uint8_t *)ansiMoveCode, sizeof(ansiMoveCode));
}

static const char* SAVE_POS = "\x1b[s"; 
static const char* LOAD_POS = "\x1b[u"; 
static const char* CLEAR_7_LINES = "\x1b[7M";
static const char* INSERT_7_LINES = "\x1b[7L";

void printScreen(const uint8_t* pData) {
	uint16_t size = 0;
	while (pData[size] != 0x00) { size++; }
	
	sendData(0x0a);
	CLI_Transmit(pData, size);
}

void printScreenOnLine(const uint8_t* pData) {
	uint16_t size = 0;
	while (pData[size] != 0x00) { size++; }
	
	CLI_Transmit(pData, size);
}

void CLI_Transmit(const uint8_t* pData, uint16_t size ) {
	int i = 0;
	for(i = 0; i < size; i++) {
		sendData(pData[i]);
	}
}


void CLI_Receive(uint8_t* pData, char nextCharacter, int* bufferIndex) {	
	if (nextCharacter == 0x8) { /* Backspace handling */
		if (*bufferIndex == 0) {
			return;
		}
		
		sendData(nextCharacter);
		sendData(0x1B);
		sendData(0x5B);
		sendData('1');
		sendData('X');

		(*bufferIndex)--;
		return;
	}
	
	/* If the buffer is full. We need one extra char for null byte */
	if (((*bufferIndex) + 1) == (COMMAND_STRING_BUFFER_SIZE - 1))  {
		return;
	}
	
	sendData(nextCharacter);
	
	pData[(*bufferIndex)] = nextCharacter;
	(*bufferIndex)++;
}

const static char* SET_SCROLL_WINDOW = "\x1b[5;r";
const static char* CLEAR = "\x1b[2J";
const static char* CLEAR_LINE = "\x1b[2K";

void clearScrean() {
	/* Clear the terminal */
	CLI_Transmit((const uint8_t*) CLEAR, sizeof(CLEAR));

	sendData(0x1B);
	sendData(0x5B);
	sendData('?');
	sendData('1');
	sendData('2');
	sendData('h');
	
	sendData(0x1B);
	sendData(0x5B);
	sendData('H');
	printScreen((const uint8_t*) STARTING_PROMPT);
	printScreen((const uint8_t*)PROMPT);

}
#define HELP_STRING "help"
#define QUIT_STRING "quit"
#define VERSION_STRING "version"

enum Commands parseReturnCommand(char*);
void toLowerCase(char*);

void toLowerCase(char* str) {
    int i = 0;
    while (str[i] != 0x00) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32;
        }
        i++;
    }
}

int parseCommandValue(char* commandInput) {
    char* token;
    char* endptr;
    long value;

    token = strtok(commandInput, " ");
    if (token == NULL) {
        return -1;
    }

	token = strtok(NULL, " ");
    if (token == NULL) {
        // No argument found after the space
        return -1;
    }

    value = strtol(token, &endptr, 10);

    if (endptr == token) {
        // No digits were found
        return -1;
    }

    return (int)value;
}

enum Commands parseReturnCommand(char* commandInput) {
	size_t length;

	char* space = strchr(commandInput, ' ');
    if (space == NULL) {
        length = strlen(commandInput);
    } else {
        length = space - commandInput;
    }
	
	if (strncmp(QUIT_STRING, commandInput, length) == 0x00 && length == sizeof(QUIT_STRING) - 1) {
		return QUIT;
	} else if (strncmp(VERSION_STRING, commandInput, length) == 0x00 && length == sizeof(VERSION_STRING) - 1) {
		return VERSION;	
	}else {
		return UNKNOWN;
	}
}

const char* ERROR_PARAM_STRING = "The parameter was not a number";
const char* HELP_OUTPUT_STRING = "Commands: \n\
Dasenable: Enable Delayed Auto Repeat \n\
Dasdisable: Disable Delayed Auto Repeat \n\
Help: Prints command list \n\
Quit: Return to the game \n\
DAS N: Set the das delay time to N (default is 40000) \n\
ARR N: Set the arr delay time to N. Zero to disable (default is 0)";
const char* VERSION_OUTPUT_STRING = VERSION_MESSAGE;

const char* UNKNOWN_OUTPUT_STRING = "Unknown command";

void processCommand(enum Commands command, int value) {
	
	char* valueStr = (char*)malloc(sizeof(char) * 20);
	switch (command)
	{
	case HELP:
		printScreen((const uint8_t*)HELP_OUTPUT_STRING);
		break;
	case QUIT:
		sendData(0x1B);
		sendData(0x5B);
		sendData('?');
		sendData('2');
		sendData('5');
		sendData('l');
		
		sendData(0x1B);
		sendData(0x5B);
		sendData('?');
		sendData('1');
		sendData('2');
		sendData('l');

		free(valueStr);
		CLI_ENABLED = false;
		return;
	case VERSION:
		printScreen((const uint8_t*) VERSION_OUTPUT_STRING);
		break;
	default:
		printScreen((const uint8_t*) UNKNOWN_OUTPUT_STRING);
		break;
	}
	free(valueStr);
	sendData(0x0a);
	delay(1000000);
	printScreenOnLine((const uint8_t*)PROMPT);
}


void vCLITask(void* parameters) {
	char recievedChar;
	uint8_t* commandStringBuffer = (uint8_t*)malloc(sizeof(char) * COMMAND_STRING_BUFFER_SIZE);
	int bufferIndex = 0;
	enum Commands command;
	int value;

	for(;;){
		if (xQueueReceive(xCLIQueue, &recievedChar, 0) == pdTRUE) {
			if (recievedChar == 0x0D &&  bufferIndex != 0)  {
				// Send command code here
				
				commandStringBuffer[bufferIndex] = 0x00;
				toLowerCase((char *)commandStringBuffer);
				command = parseReturnCommand((char *)commandStringBuffer);
				value = parseCommandValue((char*)commandStringBuffer);
				
				processCommand(command, value);
				bufferIndex = 0x00;
			} else if (recievedChar != 0x0D && (isalpha(recievedChar) || isdigit(recievedChar) || recievedChar == 0x08 || recievedChar == 0x20)) {
				CLI_Receive(commandStringBuffer, recievedChar, &bufferIndex);
			}
		}
	}
}
