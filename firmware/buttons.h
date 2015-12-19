/*
 * 4chord midi - Input button handling
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
#ifndef _BUTTONS_H
#define _BUTTONS_H
#include <stdint.h>

/**
 * Button name enumeration.
 */
typedef enum {
    BUTTON_MENU_PREV,
    BUTTON_MENU_SELECT,
    BUTTON_MENU_NEXT,
    BUTTON_CHORD_1,
    BUTTON_CHORD_2,
    BUTTON_CHORD_3,
    BUTTON_CHORD_4,
    BUTTON_MAX
} button_name;


/**
 * Map controller input port pin to internal button handler.
 *
 * @param button Button name to assign based on button_name enumeration
 * @param port   Pointer to input port as defined in <avr/io.h>
 * @param pin    Pin number of the given port
 * @return       0 on success, -1 if button parameter is invalid
 */
int8_t button_map_port(button_name button, volatile uint8_t *port, uint8_t pin);

/**
 * Button input loop function.
 * Polls all mapped buttons and handles press/release callback functions.
 * Call this from inside the main loop.
 */
void button_input_loop(void);

#endif

