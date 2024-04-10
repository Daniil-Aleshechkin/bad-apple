/********************************************************************************
 * Project: Tetris RTOS for STM32
 * 
 * Description: This firmware implements a real-time operating system (RTOS) version of the classic Tetris game, designed to run on an STM32 microcontroller. 
 * It showcases multitasking capabilities, using FreeRTOS for task management, handling user input, updating game state, and refreshing the display in real-time. 
 * The unique challenge of this project was to efficiently manage system resources and ensure smooth gameplay, even as the game's complexity increases. 
 * The implementation required careful attention to task prioritization, synchronization, and real-time input handling, making it a compelling example of an embedded system with RTOS.
 * 
 * Development Environment: The project is developed using the STM32CubeIDE for coding and debugging. 
 * The project structure and build configurations are managed within this IDE, simplifying the development process for STM32-based projects.
 * 
 * Compiler: ARM GCC 13.2.90.20231008-git
 * The project relies on specific features and optimizations available in this version of the ARM GCC compiler, ensuring compatibility and performance on the target hardware.
 * 
 * Libraries: FreeRTOS V202212.01, providing the multitasking kernel. STM32 CMSIS Library for hardware abstraction.
 * 
 * Other Tools: ST-Link for flashing and debugging the microcontroller. usbipd-win for USB over IP, facilitating communication between the WSL environment and the STM32 board.
 * 
 * CPU Configuration: STM32F103xB; The microcontroller is configured for a 72 MHz system clock, with all peripheral clocks appropriately divided. 
 * Specific attention is paid to USART and GPIO configurations to support game input and display output.
 * 
 * Build Instructions: Use the provided Makefile for building the project. Run `make all` to compile the firmware and `make flash` to flash it to the STM32 board. 
 * Ensure `usbipd` is correctly set up if using WSL to connect to the device.
 *********************************************************************************/

#include "usart_STM32.h"
#include "util_STM32.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "cli.h"
#include "semphr.h"
#include "com_14646_matrix.h"
#include <stm32f103xb.h>

# define mainPRINT_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
# define mainINPUT_READ_PRIORITY (tskIDLE_PRIORITY + 1)

static struct pixel red = {false, true, false};
static struct pixel blue = {false, false, true};
static struct pixel green = {true, false, false};
static struct pixel nothing = {false, false, false};

bool CLI_ENABLED = false;

SemaphoreHandle_t xState;

QueueHandle_t xCLIQueue;

int readCharacter;
int adcVal;
int frames;

void handleInput(int input);
void printNum(int num);

void vPrintTask(void* parameters);

void vPrintTask(void* parameters) {
	static int offset = 0;
  
  for(;;){
    if (!CLI_ENABLED) {


      refreshDisplay();

    } else {
       __enable_irq();
    }

    vTaskDelay(3);
	}
}

#define FRAMES 60
#define FRAME_LENGTH 128
#define BYTES FRAMES*FRAME_LENGTH

void vInputTask(void* parameters) {
  int input;
  bool hasTimeout = false;
  uint8_t frame[128];

  for(;;){
    
    __enable_irq();
    hasTimeout = false;
    input = readData(&hasTimeout);

    if (input == 0xe) {
      sendData(0x51);
      __disable_irq();
      for (int i = 0; i < 128; i++) {
        
        input = readData(&hasTimeout);

        if (hasTimeout) {
          break;
        }

        sendData(input);
        frame[i] = input;
      }
      bufferFrame(frame);
      __enable_irq();
    }

    //if (xSemaphoreTake(xState, portMAX_DELAY)) {
    //  xSemaphoreGive(xState);
    //}
	}
}

void USART2_IRQHandler(void) {
  //__disable_irq();
  bool hasTimeout = false;
  int input = readData(&hasTimeout);
  bufferPixel({false, false, false}, 0, 0);
  //__enable_irq(); 
}


uint8_t array[128] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x20, 0x00, 0x00, 0x06, 0xfc, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x03, 0xf0, 0x00, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x03, 0x7f, 0x80, 0x00, 0x01, 0xfe, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0xfb, 0x00, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 0x9f, 0xf0, 0x00, 0x01, 0xbf, 0xf3, 0x7e, 0x01, 0x7f, 0xff, 0x3f, 0xff, 0xff, 0xff, 0x01, 0xe1, 0xff, 0xff, 0x00, 0x00, 0xfe, 0x7f, 0x00, 0x00, 0x07, 0x0f, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


int main() {
  
  //USART2_IRQHandler();
      //   for (int x = 0; x < 32; x++) {
      //   for (int y = 0; y < 32; y++) {
      //     bufferPixel({true, true, true}, x, y);
      //   }
      // }


  //bufferFrame(array);
  usartInit();
  displayInit();
  clockInit();

  xCLIQueue = xQueueCreate(1000, sizeof(char));
  xState = xSemaphoreCreateMutex();

  sendData(0x55);
  
  xTaskCreate(vPrintTask, "Print", configMINIMAL_STACK_SIZE + 1000, NULL, mainPRINT_TASK_PRIORITY, NULL);
  xTaskCreate(vInputTask, "Input", configMINIMAL_STACK_SIZE, NULL, mainINPUT_READ_PRIORITY, NULL);
  //xTaskCreate(vCLITask, "CLI", configMINIMAL_STACK_SIZE + 200, NULL, mainCLI_TASK_PRIORITY, NULL);
  vTaskStartScheduler();

  for (;;) {
    sendData(0x45);
  }
}

char getCharFromNum(int num) {
  if (num < 10) {return (char)(num+48);}
  else {
    return (char)(num-10+65);
  }
}

void printNum(int num ) {
  sendData(getCharFromNum(num / 256));
  sendData(getCharFromNum((num % 256)/ 16));
  sendData(getCharFromNum(num% 16));
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName) {
  /* This function will get called if a task overflows its stack.   If the
  parameters are corrupt then inspect pxCurrentTCB to find which was the
  offending task. */

  (void)pxTask;
  (void)pcTaskName;

  for (;;) {
    sendData(0x55);
  }
}