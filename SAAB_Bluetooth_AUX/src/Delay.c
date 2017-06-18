/*
 * Delay.c
 *
 *  Created on: 18 juni 2017
 *      Author: elias
 */
#include "Delay.h"
#include "stm32f3xx.h"

volatile TIM_TypeDef * tim15 = (volatile TIM_TypeDef *) TIM15_BASE;

void delay_ms(int ms){

	//Leave it like you found it!

	unsigned short cntVal = tim15->CNT;

	tim15->CR1 &= ~1;				//Disable/Reset counter
	tim15->SR &= ~(TIM_SR_UIF);		//Clear status flag
	tim15->PSC = (short) 999;		//results in 8000 Hz clock
	tim15->ARR = (short) 8;			//reload value, to get an UIF-flag after 1 ms
	tim15->CNT = (short) 0;
	tim15->CR1 |= 1;					//Enable/Start counter
	for(int i = 0;i<ms;i++){
		while(!(tim15->SR & TIM_SR_UIF)){
			tim15->CR1 |= 1;					//In case a n IRQ occurred and disabled the counter...
		};
		tim15->SR &= ~(TIM_SR_UIF);	//Clear status flag
		tim15->CNT = (short) 0;
	}

	tim15->CR1 &= ~1;				//Disable/Reset counter
	tim15->CNT = cntVal;

}





