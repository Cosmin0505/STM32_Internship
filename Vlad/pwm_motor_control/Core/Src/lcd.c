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

/* Send a 4-bit value and pulse the LCD enable line */
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

/* Send two 4-bit values as commands, with RS reset */
void lcd_sendcommand(uint8_t cmd)
{
	lcd_write_nibble((cmd & 0xF0u), 0);
	lcd_write_nibble(((cmd << 4) & 0xF0u), 0);
}

/* Send two 4-bit values as data, with RS set */
void lcd_senddata(uint8_t data)
{
	lcd_write_nibble((data & 0xF0u), LCD_RS);
	lcd_write_nibble(((data << 4) & 0xF0u), LCD_RS);
}

void lcd_init(void)
{
	/* Allow LCD to power on */
	delay_ms(50);

	/* Initialize in 4-bit mode */
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	delay_ms(5);
	lcd_write_nibble(0x30u, 0);
	lcd_write_nibble(0x20u, 0);

	lcd_sendcommand(0x28); // 4-bit mode and 2 lines
	lcd_sendcommand(0x0C); // Display on, cursor off
	lcd_sendcommand(0x06); // Increment cursor after each character
	lcd_sendcommand(0x01); // Clear display
	delay_ms(2);
}

void lcd_write(char *s)
{
	while(*s)
	{
		lcd_senddata(*s++);
	}
}



