/*
 * CAN_Mailman.h
 *
 *  Created on: 28 apr. 2017
 *      Author: elias
 */

#ifndef CAN_MAILMAN_H_
#define CAN_MAILMAN_H_
#define CAN_TIMEOUTVALUE_ms 50

typedef struct{
	short ID;
	unsigned char DATA[8];
}SAAB_CAN_FRAME;

typedef enum {
	OK,FAILED,TIMEOUT
}MailmanStatus;

	void CAN_Mailman_Init(int bitratePrescaler);
	MailmanStatus CAN_Mailman_reciveFrame(SAAB_CAN_FRAME * reciveFrame);
	MailmanStatus CAN_Mailman_transmitFrame(SAAB_CAN_FRAME * transmitFrame);
	void CAN_Mailman_addID_ToFilter(unsigned short frameID);


#endif /* CAN_MAILMAN_H_ */
