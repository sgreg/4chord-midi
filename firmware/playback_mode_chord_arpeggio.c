/*
 * 4chord MIDI - Playback mode "Chord Arpeggio"
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
#include "playback.h"

playback_mode_t playback_mode_chord_arpeggio;

void
playback_mode_chord_arpeggio_cycle(chord_t *chord)
{
    switch (playback_mode_chord_arpeggio.count) {
        case 0:
            play_start_note(chord->third);
            play_start_note(chord->fifth);
            play_start_note(chord->octave);
            /* fall through */
        case 4:
            play_start_note(chord->root);
            break;
        case 1:
        case 5:
            play_start_note(chord->third);
            break;
        case 2:
        case 6:
            play_start_note(chord->fifth);
            break;
        case 3:
        case 7:
            play_start_note(chord->third);
            break;
    }
}

void
playback_mode_chord_arpeggio_stop(chord_t *chord)
{
    play_stop_note(chord->root);
    play_stop_note(chord->third);
    play_stop_note(chord->fifth);
    play_stop_note(chord->octave);
}

playback_mode_t playback_mode_chord_arpeggio = {
    .start = playback_mode_chord_arpeggio_cycle,
    .cycle = playback_mode_chord_arpeggio_cycle,
    .stop = playback_mode_chord_arpeggio_stop
};

