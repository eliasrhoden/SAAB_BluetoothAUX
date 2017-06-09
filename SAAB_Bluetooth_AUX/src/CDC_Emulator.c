/*
 * CDC_Emulator.cpp
 *
 *  Created on: 28 apr. 2017
 *      Author: elias
 */
#include "CDC_Emulator.h"
#include "SAAB_I-BUS_CONSTANTS.h"
#include "CAN_Mailman.h"
#include <string.h>
#include "DebugSerial.h"

typedef enum{ACTIVE, IDLE} CDC_State;
typedef enum{FALSE, TRUE} boolean;

CDC_State stateOfCDC;
boolean needToSendCDC_EventResponse;
boolean resendDueToCDC_cmd;
TIM_TypeDef * tim6 = (TIM_TypeDef *) TIM6_BASE;

void sendStatusOfNodeResponse(SAAB_CAN_FRAME frame);
void sendNodeStatus_sequence(unsigned char response[4][8]);
void handleRecived_CDC_CMD(SAAB_CAN_FRAME frameWithCMD);
void irqProofDelay_ms(unsigned int ms);

void sendStatusOfNodeResponse(SAAB_CAN_FRAME frame){

	unsigned char thirdByte = frame.DATA[3];
	unsigned char response[4][8];

	switch(thirdByte & 0x0F){
	case CDC_NODE_POWER_ON:
		memcpy(response,cdcPoweronCmd,8);
		break;
	case CDC_NODE_ACTIVE:
		memcpy(response,cdcActiveCmd,8);
		break;
	case CDC_NODE_POWER_DOWN:
		memcpy(response,cdcPowerdownCmd,8);
		break;
	}

	sendNodeStatus_sequence(response);
}

void sendNodeStatus_sequence(unsigned char response[4][8]){
	for(int i = 0;i<4;i++){
		SAAB_CAN_FRAME frame;
		frame.ID = STATUS_OF_NODE;
		memcpy(frame.DATA,response[i],8);
		CAN_Mailman_transmitFrame(&frame);
		irqProofDelay_ms(NODE_STATUS_INTERVAL_ms);
	}
}

void handleRecived_CDC_CMD(SAAB_CAN_FRAME frameWithCMD){
	//This code/function is mainly copied from the blueSAAB code.
	boolean event = (frameWithCMD.DATA[0] == 0x80);

	if ((!event) && (stateOfCDC = ACTIVE)) {
		//checkCanEvent(1);	//Used to detect long press on buttons
		return;
	}

	switch (frameWithCMD.DATA[1]) {
	case 0x24: // CDC = ON (CD/RDM button has been pressed twice)
		stateOfCDC = ACTIVE;
		DebugSerial_println("CDC is activated!");
		//BT.bt_reconnect();
		break;
	case 0x14: // CDC = OFF (Back to Radio or Tape mode)
		//BT.bt_disconnect();
		stateOfCDC = IDLE;
		DebugSerial_println("CDC is deactivated!");
		break;
	default:
		break;
	}

	if ((event) && (frameWithCMD.DATA[1] != 0x00)) {
		if (stateOfCDC == ACTIVE) {
			switch (frameWithCMD.DATA[1]) {
			case 0x59: // NXT
				//BT.bt_play();
				break;
			case 0x84: // SEEK button (middle) long press on IHU
				break;
			case 0x88: // > 2 sec long press of SEEK button (middle) on IHU
				break;
			case 0x76: // Random ON/OFF (Long press of CD/RDM button)
				break;
			case 0xB1: // Pause ON
				//BT.bt_play();
				break;
			case 0xB0: // Pause OFF
				//BT.bt_play();
				break;
			case 0x35: // Track +
				//BT.bt_next();
				break;
			case 0x36: // Track -
				//BT.bt_prev();
				break;
			case 0x68: // IHU buttons "1-6"
				switch (frameWithCMD.DATA[2]) {
				case 0x01:
					///BT.bt_volup();
					break;
				case 0x02:
					//BT.bt_set_maxvol();
					break;
				case 0x03:
					//BT.bt_reconnect();
					break;
				case 0x04:
					//BT.bt_voldown();
					break;
				case 0x06:
					//BT.bt_disconnect();
					break;
				default:
					break;
				}
				break;
				default:
					break;
			}
		}
		needToSendCDC_EventResponse = TRUE;
		resendDueToCDC_cmd = TRUE;

		irqProofDelay_ms(50);		//These two lines could be trouble...
		CDC_Emulator_sendCDC_Status();
	}
}

void CDC_Emulator_Init(){
	CAN_Mailman_Init(21);
	__HAL_RCC_TIM6_CLK_ENABLE();

	CAN_Mailman_addID_ToFilter(REQUEST_STATUS_OF_NODE);
	CAN_Mailman_addID_ToFilter(CMD_TO_CDC);

	stateOfCDC = IDLE;
	resendDueToCDC_cmd = FALSE;
	needToSendCDC_EventResponse = FALSE;

}

//Gets called at least every 1000ms
void CDC_Emulator_sendCDC_Status(){
	SAAB_CAN_FRAME frame;
	frame.ID = STATUS_OF_CDC;
	//This code/function is mainly copied from the blueSAAB code.
	frame.DATA[0] = ((needToSendCDC_EventResponse ? 0x07 : 0x00) | (resendDueToCDC_cmd ? 0x00 : 0x01)) << 5;
	frame.DATA[1] = (stateOfCDC == ACTIVE ? 0xFF : 0x00);
	frame.DATA[2] = (stateOfCDC == ACTIVE ? 0x3F : 0x01);
	frame.DATA[3] = (stateOfCDC == ACTIVE ? 0x41 : 0x01);
	frame.DATA[4] = 0xFF;
	frame.DATA[5] = 0xFF;
	frame.DATA[6] = 0xFF;
	frame.DATA[7] = 0xD0;

	CAN_Mailman_transmitFrame(&frame);
	needToSendCDC_EventResponse = FALSE;
	resendDueToCDC_cmd = FALSE;
}

//Called when new frames has arrived
void CDC_Emulator_handeRecivedFrames(){
	SAAB_CAN_FRAME recFrame;
	CAN_Mailman_reciveFrame(&recFrame);

	switch (recFrame.ID){
	case REQUEST_STATUS_OF_NODE:
		irqProofDelay_ms(NODE_STATUS_INTERVAL_ms);
		sendStatusOfNodeResponse(recFrame);
		break;

	case CMD_TO_CDC:
		handleRecived_CDC_CMD(recFrame);
		break;

	default:			//Here should I put some error handling... #PallaDeeeeeet
		break;
	}
}

void irqProofDelay_ms(unsigned int ms){
	tim6->CR1 &= ~1;				//Disable/Reset counter
	tim6->SR &= ~(TIM_SR_UIF);		//Clear status flag
	tim6->PSC = (short) 999;		//results in 8000 Hz clock
	tim6->ARR = (short) 8;			//reload value, to get an UIF-flag after 1 ms
	tim6->CR1 |= 1;					//Enable/Start counter
	for(int i = 0;i<ms;i++){
		while(!(tim6->SR & TIM_SR_UIF));
		tim6->SR &= ~(TIM_SR_UIF);	//Clear status flag
	}
	tim6->CR1 &= ~1;				//Disable/Reset counter

}


char CDC_Emulator_isOK_ToEnterSleep(){
	//Determine if it's ok for the unit to enter sleep
	//Need to check how the CAN-bus behaves during idle (when the stereo isn't in use)
	return 0 ;
}




