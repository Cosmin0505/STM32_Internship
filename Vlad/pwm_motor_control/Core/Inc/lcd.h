/*
 * lcd.h
 *
 *  Created on: Aug 19, 2026
 *      Author: savav
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "i2c.h"

#define LCD_ADDRESS 0x3F

#define LCD_RS (1U << 0)
#define LCD_RW (1U << 1)
#define LCD_E (1U << 2)
#define LCD_BT (1U << 3)
#define LCD_DATA (1U << 4)

void delay_ms(uint32_t ms);
void lcd_write_nibble(uint8_t nibble, uint8_t rs);
void lcd_init(void);
void lcd_sendcommand(uint8_t cmd);
void lcd_senddata(uint8_t data);
void lcd_write(char *s);
void lcd_create_mirrored_char(uint8_t slot, uint8_t *pixel_data);


#endif /* INC_LCD_H_ */
