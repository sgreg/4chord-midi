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
#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "menu.h"
#include "playback.h"
#include "timer.h"
#include "usb.h"

/* root notes array for each chord (I, V, vi, IV) in all keys (C..B) */
static const uint8_t root_notes[PLAYBACK_KEY_MAX][4] PROGMEM = {
    {48, 43, 45, 41},   /* C3   G2  A2  F2  */
    {49, 44, 46, 42},   /* C#3  G#2 Bb2 F#2 */
    {50, 45, 47, 43},   /* D3   A2  B2  G2  */
    {51, 46, 48, 44},   /* D#3  Bb2 C3  G#2 */
    {52, 47, 49, 45},   /* E3   B2  C#3 A2  */
    {53, 48, 50, 46},   /* F3   C3  D3  Bb2 */
    {54, 49, 51, 47},   /* F#3  C#3 D#3 B2  */
    {43, 50, 52, 48},   /* G2   D3  E3  C3  */
    {44, 51, 53, 49},   /* G#2  D#3 F3  C#3 */
    {45, 52, 54, 50},   /* A2   E3  F#3 D3  */
    {46, 41, 43, 39},   /* Bb2  F2  G2  D#2 */
    {47, 42, 44, 40}    /* B2   F#2 G#2 E2  */
};

/* list of offsets to the chord's major (I, V, IV) or minor (vi) third */
static const uint8_t third_offset[] = {4, 4, 3, 4};

/* offset to the chord's perfect fifth, same for all */
static const uint8_t fifth_offset = 7;

/* offset to the root octave */
static const uint8_t octave_offset = 12;

/* MIDI note on velocity */
#define VELOCITY 0x7f

/* button press status */
static uint8_t pressed;

extern playback_mode_t playback_mode_chord;
extern playback_mode_t playback_mode_chord_arpeggio;
extern playback_mode_t playback_mode_chord_arpeggio_octave;
extern playback_mode_t playback_mode_arpeggio;
extern playback_mode_t playback_mode_arpeggio_octave;

/* array of available playback mode structures */
static playback_mode_t *playback_modes[PLAYBACK_MODE_MAX] = {
    &playback_mode_chord,
    &playback_mode_chord_arpeggio,
    &playback_mode_chord_arpeggio_octave,
    &playback_mode_arpeggio,
    &playback_mode_arpeggio_octave
};

/* pointer to currently active playback mode, set in button press handler */
static playback_mode_t *playback_mode;

/* timer trigger status */
static uint8_t playback_timer_triggered;

/* currently selected chord to play */
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

    chord.root   = pgm_read_byte(&root_notes[key][chord_num]);
    chord.third  = chord.root + third_offset[chord_num];
    chord.fifth  = chord.root + fifth_offset;
    chord.octave = chord.root + octave_offset;
}

/**
 * Set up the timer value based on the current playback tempo.
 * The actual playback will be based on 1/8 notes, but the interval is
 * based on 1/4 notes (at least for now, as there's only 4/4 metre),
 * so this will return half of the BPM interval.
 */
static uint16_t
playback_interval(void)
{
    uint8_t  tempo    = menu_get_current_playback_tempo();
    uint32_t minute   = TICKS_PER_CYCLE * 60;
    uint16_t interval = minute / tempo;

    return interval >> 1;
}

/**
 * Timer compare match interrupt callback.
 * Use the playback_poll() function to check its state.
 */
static void
playback_cycle_timer_callback(void)
{
    if (++playback_mode->count == 8) {
        playback_mode->count = 0;
    }
    playback_timer_triggered = 1;
}

/**
 * Playback mode poll function.
 * If the cycle callback function is used by the playback mode, a timer is
 * started according to the currently selected playback tempo. This function
 * polls the status of the timer interrupt; if the cycle period is elapsed,
 * the cycle callback function is executed.
 */
void
playback_poll(void)
{
    if (playback_timer_triggered) {
        if (playback_mode->cycle != NULL) {
            playback_mode->cycle(&chord);
        }

        playback_timer_triggered = 0;
    }
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
        playback_mode = playback_modes[menu_get_current_playback_mode()];
        if (playback_mode->start != NULL) {
            playback_mode->start(&chord);
        }

        if (playback_mode->cycle != NULL) {
            timer1_start(playback_interval(), playback_cycle_timer_callback);
        }
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
    pressed = 0;
    timer1_stop();
    playback_mode->count = 0;

    if (playback_mode->stop != NULL) {
        playback_mode->stop(&chord);
    }
}


/**
 * Start playing a given MIDI note.
 * Sends the note via USB as MIDI Note On message.
 *
 * @param note MIDI note to start playing
 */
void
play_start_note(uint8_t note)
{
    midi_msg_note_on(note, VELOCITY);
}

/**
 * Stop playing a given MIDI note.
 * Sends the note via USB as MIDI Note Off message.
 *
 * @param note MIDI note to stop playing
 */
void
play_stop_note(uint8_t note)
{
    midi_msg_note_off(note, VELOCITY);
}

