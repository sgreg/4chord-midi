/*
 * 4chord MIDI - EEPROM data
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
#include <avr/eeprom.h>
#include "eeprom.h"
#include "menu.h"

struct eeprom_data_t eeprom_data EEMEM = {
    /* initial default values */
    .defaults = {
        .menu  = MENU_KEY,
        .key   = PLAYBACK_KEY_C,
        .mode  = PLAYBACK_MODE_CHORD,
        .metre = PLAYBACK_METRE_4_4,
        .tempo = PLAYBACK_TEMPO_DEFAULT,
    },
};
