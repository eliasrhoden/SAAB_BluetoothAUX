/*
 * CAN_Test.h
 *
 *  Created on: 1 maj 2017
 *      Author: elias
 */

#ifndef CAN_TEST_H_
#define CAN_TEST_H_

#include "Custom_CAN_HAL.h"

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

	CAN_HAL_transmit(&fr);

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

	CAN_HAL_transmit(&fr);

}

#endif /* CAN_TEST_H_ */
