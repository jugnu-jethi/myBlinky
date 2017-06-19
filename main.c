#include "stm32f4xx.h"



#define FALSE (0)
#define TRUE !(FALSE)
#define DELAY_COUNTER (10000000U)
#define PLLM_DIV_4 ((4U) << RCC_PLLCFGR_PLLM_Pos)
#define PLLM_DIV_8 ((8U) << RCC_PLLCFGR_PLLM_Pos)
#define PLLN_MUL_336 ((336U) << RCC_PLLCFGR_PLLN_Pos)
#define PLLP_DIV_2 ((0U) << RCC_PLLCFGR_PLLP_Pos)
#define PLLP_DIV_4 ((1U) << RCC_PLLCFGR_PLLP_Pos)
#define PLLQ_DIV_7 ((7U) << RCC_PLLCFGR_PLLQ_Pos)
#define PLLQ_DIV_14 ((14U) << RCC_PLLCFGR_PLLQ_Pos)



volatile uint32_t Delay = 0;



void msDelay(uint32_t nTime);



int main(){
	
	uint32_t tmpctr = DELAY_COUNTER;
	RCC_TypeDef *myRCC = RCC;
	GPIO_TypeDef *myPortD = GPIOD;
	FLASH_TypeDef *myFlash = FLASH;
	
	// Set flash wait states to 5
	// Reference Manual: Section 3.5.1
	myFlash->ACR &= ~(FLASH_ACR_LATENCY);
	myFlash->ACR |= FLASH_ACR_LATENCY_5WS;
	while(FLASH_ACR_LATENCY_5WS != (myFlash->ACR & FLASH_ACR_LATENCY));
	// Enable prefetch; Enable Instruction Cache; Enable Data Cache
	myFlash->ACR |= FLASH_ACR_PRFTEN;
	myFlash->ACR |= FLASH_ACR_ICEN;
	myFlash->ACR |= FLASH_ACR_DCEN;
	
	// Enable HSE oscillator source i.e. 8Mhz crystal
	myRCC->CR &= ~(RCC_CR_HSEBYP);
	myRCC->CR |= RCC_CR_HSEON;
	while(FALSE == (myRCC->CR & RCC_CR_HSERDY));
	
	// Set-up PLL configuration
	// PLLM = 8, PLLN = 336, PLLP = 2, PLLQ = 7
	// fPLLIN = 8Mhz/8 = 1Mhz; fVCOOUT = 1Mhz * 336 = 336Mhz; fPLLOUT = 336Mhz/2 = 168Mhz
	// fUSBOTG = 336Mhz/7 = 48Mhz
	myRCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
	myRCC->PLLCFGR |= PLLM_DIV_8;
	myRCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
	myRCC->PLLCFGR |= PLLN_MUL_336;
	myRCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
	myRCC->PLLCFGR |= PLLP_DIV_2;
	myRCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ);
	myRCC->PLLCFGR |= PLLQ_DIV_7;
	myRCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
	
	// Set-up AHB@168Mhz; APB1@42Mhz; APB2@84Mhz
	myRCC->CFGR &= ~(RCC_CFGR_HPRE);
	myRCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	myRCC->CFGR &= ~(RCC_CFGR_PPRE1);
	myRCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
	myRCC->CFGR &= ~(RCC_CFGR_PPRE2);
	myRCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	
	// Enable PLL
	myRCC->CR |= RCC_CR_PLLON;
	while(FALSE == (myRCC->CR & RCC_CR_PLLRDY));
	
	// Switch system clock to PLL source
	myRCC->CR &= ~(RCC_CFGR_SW);
	myRCC->CFGR |= RCC_CFGR_SW_PLL;
	while(RCC_CFGR_SWS_PLL != (myRCC->CFGR & RCC_CFGR_SWS));
	
	// Update SystemCoreClock variable
	SystemCoreClockUpdate();
	
	// Enable clock for GPIOD
	myRCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	// Set-up PD12 as slow speed general purpose push-pull output
	myPortD->MODER |= GPIO_MODER_MODE12_0;
	myPortD->MODER &= ~(GPIO_MODER_MODE12_1);
	myPortD->OTYPER &= ~(GPIO_OTYPER_OT12);
	myPortD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED12);
	myPortD->PUPDR  &= ~(GPIO_PUPDR_PUPD12);
	
	// Set-up PD13 as slow speed general purpose push-pull output
	myPortD->MODER |= GPIO_MODER_MODE13_0;
	myPortD->MODER &= ~(GPIO_MODER_MODE13_1);
	myPortD->OTYPER &= ~(GPIO_OTYPER_OT13);
	myPortD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED13);
	myPortD->PUPDR  &= ~(GPIO_PUPDR_PUPD13);
	
	// Set-up for a systick of every 1ms
	SysTick_Config((168000000U/1000U) - 1);
	
	
	
	while(TRUE){
		
		// Atomically set PD12
		myPortD->BSRR |= GPIO_BSRR_BS12;
		msDelay(1000);
		//for( tmpctr = DELAY_COUNTER; tmpctr > 0; --tmpctr);
		// Atomically reset PD12
		myPortD->BSRR |= GPIO_BSRR_BR12;
		//for( tmpctr = DELAY_COUNTER; tmpctr > 0; --tmpctr);
		msDelay(1000);
	}
	
	
	
	return 0;
}



void SysTick_Handler(void){
	if(Delay > 0) --Delay;
}



void msDelay(uint32_t nTime){
	Delay = nTime;
	while(Delay != 0);
}
