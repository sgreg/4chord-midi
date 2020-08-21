/*
 * 4chord MIDI - EEPROM data
 *
 * Copyright (C) 2020 Sven Gregori <sven@craplab.fi>
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
#include "lcd.h"

/**
 * EEPROM data structure version.
 * This should be increased any time there's new data defined in the
 * eeprom_data_t struct that either require a defined default value,
 * or the firmware expects to have a specific / initialized value.
 */
static const uint8_t EEPROM_VERSION = 1;

/**
 * Default initialization values for EEPROM.
 *
 * This data is never actually written unless explicitly
 * flashed via the program-eeprom Makefile target.
 */
struct eeprom_data_t eeprom_data EEMEM = {
    /* initial default values */
    .header = {
        .magic = "\xc1\xab\x4c\x0d",
        .eeprom_version = EEPROM_VERSION,
    },
    .board_data = {
        .lcd = {
            .tcoeff = LCD_DEFAULT_TCOEFF,
            .bias   = LCD_DEFAULT_BIAS,
            .vop    = LCD_DEFAULT_VOP,
        },
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
static const char eeprom_update_string[] PROGMEM = "\r\nUpdating EEPROM data";
static const char eeprom_update_done_string[] PROGMEM = " - done\r\n";

static void restore_defaults(void);
static uint8_t check_header_magic(void);
static void check_update(void);

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
        check_update();
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
    eeprom_update_byte(&eeprom_data.header.eeprom_version, EEPROM_VERSION);

    eeprom_update_byte(&eeprom_data.board_data.lcd.tcoeff, LCD_DEFAULT_TCOEFF);
    eeprom_update_byte(&eeprom_data.board_data.lcd.bias, LCD_DEFAULT_BIAS);
    eeprom_update_byte(&eeprom_data.board_data.lcd.vop, LCD_DEFAULT_VOP);

    eeprom_update_byte(&eeprom_data.defaults.menu, MENU_KEY);
    eeprom_update_byte(&eeprom_data.defaults.key, PLAYBACK_KEY_C);
    eeprom_update_byte(&eeprom_data.defaults.mode, PLAYBACK_MODE_CHORD);
    eeprom_update_byte(&eeprom_data.defaults.metre, PLAYBACK_METRE_4_4);
    eeprom_update_byte(&eeprom_data.defaults.tempo, PLAYBACK_TEMPO_DEFAULT);
}

/**
 * Check and perform EEPROM data update.
 *
 * If a new firmware version was installed that has new EEPROM data added to
 * the eeprom_data_t structure, and the firmware expects some specific values
 * at its location (or any value at all that isn't just empty 0xff data), this
 * function here can be used to initialize those values.
 *
 * The EEPROM data version is set in the EEPROM_VERSION constant at the top
 * of this file, and the EEPROM itself stores the last known version it has
 * seen. If the values are the same, there's nothing to do. If the values
 * differ, new data was added that requires update handling, so the update
 * process is executed, and the new EEPROM version value is stored back to
 * the EEPROM itself.
 *
 * Note that running an update process is only necessary if the firmware
 * expects a defined value in the new added EEPROM struct fields.
 */
static void check_update(void)
{
    uint8_t last_version = eeprom_read_byte(&eeprom_data.header.eeprom_version);

    if (last_version == EEPROM_VERSION) {
        /* already latest known EEPROM data version, nothing to do */
        return;
    }

    uart_print_pgm(eeprom_update_string);

    /*
     * Switch through the last booted EEPROM version value and perform update
     * specifically for each version.
     *
     * Note that these cases shouldn't have breaks but rather fall through,
     * which will allow multiple updates at once if necessary.
     *
     * E.g. updating (in some future) from version 2 to version 5 will then
     * go one by one through all the update processes for 2->3, 3->4, and 4->5
     * ..at least in theory, but that's to be seen.
     *
     * Also note that this ignores downgrading to older EEPROM versions.
     */
    switch (last_version) {
        case 0xff:
        case 0x00:
            /*
             * Update to version 1
             *
             * Changes: Added board_data.lcd struct for LCD specific board data
             * Update:  Set default values for LCD tcoeff, bias, and V_op
             */
            eeprom_update_byte(&eeprom_data.board_data.lcd.tcoeff, LCD_DEFAULT_TCOEFF);
            eeprom_update_byte(&eeprom_data.board_data.lcd.bias, LCD_DEFAULT_BIAS);
            eeprom_update_byte(&eeprom_data.board_data.lcd.vop, LCD_DEFAULT_VOP);
    }

    /* Update EEPROM data with latest version number */
    eeprom_update_byte(&eeprom_data.header.eeprom_version, EEPROM_VERSION);

    uart_print_pgm(eeprom_update_done_string);
}

