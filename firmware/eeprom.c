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
#include <avr/pgmspace.h>
#include "eeprom.h"
#include "menu.h"
#include "uart.h"

struct eeprom_data_t eeprom_data EEMEM = {
    /* initial default values */
    .header = {
        .magic = "\xc1\xab\x4c\x0d",
    },
    .defaults = {
        .menu  = MENU_KEY,
        .key   = PLAYBACK_KEY_C,
        .mode  = PLAYBACK_MODE_CHORD,
        .metre = PLAYBACK_METRE_4_4,
        .tempo = PLAYBACK_TEMPO_DEFAULT,
    },
};

static const char eeprom_string[] PROGMEM = "EEPROM ";
static const char eeprom_ok_string[] PROGMEM = "OK";
static const char eeprom_nok_string[] PROGMEM = "check failed";
static const char eeprom_restored_string[] PROGMEM = " - defaults restored";

static void restore_defaults(void);
static uint8_t check_header_magic(void);


/**
 * Initializes the EEPROM.
 * Performs a basic sanity check on the current EEPROM data by checking
 * that the header section has the correct magic number. If it doesn't,
 * the data is assumed as invalid and overwriten with default values.
 */
void
eeprom_init(void)
{
    uart_print_pgm(eeprom_string);
    if (check_header_magic()) {
        uart_print_pgm(eeprom_ok_string);
    } else {
        uart_print_pgm(eeprom_nok_string);
        restore_defaults();
        uart_print_pgm(eeprom_restored_string);
    }
    uart_newline();
}


/**
 * Checks that the EEPROM data's header section contains the correct
 * magic number 0xc1ab4c0d.
 *
 * @return 1 if magic number matches, 0 if not
 */
static uint8_t
check_header_magic(void)
{
    return (eeprom_read_byte(&eeprom_data.header.magic[0]) == 0xc1 &&
            eeprom_read_byte(&eeprom_data.header.magic[1]) == 0xab &&
            eeprom_read_byte(&eeprom_data.header.magic[2]) == 0x4c &&
            eeprom_read_byte(&eeprom_data.header.magic[3]) == 0x0d);
}

/**
 * Restores the EEPROM data with default values.
 */
static void
restore_defaults(void)
{
    eeprom_update_byte(&eeprom_data.header.magic[0], 0xc1);
    eeprom_update_byte(&eeprom_data.header.magic[1], 0xab);
    eeprom_update_byte(&eeprom_data.header.magic[2], 0x4c);
    eeprom_update_byte(&eeprom_data.header.magic[3], 0x0d);

    eeprom_update_byte(&eeprom_data.defaults.menu, MENU_KEY);
    eeprom_update_byte(&eeprom_data.defaults.key, PLAYBACK_KEY_C);
    eeprom_update_byte(&eeprom_data.defaults.mode, PLAYBACK_MODE_CHORD);
    eeprom_update_byte(&eeprom_data.defaults.metre, PLAYBACK_METRE_4_4);
    eeprom_update_byte(&eeprom_data.defaults.tempo, PLAYBACK_TEMPO_DEFAULT);
}

