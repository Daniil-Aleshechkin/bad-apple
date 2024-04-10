#include <stm32f103xb.h>
#include "tim4.h"

void init_tim4() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
	TIM4->PSC = 20;
	TIM4->ARR = 35200;
	
	/* CR1 CONFIGS */
	TIM4->CR1 &= ~TIM_CR1_CKD; // No clock division
	TIM4->CR1 &= ~TIM_CR1_ARPE; // Auto-reload preload disabled
	TIM4->CR1 &= ~TIM_CR1_CMS; // Center aligned mode
	TIM4->CR1 &= ~TIM_CR1_DIR; // Up counter
	TIM4->CR1 |= TIM_CR1_OPM; // One pulse mode disabled
	
	/* CR2 CONFIGS */
	TIM4->CR2 &= ~TIM_CR2_MMS; // Reset mms mode 
	
	/* SMCR CONFIGS */
	TIM4->SMCR &= ~TIM_SMCR_MSM; // No Master mode
	TIM4->SMCR &= ~TIM_SMCR_ETP; // No external clocks
	TIM4->SMCR &= ~TIM_SMCR_ECE; 
	TIM4->SMCR &= ~TIM_SMCR_ETPS; 
	TIM4->SMCR &= ~TIM_SMCR_ETF; 
	TIM4->SMCR &= ~TIM_SMCR_SMS; // No Slave mode
	
}

void start_tim4(void) {
	TIM4->CR1 |= TIM_CR1_CEN;
	TIM4->SR &= ~TIM_SR_UIF;
	//while (!(TIM2->SR & TIM_SR_UIF));
}

void stop_tim4(void) {
	TIM4->CR1 &= ~TIM_CR1_CEN;
	TIM4->SR &= ~TIM_SR_UIF;
    //TIM4->CNT = TIM4->ARR;
}

bool isTim4Updated() {
	return !((TIM4->SR & TIM_SR_UIF) == 0);
}