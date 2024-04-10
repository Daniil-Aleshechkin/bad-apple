#include <stm32f103xb.h>
#include "usart_STM32.h"
#include "util_STM32.h"
#include "stdbool.h"

void usartInit(void) 
{
	//Enable Port A and USART clocks
			
    RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;		
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;	
	
		GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= GPIO_CRL_CNF3_0; /* set floating input */
		
		GPIOA->CRL &= ~(GPIO_CRL_CNF2_0);
    GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_MODE2; /* alternate function output push pull 50mhz */


	USART2->CR1 |= USART_CR1_UE;
	USART2->BRR |= 0x10;
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART2->CR1 |= USART_CR1_RXNEIE; // Enable USART interupt
	
	uint32_t prioritygroup = NVIC_GetPriorityGrouping();
	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(prioritygroup, 0, 0));

	USART2->CR1 |= USART_CR1_UE;
}

void sendData(int data) {
		
		while((USART2->SR & USART_SR_TXE) == 0x0) {}
			
		USART2->DR = (data & 0xFF);
}

const int MAX_ATTEMPTS = 1000;

int readData(bool* hasTimeout) {
	int attempts = 0;
	while((USART2->SR & USART_SR_RXNE) != USART_SR_RXNE) {
		if (attempts > MAX_ATTEMPTS) {
			*hasTimeout = true;
			return 0x0;
		}
		attempts++;
	}
	
	return USART2->DR;
}