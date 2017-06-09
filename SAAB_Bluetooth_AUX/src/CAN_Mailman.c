/*
 * CAN_Mailman.cpp
 *
 *  Created on: 28 apr. 2017
 *      Author: elias
 */

#include "CAN_Mailman.h"
#include "stm32f3xx.h"
#include "stm32f3xx_nucleo_32.h"
#include "DebugSerial.h"
#include "Custom_CAN_HAL.h"
//#define DEBUG 0


GPIO_TypeDef * portA;
CAN_HandleTypeDef hcan;
int filterCounter = 0;

MailmanStatus getMailmanStatusFromCAN_HAL(CAN_STATE HAL_Status);


void CAN_Mailman_Init(int bitratePrescaler){

	CAN_HAL_init(20);
	hcan.Instance = (CAN_TypeDef*) CAN_BASE;

	hcan.Instance->IER |= (0x1 << 1);
	NVIC_EnableIRQ(CAN_RX0_IRQn);
	NVIC_SetPriority(CAN_RX0_IRQn,4);


}

MailmanStatus getMailmanStatusFromCAN_HAL(CAN_STATE HAL_Status){
	MailmanStatus toRet;
	switch(HAL_Status){
	case CAN_OK:
		toRet = OK;
		break;
	case CAN_TIMEOUT:
		toRet = TIMEOUT;
		break;
	case CAN_ERROR:
		toRet = FAILED;
		break;
	default:
		toRet = FAILED;
	}
	return toRet;
}

MailmanStatus CAN_Mailman_reciveFrame(SAAB_CAN_FRAME * reciveFrame){
	CanRxMsgTypeDef reciveHAL;
	hcan.pRxMsg = &reciveHAL;

	CAN_HAL_recive(&reciveHAL,FIFO_MAILBOX_0);
	reciveFrame->ID = (reciveHAL.StdId & 0x7FF);

	for(int i = 0;i<8;i++){
		reciveFrame->DATA[i] = reciveHAL.Data[i];
	}

	MailmanStatus st = getMailmanStatusFromCAN_HAL(CAN_HAL_getStatus());

	if(st != OK){
		DebugSerial_println("\n --- Mailman failed to recive a frame (TIMEOUT) --- \n");
	}

	if(st == OK){
		DebugSerial_print("Received ID: ");
		DebugSerial_printHex(reciveFrame->ID);
		DebugSerial_print("\n\r");
		for(int i = 0;i<8;i++){
			DebugSerial_printHex(reciveFrame->DATA[i]);
			DebugSerial_print(" | ");
		}
		DebugSerial_println("\n");
	}

	return st;
}

MailmanStatus CAN_Mailman_transmitFrame(SAAB_CAN_FRAME * transmitFrame){
	CanTxMsgTypeDef frameToSend;
	frameToSend.StdId = (0x7FF & transmitFrame->ID);

	frameToSend.DLC = 8;
	frameToSend.IDE = CAN_ID_STD;
	frameToSend.RTR = CAN_RTR_DATA;

	for(int i = 0;i<8;i++){
		frameToSend.Data[i] = transmitFrame->DATA[i];
	}

	CAN_HAL_transmit(&frameToSend);
	MailmanStatus st = getMailmanStatusFromCAN_HAL(CAN_HAL_getStatus());

	if(st != OK){
		DebugSerial_println("\n --- Mailman failed to transmit a frame (TIMEOUT) --- \n");
	}

	if(st == OK){
		DebugSerial_print("Transmitted ID: ");
		DebugSerial_printHex(transmitFrame->ID);
		DebugSerial_print("\n\r");
		for(int i = 0;i<8;i++){
			DebugSerial_printHex(transmitFrame->DATA[i]);
			DebugSerial_print(" | ");
		}
		DebugSerial_println("\n");
	}

	return st;
}

void CAN_Mailman_addID_ToFilter(unsigned short frameID){
	//Only uses FIFO_0;
	CAN_FilterConfTypeDef filter;

	filter.FilterMode = CAN_FILTERMODE_IDLIST;
	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterActivation = ENABLE;

	filter.FilterScale = CAN_FILTERSCALE_16BIT;
	filter.FilterNumber = filterCounter;
	filter.BankNumber = filterCounter/2;

	frameID &= 0x7FFF;
	filter.FilterIdLow = (frameID << 5);

	HAL_CAN_ConfigFilter(&hcan,&filter);

	filterCounter++;
}


