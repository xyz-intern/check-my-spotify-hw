/*
 * serial_communication.h
 *
 *  Created on: Dec 11, 2023
 *      Author: sion
 */

#ifndef INC_SERIAL_COMMUNICATION_H_
#define INC_SERIAL_COMMUNICATION_H_

#include "main.h"
#include "lcd.h"

#define MAX_LENGTH  300
#define LCD_MAX_LENGTH 16

typedef struct MUSIC {
	char ti_tmp[MAX_LENGTH];
	char ar_tmp[MAX_LENGTH];
	char vo_tmp[MAX_LENGTH];
} MUSIC;

//void process_serial_data();
void rcv_song_process(LCDAT *tokens ,MUSIC *musics , LCD_SCROLL *scroll);
void rcv_time_process(LCDAT *tokens ,TIMES *time_tmp,LCD_SCROLL *scroll);
void rcv_stop_process(LCDAT *tokens ,MUSIC *musics);
void volume_screen(LCDAT *tokens,MUSIC *musics,LCD_SCROLL *scroll);

#endif /* INC_SERIAL_COMMUNICATION_H_ */
