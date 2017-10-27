/*
 * 4chord MIDI - Menu selection
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
#ifndef _MENU_H_
#define _MENU_H_

/* Menu button identifier list */
typedef enum {
    MENU_BUTTON_PREV,
    MENU_BUTTON_SELECT,
    MENU_BUTTON_NEXT,
    MENU_BUTTON_MAX
} menu_button_t;

/* Menu item list */
typedef enum {
    MENU_MODE,
    MENU_KEY,
    MENU_TEMPO,
    MENU_MAX
} menu_item_t;

/* Playback key list */
typedef enum {
    PLAYBACK_KEY_C,
    PLAYBACK_KEY_C_SHARP,
    PLAYBACK_KEY_D,
    PLAYBACK_KEY_D_SHARP,
    PLAYBACK_KEY_E,
    PLAYBACK_KEY_F,
    PLAYBACK_KEY_F_SHARP,
    PLAYBACK_KEY_G,
    PLAYBACK_KEY_G_SHARP,
    PLAYBACK_KEY_A,
    PLAYBACK_KEY_B_FLAT,
    PLAYBACK_KEY_B,
    PLAYBACK_KEY_MAX
} playback_key_item_t;

/* Playback mode list */
typedef enum {
    PLAYBACK_MODE_CHORD,
    PLAYBACK_MODE_CHORD_ARP,
    PLAYBACK_MODE_CHORD_ARP_OCTAVE,
    PLAYBACK_MODE_ARP,
    PLAYBACK_MODE_ARP_OCTAVE,
    PLAYBACK_MODE_MAX
} playback_mode_item_t;


/**
 * Initialize the GUI menu.
 * Set up all internal default values and call the GUI functions to draw the
 * menu on the LCD.
 */
void menu_init(void);

/**
 * Get the currently selected playback key.
 * @return Currently selected playback key
 */
playback_key_item_t menu_get_current_playback_key(void);

/**
 * Get the currently selected playback mode.
 * @return Currently selected playback mode.
 */
playback_mode_item_t menu_get_current_playback_mode(void);

/**
 * Get the currently selected playback tempo.
 * @return Currently selected playback tempo.
 */
uint8_t menu_get_current_playback_tempo(void);

/**
 * Button press handler function for Menu Previous button.
 */
void menu_button_prev(void);

/**
 * Button press handler function for Menu Next button
 */
void menu_button_next(void);

/**
 * Button press handler function for Menu Select button
 */
void menu_button_select(void);


/**
 * Menu button press callback function.
 * Called when one of the three menu buttons is pressed. The pressed menu
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to menu_button_t
 */
void menu_button_press(void *arg);

/**
 * Button release callback function.
 * Called when one of the three menu buttons is released. The released menu
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer menu_button_t
 */
void menu_button_release(void *arg);

/**
 * Menu button poll function.
 *
 * Called from the main loop to periodically check the menu button press
 * state, and call the button's cycle function for long press cases.
 */
void menu_poll(void);

#endif
