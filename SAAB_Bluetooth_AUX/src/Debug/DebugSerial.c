/*
 * DebugSerial.c
 *
 *  Created on: 29 apr. 2017
 *      Author: elias
 */

#include "stm32f3xx.h"
#include <string.h>
#include "DebugSerial.h"
#include <stdio.h>

GPIO_TypeDef * portWithPins;
UART_HandleTypeDef uart;

void confGPIO_pins();
void confUART();

void DebugSerial_init(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	uart.Instance = (USART_TypeDef*) USART2_BASE;
	portWithPins = (GPIO_TypeDef*) GPIOA_BASE;

	confGPIO_pins();
	confUART();
}

void confGPIO_pins(){
	/*GPIO_InitTypeDef gpioInit;
	gpioInit.Pin = (GPIO_PIN_2 | GPIO_PIN_15);
	gpioInit.Mode = GPIO_MODE_AF_PP;
	gpioInit.Alternate = GPIO_AF7_USART2;
	gpioInit.Speed = GPIO_SPEED_FREQ_MEDIUM;
	gpioInit.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(&portWithPins,&gpioInit);
	*/
	//AF7 = USART2

	portWithPins->MODER |= ((2<<4) | (2<<30));		//AF on PA2 & PA15
	portWithPins->OSPEEDR |= ((1<<4) | (1<<30));	//Medium speed
	portWithPins->PUPDR |= ((1<<4) | (1<<30));		//PULL-UP
	portWithPins->AFR[0] |= (7<<8);					//AF7 on PA2
	portWithPins->AFR[1] |= (7<<28);				//AF7 on PA15

}

void confUART(){
	UART_InitTypeDef uartInit;
	uartInit.BaudRate = 9600;
	uartInit.Mode = UART_MODE_TX;
	uartInit.WordLength = 8;
	uartInit.StopBits = UART_STOPBITS_1;
	uartInit.Parity = UART_PARITY_NONE;
	uartInit.HwFlowCtl = UART_HWCONTROL_NONE;
	uartInit.OverSampling = UART_OVERSAMPLING_8;

	uart.Init = uartInit;
	HAL_UART_Init(&uart);
}

void DebugSerial_print(char * string){
	int stringLenght = strlen(string);
	unsigned char * stringToPrint = (unsigned char *) string;
	HAL_UART_Transmit(&uart,stringToPrint,stringLenght,SERIAL_TIMEOUT_ms);
}

void DebugSerial_println(char * string){
	int stringLenght = strlen(string);
	char toPrint[stringLenght + 3];
	strcpy(toPrint,string);
	strcat(toPrint,"\n\r");
	DebugSerial_print(toPrint);
}

void DebugSerial_printHex(unsigned int hex){
	char string[6];
	sprintf(string,"%X",hex);
	DebugSerial_print(string);
}
