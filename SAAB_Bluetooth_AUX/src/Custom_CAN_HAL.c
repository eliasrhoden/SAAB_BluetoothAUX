/*
 * Custom_CAN.c
 *
 *  Created on: 20 apr. 2017
 *      Author: elias
 */
#include "Custom_CAN_HAL.h"

#define TIMEOUT_MS 500
#define HSI_FREQ 8000000
#define CAN_STD_ID_MAX 0x7FF
#define CAN_EXT_ID_MAX 0x1FFFFFFF
#define TRUE 1
#define FALSE 0

#define DEBUG 0

CAN_TypeDef * can = (CAN_TypeDef *) CAN_BASE;
CAN_STATE status = CAN_OK;
volatile TIM_TypeDef * tim15 = (volatile TIM_TypeDef *) TIM15_BASE;

//Private function prototypes
int findFreeMailBox();
void startClocks();
void configPins();
int findFreeMailBox();
void exitSleepAndEnterInitMode();
void initalizationModeConfig(short brpValue);
void leaveInitAndEnterNormalMode();
void writeFrameToMailBox(CanTxMsgTypeDef * frame, CAN_TxMailBox_TypeDef * mailbox);
void requestTransmission(int mailbox);
void getFrameInfoFromMailboxToFrame(CAN_FIFOMailBox_TypeDef * mailbox, CanRxMsgTypeDef * frame);
void getDataFromMailboxToFrame(CAN_FIFOMailBox_TypeDef * mailbox, CanRxMsgTypeDef * frame);
void releaseMailBox(int mailboxNR);
void waitUntilMailboxIsPending(int FIFO_nr);
void delay_ms();

void setCAN_Status(CAN_STATE stateToSet){
	//For reporting errors/ statuses in functions
	if(stateToSet == status){
		return;
	}

	if(stateToSet != CAN_OK && status == CAN_OK){
		status = stateToSet;
	}
}


//Public function
CAN_STATE CAN_HAL_init(short brpVal){
	__HAL_RCC_TIM15_CLK_ENABLE();
	startClocks();
	configPins();
	exitSleepAndEnterInitMode();
	initalizationModeConfig(brpVal);
	leaveInitAndEnterNormalMode();

	return status;
}

CAN_STATE CAN_HAL_getStatus(){
	return status;
}

void startClocks(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();
}

void exitSleepAndEnterInitMode(){

	can->MCR |= CAN_MCR_INRQ;
	can->MCR &= ~CAN_MCR_SLEEP;

	for(int i = 0;!(can->MSR & CAN_MSR_INAK) && (can->MSR & CAN_MSR_SLAK);i++){
		delay_ms(1);
		if(i>TIMEOUT_MS){
			setCAN_Status(CAN_TIMEOUT);
			return;
		}
	}
	setCAN_Status(CAN_OK);
}

void initalizationModeConfig(short brpValue){
	//can->MCR &= ~(1<<16);					//Don't freeze during debug
	//can->MCR |= (1<<16);					//Freeze during debug
	can->BTR |= (brpValue & 0x3FF);			//Default : TS1:4, TS2:3 ; For SAAB I_BUS BRP = 20
#if DEBUG == 1
	can->BTR |= (1<<30);			//Silent mode: 31, Loopback: 30 (FOR DEBUGGING!)
#endif

}

void leaveInitAndEnterNormalMode(){
	can->MCR &= ~CAN_MCR_INRQ;
	for(int i = 0;(can->MSR & CAN_MSR_INAK);i++){
		delay_ms(1);
		if(i>TIMEOUT_MS){
			setCAN_Status(CAN_TIMEOUT);
			return;
		}
	}
	setCAN_Status(CAN_OK);
}

void configPins(){
	GPIO_TypeDef * portA = GPIOA;
	portA->MODER |= 0x02800000;		// AF (Alt. Funct.) on PA11 & PA12
	portA->OSPEEDR |= 0x03C00000;	//High speed on PA11 & PA12
	portA->PUPDR |= (0x1<<22);	//Pull-Up on PA11
	portA->AFR[1] |= 0x0099000;		//AF9 on PA11 & PA12
}

