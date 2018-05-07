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
#ifndef _EEPROM_H_
#define _EEPROM_H_
#include <stdint.h>
#include <avr/eeprom.h>
#include "menu.h"

extern struct eeprom_data_t {
    /* reserved for later use */
    uint8_t __header[0x10];             /* 0x00 */
    /* reserved for later use */
    uint8_t __meta[0x10];               /* 0x10 */
    /* reserved for later use */
    uint8_t __board_data[0x10];         /* 0x20 */
    /* default settings */
    struct {
        menu_item_t menu;               /* 0x30 */
        playback_key_item_t key;        /* 0x31 */
        playback_mode_item_t mode;      /* 0x32 */
        playback_metre_item_t metre;    /* 0x33 */
        playback_tempo_item_t tempo;    /* 0x34 */
        uint8_t __reserved[11];         /* 0x35 */
    } defaults;
} eeprom_data EEMEM;

#endif
