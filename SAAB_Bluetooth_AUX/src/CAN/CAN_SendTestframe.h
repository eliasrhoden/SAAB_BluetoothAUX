/*
 * CAN_SendTestframe.h
 *
 *  Created on: 29 apr. 2017
 *      Author: elias
 */
#ifdef b

#ifndef CAN_SENDTESTFRAME_H_
#define CAN_SENDTESTFRAME_H_
#include "stm32f3xx.h"

#define CAN_ERROR 0
#define CAN_TIMEOUT 1
#define CAN_OK 2

#define TIMEOUT_MS 500
#define HSI_FREQ 8000000
#define CAN_STD_ID_MAX 0x7FF
#define CAN_EXT_ID_MAX 0x1FFFFFFF

CAN_TypeDef * canP = CAN_BASE;

char CAN_TEST_transmit(CanTxMsgTypeDef * frame);

void sendTestFrame1(){

	CanTxMsgTypeDef fr;
	fr.StdId = 0x3c0;
	fr.IDE = CAN_ID_STD;
	fr.DLC = 8;
	fr.RTR = CAN_RTR_DATA;
	fr.Data[0] = 0x80;
	fr.Data[1] = 0x24;
	fr.Data[2] = 0;
	fr.Data[3] = 0;
	fr.Data[4] = 0;
	fr.Data[5] = 0;
	fr.Data[6] = 0;
	fr.Data[7] = 0;

	CAN_TEST_transmit(&fr);

}

void sendTestFrame2(){

	CanTxMsgTypeDef fr;
	fr.StdId = 0x6a1;
	fr.IDE = CAN_ID_STD;
	fr.DLC = 8;
	fr.RTR = CAN_RTR_DATA;
	fr.Data[0] = 0x80;
	fr.Data[1] = 0x24;
	fr.Data[2] = 0;
	fr.Data[3] = 0x03;
	fr.Data[4] = 0;
	fr.Data[5] = 0;
	fr.Data[6] = 0;
	fr.Data[7] = 0;

	CAN_TEST_transmit(&fr);

}

char requestTransmission(int mailbox){
	canP->sTxMailBox[mailbox].TIR |= 0x1; 				//request transmit

	for(int i = 0;!(canP->TSR & CAN_TSR_TME0);i++){
		HAL_Delay(1);
		if(i>TIMEOUT_MS){
			return CAN_TIMEOUT;
		}
	}
	return CAN_OK;
}

//Public function
char CAN_TEST_transmit(CanTxMsgTypeDef * frame){

	int mailbox = findFreeMailBox();
	if(mailbox < 0){
		return 2;
	}
	writeFrameToMailBox(frame,&(canP->sTxMailBox[mailbox]));

	return requestTransmission(mailbox);
}

int findFreeMailBox(){

	if(canP->TSR & CAN_TSR_TME0){
		return 0;
	}

	if(canP->TSR & CAN_TSR_TME1){
		return 1;
	}

	if(canP->TSR & CAN_TSR_TME2){
		return 2;
	}

	return -1;

}

void writeFrameToMailBox(CanTxMsgTypeDef * frame, CAN_TxMailBox_TypeDef * mailbox){

	//Write identifier
	if(frame->IDE == CAN_ID_STD){				//If standard CAN_ID
		mailbox->TIR &= ~(CAN_TI0R_IDE);
		mailbox->TIR &= ~(CAN_STD_ID_MAX << 21);
		mailbox->TIR |= (frame->StdId << 21);
	}else if(frame->IDE == CAN_ID_EXT){			//If extended CAN_ID
		mailbox->TIR |= (CAN_TI0R_IDE);
		mailbox->TIR &= ~(CAN_EXT_ID_MAX << 3);
		mailbox->TIR |= (frame->ExtId << 3);
	}else{
		//#HereWouldIPutMyErrorHandling IF I HAD ANY...
	}

	//Write transmission req, Data or Remote frame
	if(frame->RTR == CAN_RTR_DATA){
		mailbox->TIR &= ~ CAN_TI0R_RTR;
	}else if(frame->RTR == CAN_RTR_DATA){
		mailbox->TIR |=  CAN_TI0R_RTR;
	}else{
		//#HereWouldIPutMyErrorHandling IF I HAD ANY...
	}

	//Write data bytes
	mailbox->TDTR &= ~0xF;
	mailbox->TDTR |= frame->DLC;
	volatile uint32_t * lowestDataByte = &(mailbox->TDLR);

	for(int i = 0;i<frame->DLC;i++){
		int k = i;
		if(i>=4) k -= 4;

		*(lowestDataByte + (i/4)) &= ~(0xFF << (8*k));
		*(lowestDataByte + (i/4)) |= (frame->Data[i] << (8*k));
	}
}



#endif /* CAN_SENDTESTFRAME_H_ */
#endif

