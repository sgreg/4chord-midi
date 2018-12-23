/*
 * 4chord MIDI - Nokia LCD handling
 *
 * Copyright (C) 2018 Sven Gregori <sven@craplab.fi>
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

#include <stdint.h>
#include "config.h"
#include "xbmlib.h"

/* set LCD reset pin high */
#define lcd_rst_high()  do { LCD_RESET_PORT |=  (1 << LCD_RESET_PIN); } while (0)
/* set LCD reset pin low */
#define lcd_rst_low()   do { LCD_RESET_PORT &= ~(1 << LCD_RESET_PIN); } while (0)

/**
 * Initialize the LCD.
 */
void lcd_init(void);

/**
 * Clear the LCD by writing all zeros to it.
 */
void lcd_clear(void);

/**
 * Display the given xbmlib frame on the display.
 * Wrapper function for all nokia_lcd_write_*_frame() functions, taking
 * a xbmlib frame struct and selects the right write function based on
 * the frame's type.
 *
 * @param frame xbmlib frame data referencing the actual data to display
 */
void lcd_write_frame(const struct xbmlib_frame *frame);

/**
 * Write the menu area to the LCD internal memory.
 * @param menu Menu graphics to be written to internal memory
 */
void lcd_set_menu(const unsigned char *menu);

/**
 * Write the full chord area to the LCD internal memory.
 * This writes both, the chord key and the chord modifier.
 *
 * @param chords Array of two graphics for the chord key (index 0) and the
 *               chord modifier (index 1)
 */
void lcd_set_chord(const unsigned char *chord[2]);

/**
 * Write the tempo area to the LCD internal memory.
 * The tempo is displayed as three digits, each digit is written separetely.
 *
 * @param digits Array of three graphics, each containing the digit to display
 *               with index 0 holding the 100's value.
 */
void lcd_set_tempo(const unsigned char *digits[3]);

/**
 * Write the mode area to the LCD internal memory.
 * @param mode Mode graphics to be written to internal memory
 */
void lcd_set_mode(const unsigned char *mode);

/**
 * Display a single chord in the chord list area, highlighted or normal.
 * Chords are based on the currently selected key set through the menu.
 *
 * @param chord_num Chord number index of the currently selected key
 * @param highlighted Set chord display highlighted (1) or normal (0)
 */
void lcd_set_list_chord(uint8_t chord_num, uint8_t highlighted);

/**
 */
void lcd_set_metre(uint8_t top, uint8_t bot);

/**
 * Display the metronome beat.
 *
 * @param beat Beat number based on the playback cycle count
 */
void lcd_set_metronome(uint8_t beat);

#endif

