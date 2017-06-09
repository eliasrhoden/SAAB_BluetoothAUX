/*
 * CDC_Emulator.h
 *
 *  Created on: 28 apr. 2017
 *      Author: elias
 */

#ifndef CDC_EMULATOR_H_
#define CDC_EMULATOR_H_
#include "stm32f3xx.h"

extern TIM_TypeDef * tim7;	//Defined in main

	void CDC_Emulator_Init();
	void CDC_Emulator_sendCDC_Status();
	void CDC_Emulator_handeRecivedFrames();

#endif /* CDC_EMULATOR_H_ */
