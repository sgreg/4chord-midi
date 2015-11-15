/*
 * 4chord midi - Chord playback
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#include <stdint.h>
#include "uart.h"

/**
 * Button press callback function.
 * Called when one of the four chord buttons is pressed. The pressed chord
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to uint8_t
 */
void chord_press(void *arg)
{
    // for now, just write some debug info to UART
    uint8_t chord_num = *((uint8_t *) arg);
    uart_print("chord ");
    uart_putint(chord_num, 1);
    uart_print(" pressed");
    uart_newline();
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
    // for now, just write some debug info to UART
    uint8_t chord_num = *((uint8_t *) arg);
    uart_print("chord ");
    uart_putint(chord_num, 1);
    uart_print(" released");
    uart_newline();
}

