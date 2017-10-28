/*
 * 4chord MIDI - LCD graphical user interface
 *
 * Copyright (C) 2017 Sven Gregori <sven@craplab.fi>
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

#include <stdint.h>
#include "menu.h"

/**
 * Display the given menu item graphic on the LCD.
 * @param Menu item index in accordance with menu.h values
 */
void gui_set_menu(menu_item_t item);

/**
 * Display the given playback mode item graphic on the LCD.
 * @param Playback mode item index in accordance with menu.h values
 */
void gui_set_playback_mode(playback_mode_item_t item);

/**
 * Display the given playback key item graphic on the LCD.
 * @param Playback key item index in accordance with menu.h values
 */
void gui_set_playback_key(playback_mode_item_t item);

/**
 * Display the given tempo value on the LCD.
 * Takes tempo as 8 bit integer (>255bpm won't be supported anyway) and
 * splits it in three single digits. The digits' representing graphics
 * are then transferred to the LCD's tempo area.
 *
 * @param tempo New tempo
 */
void gui_set_playback_tempo(uint8_t);

/**
 * Display the given metre value on the LCD.
 * @param Menu item index in accordance with menu.h values
 */
void gui_set_playback_metre(playback_metre_item_t metre);

#endif
