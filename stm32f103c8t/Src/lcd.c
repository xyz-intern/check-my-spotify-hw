/*
 * lcd.c
 *
 *  Created on: Dec 8, 2023
 *      Author: sion
 */

#include "lcd.h"

uint8_t backlight_state=1;
uint32_t du_minutes = 0;
uint32_t du_seconds = 0;
uint32_t fu_minutes = 0;
uint32_t fu_seconds = 0;

char min_str[3] = {};
char sec_str[3] = {};
char time_str[17] = {};

void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
	/*
	 * 4 bit - nibble data LCD Display
	 */
	uint8_t data = nibble << D4_BIT;
	data |= rs << RS_BIT;
	data |= backlight_state << BL_BIT; // Include backlight state in data
	data |= 1 << EN_BIT;
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
	HAL_Delay(1);
	data &= ~(1 << EN_BIT);
	HAL_I2C_Master_Transmit(&hi2c1, I2C_ADDR << 1, &data, 1, 100);
}

void lcd_send_cmd(uint8_t cmd) {
	uint8_t upper_nibble = cmd >> 4;
	uint8_t lower_nibble = cmd & 0x0F;
	lcd_write_nibble(upper_nibble, 0);
	lcd_write_nibble(lower_nibble, 0);
	if (cmd == 0x01 || cmd == 0x02) {
		HAL_Delay(2);
	}
}

void lcd_send_data(uint8_t data) {
	uint8_t upper_nibble = data >> 4;
	uint8_t lower_nibble = data & 0x0F;
	lcd_write_nibble(upper_nibble, 1);
	lcd_write_nibble(lower_nibble, 1);
}

void lcd_init(void) {
	HAL_Delay(50);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(5);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(1);
	lcd_write_nibble(0x03, 0);
	HAL_Delay(1);
	lcd_write_nibble(0x02, 0);
	lcd_send_cmd(0x28);
	lcd_send_cmd(0x0C);
	lcd_send_cmd(0x06);
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

void lcd_write_string(char *str) {
	while (*str) {
		lcd_send_data(*str++);
	}
}

void lcd_set_cursor(uint8_t row, uint8_t column) {
	uint8_t address;
	switch (row) {
	case 0:
		address = 0x00;
		break;
	case 1:
		address = 0x40;
		break;
	default:
		address = 0x00;
	}
	address += column;
	lcd_send_cmd(0x80 | address);
}

void lcd_clear(void) {
	lcd_send_cmd(0x01);
	HAL_Delay(2);
}

void lcd_backlight(uint8_t state) {
	if (state) {
		backlight_state = 1;
	} else {
		backlight_state = 0;
	}
}

void lcd_duration_printing(uint32_t duration_time_tmp,uint32_t full_time_tmp) {
	lcd_clear();
	lcd_set_cursor(0, 3);

	// minute
	memset(min_str, 0, strlen(min_str));
	memset(sec_str, 0, strlen(sec_str));
	memset(time_str, 0, strlen(time_str));

	du_minutes = duration_time_tmp / 60;
	du_seconds = duration_time_tmp % 60;

	itoa(du_minutes, min_str, 10);
	itoa(du_seconds, sec_str, 10);

	strcpy(time_str, min_str);
	strcat(time_str, ":");
	if (du_seconds < 10) {
		strcat(time_str, "0");
	}
	strcat(time_str, sec_str);

	memset(min_str, 0, strlen(min_str));
	memset(sec_str, 0, strlen(sec_str));

	fu_minutes = full_time_tmp / 60;
	fu_seconds = full_time_tmp % 60;

	itoa(fu_minutes, min_str, 10);
	itoa(fu_seconds, sec_str, 10);

	strcat(time_str, " ~ ");
	strcat(time_str, min_str);
	strcat(time_str, ":");
	if (fu_seconds < 10) {
		strcat(time_str, "0");
	}
	strcat(time_str, sec_str);

	lcd_write_string(time_str);
}

void lcd_volume_printing(char* volume_tmp) {
	lcd_set_cursor(0, 0);
	lcd_write_string("The current");
	lcd_set_cursor(1, 0);
	lcd_write_string("volume is ");
	lcd_set_cursor(1, 11);
	lcd_write_string(volume_tmp);
}

void lcd_song_info_printing(char* title_tmp, char* artists_tmp) {
	lcd_set_cursor(0, 0);
	lcd_write_string(title_tmp);
	lcd_set_cursor(1, 0);
	lcd_write_string(artists_tmp);
}

void lcd_no_data_printing(void) {
	lcd_set_cursor(0, 0);
	lcd_write_string("Music is not");
	lcd_set_cursor(1, 0);
	lcd_write_string("currently play.");
}

