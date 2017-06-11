/*
 * RN52_Commands.h
 *
 *  Created on: 23 apr. 2017
 *      Author: elias
 */

#ifndef RN52_COMMANDS_H_
#define RN52_COMMANDS_H_

typedef unsigned char RN52_Command;

#define RN52_NO_COMMAND (RN52_Command) 0
#define RN52_PLAY (RN52_Command) 1
#define RN52_PAUSE (RN52_Command) 2
#define RN52_VOL_DOWN (RN52_Command) 3
#define RN52_VOL_UP (RN52_Command) 4
#define RN52_ENTER_DISCOVERABLE (RN52_Command) 5
#define RN52_POWER_ON (RN52_Command) 6
#define RN52_POWER_OFF (RN52_Command) 7

#endif /* RN52_COMMANDS_H_ */
