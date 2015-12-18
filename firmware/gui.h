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
#ifndef _GUI_H_
#define _GUI_H_
#include "menu.h"

/**
 * Print the startup logo on the display
 */
void gui_printlogo(void);

/**
 * Write the given menu item graphic and update the LCD.
 * @param Menu item index in accordance with menu.h values
 */
void gui_set_menu(menu_item_t item);

/**
 * Write the given playback mode item graphic and update the LCD
 * @param Playback mode item index in accordance with menu.h values
 */
void gui_set_playback_mode(playback_mode_item_t item);

/**
 * Write the given playback key item graphic and update the LCD
 * @param Playback key item index in accordance with menu.h values
 */
void gui_set_playback_key(playback_mode_item_t item);

/**
 * Set tempo to given value and update the LCD.
 * Takes tempo as 8 bit integer (>255bpm won't be supported anyway) and
 * splits it in three single digits. The digits' representing graphics
 * are then transferred to the LCD's tempo area.
 *
 * @param tempo New tempo
 */
void gui_set_playback_tempo(uint8_t);

#endif
