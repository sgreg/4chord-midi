/*
 * 4chord midi - Playback mode "Chord Arpeggio Octave"
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
#include "playback.h"

playback_mode_t playback_mode_chord_arpeggio_octave;

void
playback_mode_chord_arpeggio_octave_cycle(chord_t *chord)
{
    switch (playback_mode_chord_arpeggio_octave.count) {
        case 0:
            play_stop_note(chord->octave);
            play_start_note(chord->root);
            play_start_note(chord->third);
            play_start_note(chord->fifth);
            break;
        case 1:
            play_start_note(chord->third);
            break;
        case 2:
            play_start_note(chord->fifth);
            break;
        case 3:
            play_start_note(chord->octave);
            break;
    }
}

void
playback_mode_chord_arpeggio_octave_stop(chord_t *chord)
{
    play_stop_note(chord->root);
    play_stop_note(chord->third);
    play_stop_note(chord->fifth);
    play_stop_note(chord->octave);
}

playback_mode_t playback_mode_chord_arpeggio_octave = {
    .start = playback_mode_chord_arpeggio_octave_cycle,
    .cycle = playback_mode_chord_arpeggio_octave_cycle,
    .stop = playback_mode_chord_arpeggio_octave_stop
};

