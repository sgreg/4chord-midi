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
#ifndef _FONTS_H_
#define _FONTS_H_

#include <stdint.h>

#define FONT_CHORD_WIDTH 5

typedef enum {
    MAJ_C,
    MAJ_D,
    MAJ_E,
    MAJ_F,
    MAJ_G,
    MAJ_A,
    MAJ_B,
    MAJ_H,
    MIN_C,
    MIN_D,
    MIN_E,
    MIN_F,
    MIN_G,
    MIN_A,
    MIN_B,
    MIN_H,
} font_chord_key_t;

typedef enum {
    NONE,
    SHARP,
    FLAT
} font_chord_mod_t;

extern const uint8_t font_chord_keys[][FONT_CHORD_WIDTH];
extern const uint8_t font_chord_mods[][FONT_CHORD_WIDTH];

#endif
