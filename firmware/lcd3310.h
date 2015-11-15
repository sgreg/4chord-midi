/*
 * 4chord midi - Nokia 3310 LCD handling
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#ifndef _LCD3310_H_
#define _LCD3310_H_

/**
 * Initialize the LCD.
 */
void lcd_init(void);

/**
 * Update the LCD content.
 * Transfers the internal memory content via SPI to the LCD
 */
void lcd_update(void);

/**
 * Clear the LCD internal memory.
 * To actually clear the display, call lcd_update() afterwards.
 */
void lcd_clear(void);

/**
 * Write a fullscreen image to the LCD internal memory.
 * Call lcd_update() to display the image on the LCD itself.
 */
void lcd_fullscreen(const unsigned char *data);

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

#endif

