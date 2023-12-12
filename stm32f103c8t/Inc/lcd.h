/*
 * lcd.h
 * lcd controll part
 * Created on: Dec 8, 2023
 *      Author: sion
 */
#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"
extern I2C_HandleTypeDef hi2c1;
#define I2C_ADDR 0x27 // I2C address of the PCF8574
#define RS_BIT 0 // Register select bit
#define EN_BIT 2 // Enable bit
#define BL_BIT 3 // Backlight bit
#define D4_BIT 4 // Data 4 bit
#define D5_BIT 5 // Data 5 bit
#define D6_BIT 6 // Data 6 bit
#define D7_BIT 7 // Data 7 bit

extern uint8_t backlight_state;
extern uint32_t du_minutes; // duration
extern uint32_t du_seconds; // duration
extern uint32_t fu_minutes; // full time
extern uint32_t fu_seconds; // full time

extern char min_str[3];
extern char sec_str[3];
extern char time_str[17];

typedef struct LCDAT {
	char** tokens;
	size_t count;
} LCDAT;

typedef struct LCD_SCROLL {
	volatile uint8_t lcd_title_scroll;
	volatile uint8_t lcd_artists_scroll;
	volatile uint8_t scroll_flag;
	volatile uint8_t song_info_print;
	uint32_t display_mode;
} LCD_SCROLL;

typedef struct TIMES {
	uint32_t duration_time_tmp;
	uint32_t full_time_tmp;
} TIMES;

void lcd_write_nibble(uint8_t nibble, uint8_t rs);
void lcd_send_cmd(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_init(void);
void lcd_write_string(char *str);
void lcd_set_cursor(uint8_t row, uint8_t column);
void lcd_clear(void);
void lcd_backlight(uint8_t state);
void lcd_song_info_printing(char* title_tmp, char* artists_tmp);
void lcd_duration_printing(uint32_t duration_tim_tmp,uint32_t full_time_tmp);
void lcd_no_data_printing(void);
void lcd_volume_printing(char* volume_tmp);

#endif /* INC_LCD_H_ */
