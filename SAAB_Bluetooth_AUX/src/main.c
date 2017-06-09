/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f3xx.h"
#include "stm32f303x8.h"
#include "stm32f3xx_nucleo_32.h"

#include "CDC_Emulator.h"
#include "CAN_Test.h"
#include "CAN_Mailman.h"
#include "DebugSerial.h"

#define DBG_MCU_REG ((volatile unsigned int *) (0xE0042008))


GPIO_TypeDef * portA = (GPIO_TypeDef *) GPIOA_BASE;
GPIO_TypeDef * portB = (GPIO_TypeDef *) GPIOB_BASE;
TIM_TypeDef * tim7;

//Function prototypes
void initTimerIRQ();
void CAN_RX0_IRQHandler();

void init(){
	//*DBG_MCU_REG |= (1<<4);	//Freeze TIM6_during debug
	HAL_Init();
	DebugSerial_init();
	CDC_Emulator_Init();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	portA->MODER |= 1;	//Heartbeat LED
	__HAL_RCC_GPIOB_CLK_ENABLE();
	portB->MODER |= 1;	//RN-StartUpPin
	initTimerIRQ();
}

void startRN_Module(){
	portB->ODR |= 1;
	HAL_Delay(2000);
	portB->ODR &= ~1;
}


void initTimerIRQ(){
	tim7 = (TIM_TypeDef *) TIM7_BASE;
	__TIM7_CLK_ENABLE();
	tim7->DIER |= 1;				//Enable TIM7 IRQ
	tim7->PSC = (short) 39999;		//results in 200 Hz clock
	tim7->ARR = (short) 190;		//reload value, to get an interupet every 950ms

	NVIC_EnableIRQ(TIM7_DAC2_IRQn);
	NVIC_SetPriority(TIM7_DAC2_IRQn,4);
	__asm("CPSIE I \n\t");
	tim7->CNT = (short) 1;

}


void heartBeatLED(){
	HAL_GPIO_TogglePin(portA,GPIO_PIN_0);
	HAL_Delay(500);
}

void startTimerIRQ(){
	tim7->CR1 |= 1;					//Enable/Start counter
}

//When a CAN-frame is received
void CAN_RX0_IRQHandler(){
	CDC_Emulator_handeRecivedFrames();

	NVIC_ClearPendingIRQ(CAN_RX0_IRQn);
}

//Called at least every 950ms
void TIM7_DAC2_IRQHandler(){
	CDC_Emulator_sendCDC_Status();
	HAL_GPIO_TogglePin(portA,GPIO_PIN_0);

	NVIC_ClearPendingIRQ(TIM7_DAC2_IRQn);
	tim7->CNT = (short) 0;
	tim7->SR &= ~(0x1);

}



int main(void)
{
	init();
	startRN_Module();
	DebugSerial_println("\n---- INIT COMPLETED ---- \n");
	HAL_Delay(1000);
	startTimerIRQ();

	while(1){
		//Send commandToSend_RN52 to the RN52 module
		//sendTestFrame2();
		HAL_Delay(2000);



	}
}


