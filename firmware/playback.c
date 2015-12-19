/*
 * 4chord midi - MIDI playback
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
#include <avr/pgmspace.h>
#include "menu.h"
#include "usb.h"

/* root notes array for each chord (I, V, vi, IV) in all keys (C..B) */
static const uint8_t root_notes[PLAYBACK_KEY_MAX][4] PROGMEM = {
    {48, 43, 45, 41},   /* C3   G2  A2  F2  */
    {49, 44, 46, 42},   /* C#3  G#2 Bb2 F#2 */
    {50, 45, 47, 43},   /* D3   A2  B2  G2  */
    {51, 46, 48, 44},   /* D#3  Bb2 C3  G#2 */
    {40, 47, 49, 45},   /* E2   B2  C#3 A2  */
    {41, 48, 50, 46},   /* F2   C3  D3  Bb2 */
    {42, 49, 51, 47},   /* F#2  C#3 D#3 B2  */
    {43, 50, 40, 48},   /* G2   D3  E2  C3  */
    {44, 51, 41, 49},   /* G#2  D#3 F2  C#3 */
    {45, 40, 42, 50},   /* A2   E2  F#2 D3  */
    {46, 41, 43, 51},   /* Bb2  F2  G2  D#3 */
    {47, 42, 44, 40}    /* B2   F#2 G#2 E2  */
};

/* list of offsets to the chord's major (I, V, IV) or minor (vi) third */
static const uint8_t third_offset[] = {4, 4, 3, 4};

/* offset to the chord's perfect fifth, same for all */
static const uint8_t fifth_offset = 7;

/* MIDI note on velocity */
#define VELOCITY 0x7f

/* button press status */
static uint8_t pressed;

/* triad chord structure (root, major/minor third, perfect fifth) */
typedef struct {
    uint8_t root;
    uint8_t third;
    uint8_t fifth;
} chord_t;

/* global currently selected chord to play */
static chord_t chord;


/**
 * Construct the triad for a given chord number.
 * This sets up the global chord structure based on the current playback key
 * selected in the menu and the progression's chord number given by the button
 * press callback function.
 *
 * @param chord_num Chord number given by the button press callback
 */
static void
construct_chord(uint8_t chord_num)
{
    uint8_t key = menu_get_current_playback_key();

    chord.root  = pgm_read_byte(&root_notes[key][chord_num]);
    chord.third = chord.root + third_offset[chord_num];
    chord.fifth = chord.root + fifth_offset;
}

/**
 * Button press callback function.
 * Called when one of the four chord buttons is pressed. The pressed chord
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to uint8_t
 */
void
playback_button_press(void *arg)
{
    uint8_t chord_num = *((uint8_t *) arg);

    if (!pressed) {
        construct_chord(chord_num);
        midi_msg_note_on(chord.root, VELOCITY);
        midi_msg_note_on(chord.third, VELOCITY);
        midi_msg_note_on(chord.fifth, VELOCITY);
        pressed = 1;
    }
}

/**
 * Button release callback function.
 * Called when one of the four chord buttons is released. The released chord
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer to uint8_t (unused)
 */
void
playback_button_release(void *arg __attribute__((unused)))
{
    midi_msg_note_off(chord.root, VELOCITY);
    midi_msg_note_off(chord.third, VELOCITY);
    midi_msg_note_off(chord.fifth, VELOCITY);
    pressed = 0;
}

