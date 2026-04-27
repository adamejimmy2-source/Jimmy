/*
 * i2c-lcd.c
 *
 *  Created on: Apr 21, 2026
 *      Author: jha0090
 */


#include "i2c-lcd.h"

extern I2C_HandleTypeDef hi2c1;

#define LCD_ADDR (0x27 << 1)
// If this does not work, try: #define LCD_ADDR (0x3F << 1)

#define LCD_BACKLIGHT 0x08
#define ENABLE        0x04
#define RW            0x00
#define RS_CMD        0x00
#define RS_DATA       0x01

static void lcd_expander_write(uint8_t data)
{
    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, &data, 1, HAL_MAX_DELAY);
}

static void lcd_pulse_enable(uint8_t data)
{
    lcd_expander_write(data | ENABLE);
    HAL_Delay(1);
    lcd_expander_write(data & ~ENABLE);
    HAL_Delay(1);
}

static void lcd_write4bits(uint8_t value)
{
    lcd_expander_write(value);
    lcd_pulse_enable(value);
}

void lcd_send_cmd(char cmd)
{
    uint8_t high = (cmd & 0xF0) | LCD_BACKLIGHT | RS_CMD | RW;
    uint8_t low  = ((cmd << 4) & 0xF0) | LCD_BACKLIGHT | RS_CMD | RW;

    lcd_write4bits(high);
    lcd_write4bits(low);
}

void lcd_send_data(char data)
{
    uint8_t high = (data & 0xF0) | LCD_BACKLIGHT | RS_DATA | RW;
    uint8_t low  = ((data << 4) & 0xF0) | LCD_BACKLIGHT | RS_DATA | RW;

    lcd_write4bits(high);
    lcd_write4bits(low);
}

void lcd_clear(void)
{
    lcd_send_cmd(0x01);
    HAL_Delay(2);
}

void lcd_put_cur(int row, int col)
{
    uint8_t addr;

    if (row == 0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    lcd_send_cmd(addr);
}

void lcd_init(void)
{
    HAL_Delay(50);

    lcd_write4bits(0x30 | LCD_BACKLIGHT);
    HAL_Delay(5);

    lcd_write4bits(0x30 | LCD_BACKLIGHT);
    HAL_Delay(1);

    lcd_write4bits(0x30 | LCD_BACKLIGHT);
    HAL_Delay(10);

    lcd_write4bits(0x20 | LCD_BACKLIGHT);
    HAL_Delay(10);

    lcd_send_cmd(0x28); // 4-bit, 2-line, 5x8 dots
    lcd_send_cmd(0x08); // display off
    lcd_clear();
    lcd_send_cmd(0x06); // entry mode
    lcd_send_cmd(0x0C); // display on, cursor off
}

void lcd_send_string(char *str)
{
    while (*str)
    {
        lcd_send_data(*str++);
    }
}