//Public function
short CAN_HAL_getBRP_fromBaudRate(int baudRate){
	return (short) (1/baudRate) * 1/8 * HSI_FREQ;
}

//Public function
CAN_STATE CAN_HAL_transmit(CanTxMsgTypeDef * frame){

	int mailbox = findFreeMailBox();
	if(mailbox < 0){
		return CAN_ERROR;
	}
	writeFrameToMailBox(frame,&(can->sTxMailBox[mailbox]));
	requestTransmission(mailbox);

	return status;
}

int findFreeMailBox(){

	if(can->TSR & CAN_TSR_TME0){
		return 0;
	}

	if(can->TSR & CAN_TSR_TME1){
		return 1;
	}

	if(can->TSR & CAN_TSR_TME2){
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

void requestTransmission(int mailbox){
	can->sTxMailBox[mailbox].TIR |= 0x1; 				//request transmit

	for(int i = 0;!(can->TSR & CAN_TSR_TME0);i++){
		delay_ms(1);
		if(i>TIMEOUT_MS){
			setCAN_Status(CAN_TIMEOUT);
			return;
		}
	}
	setCAN_Status(CAN_OK);
}

//Public function
CAN_STATE CAN_HAL_recive(CanRxMsgTypeDef * recive, int FIFO_Mailbox){

	if(FIFO_Mailbox < 0 || FIFO_Mailbox > 1){
		setCAN_Status(CAN_ERROR);
		return status;
	}

	waitUntilMailboxIsPending(FIFO_Mailbox);

	CAN_FIFOMailBox_TypeDef * mailbox = &(can->sFIFOMailBox[FIFO_Mailbox]);

	getFrameInfoFromMailboxToFrame(mailbox,recive);

	if(recive->RTR == CAN_RTR_DATA){
		getDataFromMailboxToFrame(mailbox,recive);
	}

	releaseMailBox(FIFO_Mailbox);
	return status;
}

void waitUntilMailboxIsPending(int FIFO_nr){
	volatile int * mailReg = (volatile int *) &can->RF0R;
	if(FIFO_nr == 1){
		mailReg = (volatile int *) &can->RF1R;
	}

	for(int i = 0; !(*mailReg & 0x3); i++){
		delay_ms(1);
		if(i > TIMEOUT_MS){
			setCAN_Status(CAN_TIMEOUT);
			return;
		}
	}

}

void getFrameInfoFromMailboxToFrame(CAN_FIFOMailBox_TypeDef * mailbox, CanRxMsgTypeDef * frame){

	frame -> IDE = mailbox->RIR & CAN_RI0R_IDE;
	frame -> RTR = mailbox->RIR & CAN_RI0R_RTR;
	frame -> DLC = mailbox->RDTR & CAN_RDT0R_DLC;

	if(frame -> IDE == CAN_ID_STD){
		frame->StdId = (mailbox->RIR & (0x7FF << 21)) >> 21;
	}else{
		frame->ExtId = (mailbox->RIR & (0x1FFFFFFF << 3)) >> 3;
	}
}


void getDataFromMailboxToFrame(CAN_FIFOMailBox_TypeDef * mailbox, CanRxMsgTypeDef * frame){
	for(int i = 0;i< frame->DLC;i++){
		int k = i;
		volatile int * dataReg = (volatile int *) &mailbox->RDLR;	//Bytes [0-3]

		if(i>=4){
			k -= 4;
			dataReg = (volatile int *) &mailbox->RDHR;			//Bytes [4-7]
		}

		frame->Data[i] = (*dataReg & (0xFF << 8 * k)) >> (8 * k);
	}
}



void releaseMailBox(int mailboxNR){

	if(mailboxNR == 0){
		can->RF0R |= CAN_RF0R_RFOM0;
	}else{
		can->RF1R |= CAN_RF0R_RFOM0;
	}
}

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


