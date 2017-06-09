/*
 * RN52_Handler.c
 *
 *  Created on: 9 juni 2017
 *      Author: elias
 */
#include "stm32f3xx.h"

#define RN52_PWR_ENABLE_PIN 0 //PB0
GPIO_TypeDef * portB = (GPIO_TypeDef *) GPIOB_BASE;

void RN52_init(){

	__HAL_RCC_GPIOB_CLK_ENABLE();
	portB->MODER |= (1 << RN52_PWR_ENABLE_PIN);

}

void RN52_start(){
	portB->ODR |= 1;
	HAL_Delay(2000);
	portB->ODR &= ~1;
}


