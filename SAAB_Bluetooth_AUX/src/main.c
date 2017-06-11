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
#include "Debug/DebugSerial.h"
#include "RN52/RN52_Handler.h"

#define tim7 ((TIM_TypeDef *) TIM7_BASE)
#define scb ((SCB_Type *) SCB_BASE)

char stereoHasBeenOff = 0;

//Function prototypes
void initTimerIRQ();
void CAN_RX0_IRQHandler();
void enterSleep();
void wakeUpFromSleep();

void init(){
	HAL_Init();
	DebugSerial_init();
	CDC_Emulator_Init();
	RN52_init();
	initTimerIRQ();
}


void initTimerIRQ(){
	__TIM7_CLK_ENABLE();
	tim7->DIER |= 1;				//Enable TIM7 IRQ
	tim7->PSC = (short) 39999;		//results in 200 Hz clock
	tim7->ARR = (short) 190;		//reload value, to get an interrupt every 950ms

	NVIC_EnableIRQ(TIM7_DAC2_IRQn);
	NVIC_SetPriority(TIM7_DAC2_IRQn,4);
	__asm("CPSIE I \n\t");
	tim7->CNT = (short) 1;
}

void startTimerIRQ(){
	tim7->CR1 |= 1;					//Enable/Start counter
}

void stopTimerIRQ(){
	tim7->CR1 &= ~1;				//Disable/Stop counter
}

//When a CAN-frame is received
void CAN_RX0_IRQHandler(){
	if(stereoHasBeenOff){
		wakeUpFromSleep();
	}
	CDC_Emulator_handeRecivedFrames();
	NVIC_ClearPendingIRQ(CAN_RX0_IRQn);
}

//Called at least every 950ms
void TIM7_DAC2_IRQHandler(){
	CDC_Emulator_sendCDC_Status();
	NVIC_ClearPendingIRQ(TIM7_DAC2_IRQn);
	tim7->CNT = (short) 0;
	tim7->SR &= ~(0x1);
}


void enterSleep(){
	DebugSerial_println("Entering sleep...");
	stereoHasBeenOff = 1;
	//Put RN52 to sleep (if possible)
}

void wakeUpFromSleep(){
	DebugSerial_println("Waking up from sleep");
	stereoHasBeenOff = 0;
	//Wake up RN52
}


int main(void)
{
 	init();
	RN52_start();
	DebugSerial_println("\n---- INIT COMPLETED ---- \n");
	HAL_Delay(1000);
	startTimerIRQ();
	while(1){
		HAL_Delay(3000);
		if(CDC_Emulator_isOK_ToEnterSleep() && !(stereoHasBeenOff)){
			enterSleep();
		}
	}
}


