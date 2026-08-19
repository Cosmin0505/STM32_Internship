/*
 * lcd.c
 *
 *  Created on: Aug 19, 2026
 *      Author: savav
 */
#include "lcd.h"


void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

void lcd_write_nibble(uint8_t nibble, uint8_t flags)
{
	uint8_t value = ((nibble & 0xF0u) | flags | LCD_BT);
	uint8_t data_high = value | LCD_E;
	i2c1_write(LCD_ADDRESS, &data_high, 1);
	delay_ms(1);
	uint8_t data_low = value & ~LCD_E;
	i2c1_write(LCD_ADDRESS, &data_low, 1);
	delay_ms(1);
}

void lcd_sendcommand(uint8_t cmd)
{
	lcd_write_nibble((cmd & 0xF0u), 0);
	lcd_write_nibble(((cmd << 4) & 0xF0u), 0);
}

void lcd_senddata(uint8_t data)
{
	lcd_write_nibble((data & 0xF0u), LCD_RS);
	lcd_write_nibble(((data << 4) & 0xF0u), LCD_RS);
}

void lcd_init(void)
{
	delay_ms(50);
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	lcd_write_nibble(0x20u, 0);

	lcd_sendcommand(0x28);
	lcd_sendcommand(0x0C);
	lcd_sendcommand(0x06);
	lcd_sendcommand(0x01);
	delay_ms(2);
}

void lcd_write(char *s)
{
	while(*s)
	{
		lcd_senddata(*s++);
	}
}

void lcd_clear_rpm(void)
{
	lcd_sendcommand(0x85);
	lcd_write("        ");
}

void lcd_clear_kph(void)
{
	lcd_sendcommand(0xC7);
	lcd_write("        ");
}

