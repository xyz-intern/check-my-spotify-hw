/*
 * serial_communication.c
 *
 *  Created on: Dec 11, 2023
 *      Author: sion
 */

#include "serial_communication.h"

void rcv_song_process(LCDAT *tokens,MUSIC *musics,LCD_SCROLL *scroll)
{
	  char* title = *(tokens->tokens+1);
	  char* artists = *(tokens->tokens+2);
	  char* volume = *(tokens->tokens+3);

	  size_t title_size = strlen(title);
	  size_t artists_size = strlen(artists);
	  size_t volume_size = strlen(volume);

	  memset(musics->ti_tmp, 0, sizeof(musics->ti_tmp));
	  memset(musics->ar_tmp, 0, sizeof(musics->ar_tmp));
	  memset(musics->vo_tmp, 0, sizeof(musics->vo_tmp));

	  strncpy((char *)musics->ti_tmp,(const char *)title,title_size);
	  strncpy((char *)musics->ar_tmp,(const char *)artists,artists_size);
	  strncpy((char *)musics->vo_tmp,(const char *)volume,volume_size);

	  strcat(musics->ti_tmp,"   ");
	  strcat(musics->ar_tmp,"   ");

	  if ( title_size <= LCD_MAX_LENGTH )
	  {
		  if ( scroll->display_mode == 0 )
		  {
			  lcd_set_cursor(0, 0);
			  lcd_write_string(title);
		  }
		  scroll->lcd_title_scroll = 0;
	  }
	  else
	  {
		  scroll->lcd_title_scroll = 1;
		  scroll->scroll_flag=1;
	  }

	  // check if scrolling is required
	  if ( artists_size <= LCD_MAX_LENGTH )
	  {
		  if ( scroll->display_mode == 1 )
		  {
			  lcd_set_cursor(1, 0);
			  lcd_write_string(artists);
		  }
		  scroll->lcd_artists_scroll = 0;
	  }
	  else
	  {
		  scroll->lcd_artists_scroll = 1;
		  scroll->scroll_flag=1;
	  }
}

void rcv_time_process(LCDAT *tokens,TIMES *time_tmp,LCD_SCROLL *scroll)
{
	  char* duration_time = *(tokens->tokens+1);
	  char* full_time = *(tokens->tokens+2);

	  time_tmp->duration_time_tmp = atoi(duration_time) / 1000;
	  time_tmp->full_time_tmp = atoi(full_time) / 1000;

	  if ( scroll->display_mode == 1 )
	  {
		  lcd_duration_printing(time_tmp->duration_time_tmp,time_tmp->full_time_tmp);
	  }
}

void rcv_stop_process(LCDAT *tokens ,MUSIC *musics)
{
	  char* volume = *(tokens->tokens+2);
	  size_t volume_size = strlen(volume);
	  memset(musics->vo_tmp, 0, sizeof(musics->vo_tmp));
	  strncpy((char *)musics->vo_tmp,(const char *)volume,volume_size);
}

void volume_screen(LCDAT *tokens,MUSIC *musics,LCD_SCROLL *scroll)
{
	  // volume 저장
	  char* volume = *(tokens->tokens+1);
	  size_t volume_size = strlen(volume);
	  memset(musics->vo_tmp, 0, sizeof(musics->vo_tmp));
	  strncpy((char *)musics->vo_tmp,(const char *)volume,volume_size);

	  // if display_mode == 3
	  if ( scroll->display_mode == 2 )
	  {
		  lcd_volume_printing(musics->vo_tmp);
	  }
}

void title_scrolling(MUSIC *mt) {
	int title_tmp_size = strlen(mt->ti_tmp);
	char ch_ti = 0;
	ch_ti = mt->ti_tmp[0];
	strcpy(mt->ti_tmp,mt->ti_tmp+1);
	mt->ti_tmp[title_tmp_size-1] = ch_ti;
}

void artists_scrolling(MUSIC *mt) {
	int artists_tmp_size = strlen(mt->ar_tmp);
	char ch_ar = 0;
	ch_ar = mt->ar_tmp[0];
	strcpy(mt->ar_tmp,mt->ar_tmp+1);
	mt->ar_tmp[artists_tmp_size-1] = ch_ar;
}
