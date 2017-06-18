/*
 * RN52_Handler.c
 *
 *  Created on: 9 juni 2017
 *      Author: elias
 */
#include "stm32f3xx.h"
#include "RN52_Handler.h"
#include "../Delay.h"

#define RN52_PWR_ENABLE_PIN 0 //PB0
#define RN52_TIMEOUTms 500
GPIO_TypeDef * portB = (GPIO_TypeDef *) GPIOB_BASE;
GPIO_TypeDef * portA = (GPIO_TypeDef *) GPIOA_BASE;
UART_HandleTypeDef uart1;

void confGPIO();
void confUART1();

//UART1 PB9 PB10
//fakkade... behöver RX på PB9 & TX på PB10 DET GÅR #STM 4 life <3

void RN52_init(){

	__HAL_RCC_GPIOB_CLK_ENABLE();
	portB->MODER |= (1 << RN52_PWR_ENABLE_PIN);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	uart1.Instance = (USART_TypeDef*) USART1_BASE;

	confGPIO();
	confUART1();
}

void RN52_start(){
	portB->ODR |= 1;
	delay_ms(1500);
	portB->ODR &= ~1;
	RN52_MaxVolume();
}

void confGPIO(){
	GPIO_InitTypeDef gpioInit;
	gpioInit.Pin = (GPIO_PIN_9 | GPIO_PIN_10);
	gpioInit.Mode = GPIO_MODE_AF_PP;
	gpioInit.Alternate = GPIO_AF7_USART1;
	gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	gpioInit.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(portA,&gpioInit);
}

void confUART1(){
	UART_InitTypeDef uartInit;
	uartInit.BaudRate = 9600;
	uartInit.Mode = UART_MODE_TX;
	uartInit.WordLength = 8;
	uartInit.StopBits = UART_STOPBITS_1;
	uartInit.Parity = UART_PARITY_NONE;
	uartInit.HwFlowCtl = UART_HWCONTROL_NONE;
	uartInit.OverSampling = UART_OVERSAMPLING_8;

	uart1.Init = uartInit;
	uart1.Instance->CR2 |= USART_CR2_SWAP;	//Swap Tx/Rx
	HAL_UART_Init(&uart1);
}


void RN52_shutdown(){
	unsigned char * shutdownString =(unsigned char *) "K,0f\r";
	HAL_UART_Transmit(&uart1,shutdownString,5,RN52_TIMEOUTms);
}

void RN52_setDiscoverable(char enabled){
	unsigned char * string = (unsigned char *) "@,0\r";
	if(enabled){
		string = (unsigned char *) "@,1\r";
	}
	HAL_UART_Transmit(&uart1,string,4,RN52_TIMEOUTms);
}

void RN52_playPause(){
	unsigned char * string = (unsigned char *) "AP\r";
	HAL_UART_Transmit(&uart1,string,3,RN52_TIMEOUTms);
}
void RN52_nextSong(){
	unsigned char * string = (unsigned char *) "AT+\r";
	HAL_UART_Transmit(&uart1,string,4,RN52_TIMEOUTms);
}
void RN52_prevSong(){
	unsigned char * string = (unsigned char *) "AT-\r";
	HAL_UART_Transmit(&uart1,string,4,RN52_TIMEOUTms);
}
void RN52_VolumeUp(){
	unsigned char * string = (unsigned char *) "AV+\r";
	HAL_UART_Transmit(&uart1,string,4,RN52_TIMEOUTms);
}
void RN52_VolumeDown(){
	unsigned char * string = (unsigned char *) "AT-\r";
	HAL_UART_Transmit(&uart1,string,4,RN52_TIMEOUTms);
}

void RN52_MaxVolume(){

	for(int i = 0;i<6;i++){
		RN52_VolumeUp();
		delay_ms(100);
	}



}
