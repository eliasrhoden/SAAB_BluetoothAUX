/*
 * RN52_Handler.h
 *
 *  Created on: 9 juni 2017
 *      Author: elias
 */

#ifndef RN52_HANDLER_H_
#define RN52_HANDLER_H_

void RN52_init();

void RN52_start();
void RN52_shutdown();

void RN52_setDiscoverable(char enabled);

void RN52_playPause();
void RN52_nextSong();
void RN52_prevSong();
void RN52_VolumeUp();
void RN52_VolumeDown();
void RN52_MaxVolume();


#endif /* RN52_HANDLER_H_ */
