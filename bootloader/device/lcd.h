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
#ifndef _LCD_H_
#define _LCD_H_

#define LCD_X_RES   84
#define LCD_Y_RES   48
#define LCD_MEMORY_SIZE     ((LCD_X_RES * LCD_Y_RES) / 8)

void lcd_init(void);
void lcd_fullscreen(const uint8_t *data);

#endif
