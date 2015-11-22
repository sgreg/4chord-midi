/*
 * 4chord midi - Chord playback
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#include <stdint.h>
#include "uart.h"
#include "usb.h"

/* single notes array for each chord key */
static const uint8_t notes[] = {48, 55, 57, 53}; /* C3, G3, A3, F3 */
#define VELOCITY 0x60

/* key pressed, could also set button as oneshot, but that'll have to change anyway later */
static uint8_t pressed;

/**
 * Button press callback function.
 * Called when one of the four chord buttons is pressed. The pressed chord
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to uint8_t
 */

void chord_press(void *arg)
{
    uint8_t chord_num = *((uint8_t *) arg);

    if (!pressed) {
        midi_msg_note_on(notes[chord_num], VELOCITY);
        pressed = 1;
    }
}

/**
 * Button release callback function.
 * Called when one of the four chord buttons is released. The released chord
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer to uint8_t
 */
void chord_release(void *arg)
{
    uint8_t chord_num = *((uint8_t *) arg);

    midi_msg_note_off(notes[chord_num], VELOCITY);
    pressed = 0;
}

