
// the timers in use:
// timer 1 - PWM for the main motor
// timer 4 - PWM for insert LEDs
// timer 3 - idle scan mode (works when there's no real banknote inside)
// timer 5 - for the MG only
// timer 7 - for the motor speed control PI-regulator
// timer 14 - delay_us, delay_ms
// timer 6
// timer 2 - for checking the time of recording image to SDcard

#include "timer.h"
#include "adc.h"
#include "gpio.h"
#include "pins.h"

#define	F_APB1	84000000


void delay(unsigned int x)
{
	while(x--);
}


//value in us
void EnableTimer5(unsigned int value)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	TIM5->PSC = F_APB1/1000000-1; 
	TIM5->ARR = value;
	TIM5->CR1 |= TIM_CR1_CEN;
	TIM5->DIER |= TIM_DIER_UIE;	
	NVIC_EnableIRQ(TIM5_IRQn); 
}


void DisableTimer5(void)
{
	NVIC_DisableIRQ(TIM5_IRQn); 
	TIM5->CR1&=~TIM_CR1_CEN;
}


void EnablePeriodicalAdc(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->PSC = F_APB1/1000000-1; 
	TIM3->ARR = 1000;
	TIM3->CR1 |= TIM_CR1_CEN;
	TIM3->DIER |= TIM_DIER_UIE;	
	NVIC_EnableIRQ(TIM3_IRQn); 
}


void DisablePeriodicalAdc(void)
{
	NVIC_DisableIRQ(TIM3_IRQn); 
	TIM3->CR1&=~TIM_CR1_CEN;
}


//Based on TIM14
void delay_ms(unsigned int delay)
{
	if(delay==0) { return; }
	
	while((delay--)!=0) {
		delay_us(1000);
	}
}


//Based on TIM14
void delay_us(unsigned int delay)
{
	if(delay==0) return;
	RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
	TIM14->PSC = F_APB1/1000000-1;
	TIM14->ARR = delay;
	TIM14->EGR |= TIM_EGR_UG;
	TIM14->CR1 |= TIM_CR1_OPM;
	TIM14->CR1 |= TIM_CR1_CEN;
	while (TIM14->CR1&TIM_CR1_CEN);
}


// timer 7 is being used for the motor speed measurement
void init_timer7(unsigned int value)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;	// timer 7 enable bit
	TIM7->PSC = F_APB1/1000000-1;  			// for 1us resolution
	TIM7->ARR = value;

	TIM7->EGR |= TIM_EGR_UG;
	TIM7->CR1 |= TIM_CR1_OPM;
	TIM7->CR1 &= ~TIM_CR1_DIR;					// direction = upcounting
	TIM7->CR1 |= TIM_CR1_CEN;
}


void ReLoadTimer7(unsigned int value)
{
	TIM7->ARR = value;
	TIM7->CR1 |= TIM_CR1_CEN;
}


void deInit_timer7(void)
{
	NVIC_DisableIRQ(TIM7_IRQn); 
	TIM7->CR1&=~TIM_CR1_CEN;
}


unsigned int GetTim7Value(void)
{
	return TIM7->CNT;
}


void Timer7_reset(void) 
{
	TIM7->CNT = 0;
}


// timer 6 
void init_timer6(unsigned int value)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;	// timer 6 enable bit
	TIM6->PSC = F_APB1/1000000-1;  			// for 1us resolution
	TIM6->ARR = value;

	TIM6->EGR |= TIM_EGR_UG;
	TIM6->CR1 |= TIM_CR1_OPM;
	TIM6->CR1 &= ~TIM_CR1_DIR;					// direction = upcounting
	TIM6->CR1 |= TIM_CR1_CEN;
	
	//TIM6->DIER |= TIM_DIER_UIE;	
	//NVIC_EnableIRQ(TIM6_DAC_IRQn); 
}


void ReLoadTimer6(unsigned int value)
{
	TIM6->ARR = value;
	TIM6->CR1 |= TIM_CR1_CEN;
}

void deInit_timer6(void)
{
	NVIC_DisableIRQ(TIM6_DAC_IRQn); 
	TIM6->CR1&=~TIM_CR1_CEN;
}

unsigned int GetTim6Value(void)
{
	return TIM6->CNT;
}

void Timer6_reset(void) 
{
	TIM6->CNT = 0;
}

// timer 2 
void init_timer2(unsigned int value)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;	// timer 2 enable bit
	TIM2->PSC = F_APB1/10000-1;  			// for 10ms resolution
	TIM2->ARR = value;

	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_OPM;
	TIM2->CR1 |= TIM_CR1_CEN;
}


unsigned int GetTim2Value(void)
{
	return (TIM2->CNT)/10;
}

void Timer2_reset(void) 
{
	TIM2->CNT = 0;
}





