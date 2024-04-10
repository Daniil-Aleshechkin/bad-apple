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
	USART2->CR1 |= USART_CR1_UE;
}

void sendData(int data) {
		
		while((USART2->SR & USART_SR_TXE) == 0x0) {}
			
		USART2->DR = (data & 0xFF);
}

int readData(bool* hasCompleted) {
		if ((USART2->SR & USART_SR_RXNE) == USART_SR_RXNE) {
			return USART2->DR;
		}
		else  {
			*hasCompleted = false;
			return 0x0;
		}
}