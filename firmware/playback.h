/*
 * 4chord MIDI - MIDI playback
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
#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_
#include <stdint.h>

/* triad chord structure (plus octave) */
typedef struct {
    /* root note */
    uint8_t root;
    /* major (in chord I, V and IV) or minor (in chord vi) third */
    uint8_t third;
    /* perfect fifth */
    uint8_t fifth;
    /* octave */
    uint8_t octave;
} chord_t;

/* playback mode callback function */
typedef void (*playback_mode_callback_t)(chord_t *);

/* playback mode structure */
typedef struct {
    /* beat counter in 4/4 timing, i.e. 0..3 */
    uint8_t count;
    /* start callback, executed once on button press, skipped if NULL */
    playback_mode_callback_t start;
    /* cycle callback, executed periodically based on tempo, skipped if NULL */
    playback_mode_callback_t cycle;
    /* stop callback, executed once on button release, skipped if NULL */
    playback_mode_callback_t stop;
} playback_mode_t;


/**
 * Button press callback function.
 * Called when one of the four chord buttons is pressed. The pressed chord
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to uint8_t
 */
void playback_button_press(void *arg);

/**
 * Button release callback function.
 * Called when one of the four chord buttons is released. The released chord
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer to uint8_t
 */
void playback_button_release(void *arg);

/**
 * Start playing a given MIDI note.
 * Sends the note via USB as MIDI Note On message.
 *
 * @param note MIDI note to start playing
 */
void play_start_note(uint8_t note);

/**
 * Stop playing a given MIDI note.
 * Sends the note via USB as MIDI Note Off message.
 *
 * @param note MIDI note to stop playing
 */
void play_stop_note(uint8_t note);

/**
 * Playback mode poll function.
 * If the cycle callback function is used by the playback mode, a timer is
 * started according to the currently selected playback tempo. This function
 * polls the status of the timer interrupt; if the cycle period is elapsed,
 * the cycle callback function is executed.
 */
void playback_poll(void);

#endif
