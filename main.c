#include "stm32f4xx.h"

#define FALSE (0)
#define TRUE !(FALSE)

int main(){
	
	uint32_t tmpctr = 1000000;
	RCC_TypeDef *myRCC = RCC;
	GPIO_TypeDef *myPortD = GPIOD;
	
	
	
	myRCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	myPortD->MODER |= GPIO_MODER_MODE12_0;
	myPortD->MODER &= ~(GPIO_MODER_MODE12_1);
	myPortD->OTYPER &= ~(GPIO_OTYPER_OT12);
	myPortD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED12);
	myPortD->PUPDR  &= ~(GPIO_PUPDR_PUPD12);
	
	
	
	while(TRUE){
		
		myPortD->BSRR |= GPIO_BSRR_BS12;
		for( tmpctr = 1000000; tmpctr > 0; --tmpctr){}
		myPortD->BSRR |= GPIO_BSRR_BR12;
		for( tmpctr = 1000000; tmpctr > 0; --tmpctr){}
	}
	
	
	
	return 0;
}
