/*
 * Custom_CAN.h
 *
 *	Creates a CAN interface on pins:
 *		PA12 - CAN_TX
 *		PA11 - CAN_RX
 *
 *
 *  Created on: 20 apr. 2017
 *      Author: elias
 *  Last update: 2017-04-21
 */

#ifndef CUSTOM_CAN_HAL_H_
#define CUSTOM_CAN_HAL_H_

#include "stm32f3xx.h"

#define FIFO_MAILBOX_0 0
#define FIFO_MAILBOX_1 1

typedef enum{CAN_OK,CAN_ERROR,CAN_TIMEOUT} CAN_STATE;

//Public function
CAN_STATE CAN_HAL_init(short brpVal);
CAN_STATE CAN_HAL_getStatus();
CAN_STATE CAN_HAL_transmit(CanTxMsgTypeDef * frame);
CAN_STATE CAN_HAL_recive(CanRxMsgTypeDef * recive, int FIFO_Mailbox);
short CAN_HAL_getBRP_fromBaudRate(int baudRate);

#endif /* CUSTOM_CAN_HAL_H_ */
