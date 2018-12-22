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
    /* EEPROM header, for identification and sanity check (16 bytes) */
    struct {
        /*
         * header magic number
         * should be 0xc1ab4c0d -> clab4cod -> CrapLab 4chord MIDI
         */
        uint8_t magic[4];                   /* 0x00 */
        uint8_t __header_reserved[12];      /* 0x04 */
    } header;

    /* reserved for later use (16 bytes) */
    uint8_t __meta[16];                     /* 0x10 */

    /* reserved for later use (16 bytes) */
    uint8_t __board_data[16];               /* 0x20 */

    /* default settings (16 bytes) */
    struct {
        menu_item_t menu;                   /* 0x30 */
        playback_key_item_t key;            /* 0x31 */
        playback_mode_item_t mode;          /* 0x32 */
        playback_metre_item_t metre;        /* 0x33 */
        playback_tempo_item_t tempo;        /* 0x34 */
        uint8_t __defaults_reserved[11];    /* 0x35 */
    } defaults;

    /* unused (960 bytes) */                /* 0x40 */
} eeprom_data EEMEM;

/**
 * Initializes the EEPROM.
 * Performs a basic sanity check on the current EEPROM data by checking
 * that the header section has the correct magic number. If it doesn't,
 * the data is assumed as invalid and overwriten with default values.
 */
void eeprom_init(void);

#endif
