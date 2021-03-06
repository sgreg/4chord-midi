/*
 * 4chord MIDI - Fonts
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
#include <stdint.h>
#include <avr/pgmspace.h>
#include "fonts.h"

const uint8_t font_chord_keys[][FONT_CHORD_WIDTH] PROGMEM = {
    { 0x3c, 0x42, 0x42, 0x42, 0x24 }, /* MAJ_C */
    { 0x7e, 0x42, 0x42, 0x24, 0x18 }, /* MAJ_D */
    { 0x7e, 0x4a, 0x4a, 0x4a, 0x42 }, /* MAJ_E */
    { 0x7e, 0x0a, 0x0a, 0x0a, 0x02 }, /* MAJ_F */
    { 0x3c, 0x42, 0x52, 0x52, 0x72 }, /* MAJ_G */
    { 0x7c, 0x12, 0x12, 0x12, 0x7c }, /* MAJ_A */
    { 0x7e, 0x4a, 0x4a, 0x4a, 0x34 }, /* MAJ_B */
    { 0x7e, 0x08, 0x08, 0x08, 0x7e }, /* MAJ_H */

    { 0x38, 0x44, 0x44, 0x44, 0x28 }, /* MIN_C */
    { 0x30, 0x48, 0x48, 0x50, 0x7e }, /* MIN_D */
    { 0x38, 0x54, 0x54, 0x54, 0x58 }, /* MIN_E */
    { 0x10, 0x7c, 0x12, 0x02, 0x04 }, /* MIN_F */
    { 0x08, 0x54, 0x54, 0x54, 0x3c }, /* MIN_G */
    { 0x20, 0x54, 0x54, 0x54, 0x78 }, /* MIN_A */
    { 0x7e, 0x50, 0x48, 0x48, 0x30 }, /* MIN_B */
    { 0x7e, 0x10, 0x08, 0x08, 0x70 }, /* MIN_H */
};

const uint8_t font_chord_mods[][FONT_CHORD_WIDTH] PROGMEM = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, /* NONE  */
    { 0x14, 0x3e, 0x14, 0x3e, 0x14 }, /* SHARP */
    { 0x00, 0x3e, 0x28, 0x28, 0x10 }, /* FLAT  */
};

const uint8_t font_metre[][FONT_METRE_WIDTH] PROGMEM = {
    { 0x3c, 0x7e, 0x42, 0x7e, 0x3c }, /* 0 */
    { 0x04, 0x7e, 0x7e, 0x00, 0x00 }, /* 1 */
    { 0x62, 0x72, 0x7a, 0x5e, 0x4c }, /* 2 */
    { 0x42, 0x4a, 0x4a, 0x7e, 0x34 }, /* 3 */
    { 0x30, 0x2c, 0x7e, 0x7e, 0x20 }, /* 4 */
    { 0x4e, 0x4e, 0x4a, 0x7a, 0x32 }, /* 5 */
    { 0x38, 0x7c, 0x4e, 0x7a, 0x32 }, /* 6 */
    { 0x02, 0x62, 0x7a, 0x1e, 0x06 }, /* 7 */
    { 0x34, 0x7e, 0x4a, 0x7e, 0x36 }, /* 8 */
    { 0x4c, 0x5e, 0x72, 0x3e, 0x1c }, /* 9 */
};

