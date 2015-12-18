/*
 * 4chord midi - Menu selection
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
 *
 * TODO: add user definable settings for init values and increment values
 *
 */
#include <stdint.h>
#include "menu.h"
#include "gui.h"

/* currently selected menu item */
static menu_item_t menu_current;
/* currently selected tempo */
static uint8_t playback_tempo_current;
/* currently selected mode */
static playback_mode_item_t playback_mode_current;
/* currently selected chord */
static playback_key_item_t playback_key_current;

#define PLAYBACK_TEMPO_MIN 40
#define PLAYBACK_TEMPO_DEFAULT 120
#define PLAYBACK_TEMPO_MAX 180

/**
 * Select next menu item and update the LCD.
 * If the last menu item is already selected, it cycles back to the first one.
 * Note, unlike the menu item handling, the menu selection itself only goes
 * in one direction, there is no menu_prev() function as there is only one
 * select button.
 */
static void
menu_next(void)
{
    if (++menu_current == MENU_MAX) {
        menu_current = 0;
    }

    gui_set_menu(menu_current);
}

/**
 * Select the next playback mode and update the LCD.
 * Cycles back to the first item after the last one.
 */
static void
playback_mode_next(void)
{
    if (++playback_mode_current == PLAYBACK_MODE_MAX) {
        playback_mode_current = 0;
    }
    
    gui_set_playback_mode(playback_mode_current);
}

/**
 * Select the previous playback mode and update the LCD.
 * Cycles back to the last item after the first one.
 */
static void
playback_mode_prev(void)
{
    if (playback_mode_current == 0) {
        playback_mode_current = PLAYBACK_MODE_MAX - 1;
    } else {
        playback_mode_current--;
    }

    gui_set_playback_mode(playback_mode_current);
}

/**
 * Select the next playback key and update the LCD.
 * Cycles back to the first item after the last one.
 */
static void
playback_key_next(void)
{
    if (++playback_key_current == PLAYBACK_KEY_MAX) {
        playback_key_current = 0;
    }

    gui_set_playback_key(playback_key_current);
}

/**
 * Select the previous playback key and update the LCD.
 * Cycles back to the last item after the first one.
 */
static void
playback_key_prev(void)
{
    if (playback_key_current == 0) {
        playback_key_current = PLAYBACK_KEY_MAX - 1;
    } else {
        playback_key_current--;
    }

    gui_set_playback_key(playback_key_current);
}

/**
 * Increment the playback tempo and update the LCD.
 * If the maximum tempo PLAYBACK_TEMPO_MAX is already reached, the function
 * will not do anything and just keep the maximum value.
 */
static void
playback_tempo_up(void)
{
    if (playback_tempo_current < PLAYBACK_TEMPO_MAX) {
        playback_tempo_current++;
        gui_set_playback_tempo(playback_tempo_current);
    }
}

/**
 * Decrement the playback tempo and update the LCD.
 * If the minimum tempo PLAYBACK_TEMPO_MIN is already reached, the function
 * will not do anything and just keep the minimum value.
 */
static void
playback_tempo_down(void)
{
    if (playback_tempo_current > PLAYBACK_TEMPO_MIN) {
        playback_tempo_current--;
        gui_set_playback_tempo(playback_tempo_current);
    }
}


/**
 * Callback function array for handling menu_button_prev() execution according
 * to the currently selected menu item.
 */
static void (*menu_prev_handlers[3])(void) = {
    playback_mode_prev,
    playback_key_prev,
    playback_tempo_down
};

/**
 * Callback function array for handling menu_button_next() execution according
 * to the currently selected menu item.
 */
static void (*menu_next_handlers[3])(void) = {
    playback_mode_next,
    playback_key_next,
    playback_tempo_up
};


/**
 * Button press callback function for Menu Previous button
 * @param arg Unused
 */
void
menu_button_prev(void *arg __attribute__((unused)))
{
    menu_prev_handlers[menu_current]();
}

/**
 * Button press callback function for Menu Next button
 * @param arg Unused
 */
void
menu_button_next(void *arg __attribute__((unused)))
{
    menu_next_handlers[menu_current]();
}

/**
 * Button press callback function for Menu Select button
 * @param arg Unused
 */
void
menu_button_select(void *arg __attribute__((unused)))
{
    menu_next();
}


/**
 * Initialize the GUI menu.
 * Set up all internal default values and call the GUI functions to draw the
 * menu on the LCD.
 */
void
menu_init(void)
{
    menu_current           = MENU_MODE;
    playback_mode_current  = PLAYBACK_MODE_CHORD;
    playback_key_current   = PLAYBACK_KEY_C;
    playback_tempo_current = PLAYBACK_TEMPO_DEFAULT;

    gui_set_menu(menu_current);
    gui_set_playback_mode(playback_mode_current);
    gui_set_playback_key(playback_key_current);
    gui_set_playback_tempo(playback_tempo_current);
}


/**
 * Get the currently selected playback key.
 * @return Currently selected playback key
 */
playback_key_item_t
menu_get_current_playback_key(void)
{
    return playback_key_current;
}

/**
 * Get the currently selected playback mode.
 * @return Currently selected playback mode.
 */
playback_mode_item_t
menu_get_current_playback_mode(void)
{
    return playback_mode_current;
}

/**
 * Get the currently selected playback tempo.
 * @return Currently selected playback tempo.
 */
uint8_t
menu_get_current_playback_tempo(void)
{
    return playback_tempo_current;
}

