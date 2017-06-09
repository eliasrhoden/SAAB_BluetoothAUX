/*
 * DebugSerial.h
 *
 *  Created on: 29 apr. 2017
 *      Author: elias
 */

#ifndef DEBUGSERIAL_H_
#define DEBUGSERIAL_H_

#define SERIAL_TIMEOUT_ms 100
//Initates a serial port (USART2) to the ST-link on pins: PA2-TX, PA15-RX

void DebugSerial_init();
void DebugSerial_print(unsigned char * string);
void DebugSerial_println(unsigned char * string);
void DebugSerial_printHex(unsigned int hex);



#endif /* DEBUGSERIAL_H_ */
