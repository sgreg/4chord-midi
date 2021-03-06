/*
 * 4chord MIDI bootloader - Nokia LCD handling
 *
 * Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 * 
 */
#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd.h"
#include "spi.h"

/* LCD reset port name */
#define LCD_RESET_PORT  PORTB
/* LCD reset pin number */
#define LCD_RESET_PIN   PB0

/* set LCD reset pin high */
#define lcd_rst_high()  do { LCD_RESET_PORT |=  (1 << LCD_RESET_PIN); } while (0)
/* set LCD reset pin low */
#define lcd_rst_low()   do { LCD_RESET_PORT &= ~(1 << LCD_RESET_PIN); } while (0)

#define LCD_START_LINE_ADDR (66-2)

/**
 * Hardware reset the display
 */
static void
lcd_reset(void)
{
    lcd_rst_low();
    _delay_ms(50);
    lcd_rst_high();
}

/**
 * Initialize the LCD.
 */
void
lcd_init(void)
{
    spi_init();
    lcd_reset();

    spi_send_command(0x21);
    spi_send_command(0xc8);
    spi_send_command(0x04 | !!(LCD_START_LINE_ADDR & (1u << 6)));
    spi_send_command(0x40 | (LCD_START_LINE_ADDR & ((1u << 6) -1)));
    spi_send_command(0x12);
    spi_send_command(0x20);
    spi_send_command(0x08);
    spi_send_command(0x0c);
}

/**
 * Display fullscreen image data on the LCD.
 * Note, data is expected to be stored in PROGMEM and contain the
 * full display size of data (i.e. LCD_MEMORY_SIZE bytes)
 *
 * @param data full screen PROGMEM data to display
 */
void
lcd_fullscreen(const uint8_t *data)
{
    uint16_t addr;

    spi_send_command(0x80); // set X addr to 0x00
    spi_send_command(0x40); // set Y addr to 0x00

    for (addr = 0; addr < LCD_MEMORY_SIZE; addr++) {
        spi_send_data(pgm_read_byte(&data[addr]));
    }
}
