/*
 * 4chord midi - LCD graphical user interface
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
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
#include <stdint.h>
#include "graphics.h"
#include "lcd3310.h"
#include "menu.h"

/* graphics data array for menus */
static const unsigned char *menus[] = {
    gfx_menu_mode,
    gfx_menu_key,
    gfx_menu_tempo
};

/* graphics data array for tempo digits */
static const unsigned char *tempo_digits[] = {
    gfx_tempo_0,
    gfx_tempo_1,
    gfx_tempo_2,
    gfx_tempo_3,
    gfx_tempo_4,
    gfx_tempo_5,
    gfx_tempo_6,
    gfx_tempo_7,
    gfx_tempo_8,
    gfx_tempo_9,
};

/* graphics data array for modes */
static const unsigned char *modes[] = {
    gfx_mode_chord,
    gfx_mode_chord_arp,
    gfx_mode_chord_arp_oct,
    gfx_mode_arp,
    gfx_mode_arp_oct
};

/* graphics data array for key chords and modifiers */
static const unsigned char *chords[][2] = {
    {gfx_key_c, gfx_key_none},
    {gfx_key_c, gfx_key_sharp},
    {gfx_key_d, gfx_key_none},
    {gfx_key_d, gfx_key_sharp},
    {gfx_key_e, gfx_key_none},
    {gfx_key_f, gfx_key_none},
    {gfx_key_f, gfx_key_sharp},
    {gfx_key_g, gfx_key_none},
    {gfx_key_g, gfx_key_sharp},
    {gfx_key_a, gfx_key_none},
    {gfx_key_b, gfx_key_flat},
    {gfx_key_b, gfx_key_none},
};


/**
 * Print the startup logo on the display
 */
void
gui_printlogo(void)
{
    lcd_fullscreen(gfx_logo);
}

/**
 * Write the given menu item graphic and update the LCD.
 * @param Menu item index in accordance with menu.h values
 */
void
gui_set_menu(menu_item_t item)
{
    lcd_set_menu(menus[item]);
    lcd_update();
}

/**
 * Write the given playback mode item graphic and update the LCD
 * @param Playback mode item index in accordance with menu.h values
 */
void
gui_set_playback_mode(playback_mode_item_t item)
{
    lcd_set_mode(modes[item]);
    lcd_update();
}

/**
 * Write the given playback key item graphic and update the LCD
 * @param Playback key item index in accordance with menu.h values
 */
void
gui_set_playback_key(playback_mode_item_t item)
{
    lcd_set_chord(chords[item]);
    lcd_update();
}

/**
 * Set tempo to given value and update the LCD.
 * Takes tempo as 8 bit integer (>255bpm won't be supported anyway) and
 * splits it in three single digits. The digits' representing graphics
 * are then transferred to the LCD's tempo area.
 *
 * @param tempo New tempo
 */
void
gui_set_playback_tempo(uint8_t tempo)
{
    const unsigned char *digit_graphics[3];
    uint8_t digits[3];
    uint8_t one, ten, hundred;

    /* split integer to three digits */
    one = tempo / 10;
    digits[2] = tempo - (one * 10);
    ten = one / 10;
    digits[1] = one - (ten * 10);
    hundred = ten / 10;
    digits[0] = ten - (hundred * 10);

    digit_graphics[0] = (digits[0] == 0) ? gfx_tempo_none : tempo_digits[digits[0]];
    digit_graphics[1] = tempo_digits[digits[1]];
    digit_graphics[2] = tempo_digits[digits[2]];

    lcd_set_tempo(digit_graphics);
    lcd_update();
}

