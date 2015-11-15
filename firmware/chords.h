/*
 * 4chord midi - Chord playback
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#ifndef _CHORDS_H_
#define _CHORDS_H_

/**
 * Button press callback function.
 * Called when one of the four chord buttons is pressed. The pressed chord
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to uint8_t
 */
void chord_press(void *arg);

/**
 * Button release callback function.
 * Called when one of the four chord buttons is released. The released chord
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer to uint8_t
 */
void chord_release(void *arg);

#endif
