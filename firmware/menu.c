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
 *
 * TODO: add user definable settings for init values and increment values
 *
 */
#include <stdint.h>
#include <stdio.h>
#include "menu.h"
#include "gui.h"
#include "playback.h"
#include "spi.h" // XXX temporary to inverse display on "select" long press
#include "timer.h"

/* currently selected menu item */
static menu_item_t menu_current;
/* currently selected tempo */
static uint8_t playback_tempo_current;
/* currently selected mode */
static playback_mode_item_t playback_mode_current;
/* currently selected chord */
static playback_key_item_t playback_key_current;
/* currently selected metre */
static playback_metre_item_t playback_metre_current;

#define PLAYBACK_TEMPO_MIN 30
#define PLAYBACK_TEMPO_DEFAULT 120
#define PLAYBACK_TEMPO_MAX 240

#define DELAY_LONG_PRESS    0x44a8 /* 1.5 seconds */
#define DELAY_CYCLE_SLOW    0x16e2 /* 0.5 seconds */
#define DELAY_CYCLE_FAST    0x0492 /* 0.1 seconds */

/**
 * Select next menu item and update the LCD.
 * If the last menu item is already selected, it cycles back to the first one.
 * Note, unlike the menu item handling, the menu selection itself only goes
 * in one direction, as there is only one select button.
 */
static void
menu_select(void)
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
 * Select the next playback metre and update the LCD.
 * Cycles back to the first item after the last one.
 */
static void
playback_metre_next(void)
{
    if (++playback_metre_current == PLAYBACK_METRE_MAX) {
        playback_metre_current = 0;
    }

    gui_set_playback_metre(playback_metre_current);
}

/**
 * Select the previous playback metre and update the LCD.
 * Cycles back to the last item after the first one.
 */
static void
playback_metre_prev(void)
{
    if (playback_metre_current == 0) {
        playback_metre_current = PLAYBACK_METRE_MAX - 1;
    } else {
        playback_metre_current--;
    }

    gui_set_playback_metre(playback_metre_current);
}


/**
 * Callback function array for handling menu_button_prev() execution according
 * to the currently selected menu item.
 */
static void (*menu_prev_handlers[])(void) = {
    playback_key_prev,
    playback_mode_prev,
    playback_tempo_down,
    playback_metre_prev
};

/**
 * Callback function array for handling menu_button_next() execution according
 * to the currently selected menu item.
 */
static void (*menu_next_handlers[])(void) = {
    playback_key_next,
    playback_mode_next,
    playback_tempo_up,
    playback_metre_next
};


/**
 * Button press handler function for Menu Previous button.
 * Executes the callback function for the currently selected menu item.
 */
void
menu_button_prev(void)
{
    menu_prev_handlers[menu_current]();
}

/**
 * Button press handler function for Menu Next button
 * Executes the callback function for the currently selected menu item.
 */
void
menu_button_next(void)
{
    menu_next_handlers[menu_current]();
}

/**
 * Button press handler function for Menu Select button
 */
void
menu_button_select(void)
{
    menu_select();
}


/**
 * Initialize the GUI menu.
 * Set up all internal default values and call the GUI functions to draw the
 * menu on the LCD.
 */
void
menu_init(void)
{
    menu_current           = MENU_KEY;
    playback_mode_current  = PLAYBACK_MODE_CHORD;
    playback_key_current   = PLAYBACK_KEY_C;
    playback_tempo_current = PLAYBACK_TEMPO_DEFAULT;
    playback_metre_current = PLAYBACK_METRE_4_4;

    gui_set_menu(menu_current);
    gui_set_playback_mode(playback_mode_current);
    gui_set_playback_key(playback_key_current);
    gui_set_playback_tempo(playback_tempo_current);
    gui_set_playback_metre(playback_metre_current);
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

/**
 * Get the currently selected playback metre.
 * @return Currently selected playback metre.
 */
uint8_t
menu_get_current_playback_metre(void)
{
    return playback_metre_current;
}


/* button press status */
static uint8_t pressed;
/* timer trigger status */
static uint8_t menu_timer_triggered;

/* menu handle callback function */
typedef void (*menu_handle_callback_t)(void);

/*
 * menu button handler structure
 *
 * The cycle callback is executed whenever timer1 triggers an interrupt
 * which is based on the init_delay and cont_delay values.
 *
 * On initial start, the start callback is executed, and timer1 is set to
 * the init_delay value, after which the cycle callback is executed, if
 * it's not NULL. If cont_delay_cycles is 0, that's the end to it; timer1
 * is stopped and it's basically as if the button was released.
 *
 * However, if cont_delay_cycles is >0, a cycle counter is increased every
 * time timer1 triggers an interrupt. Once the cycle counter value matches
 * the cont_delay_cycles value, timer1 is restarted with the cont_delay
 * value, and continues calling the cycle callback for all eternity (well,
 * or until the button is released).
 */
typedef struct {
    /* start callback, executed once on button press, skipped if NULL */
    menu_handle_callback_t start;
    /* cycle callback, executed during long button press, skipped if NULL */
    menu_handle_callback_t cycle;
    /* long button press initial delay value for timer1 */
    uint16_t init_delay;
    /* long button press continued delay value for timer1 */
    uint16_t cont_delay;
    /* number of cycles of switch from initial delay to cotinued delay */
    uint8_t cont_delay_cycles;
} menu_handler_t;

/* cycle counter to match against menu handler's cont_delay_cycles value */
static uint8_t cycle_counter;
/* flag to check if cycle counter needs to be further increased or not */
static uint8_t cycle_handled;


/**
 * Toggle the LCD's inverse video mode.
 *
 * This is just temporarily to have somethig happening when long pressing
 * the "Select" menu button. Later on this will be replaced by a general
 * settings menu opening up.
 */
static void toggle_inverse(void) {
    static uint8_t inverse;
    spi_send_command((inverse) ? 0x0c : 0x0d);
    inverse = !inverse;
}

/* menu handler structure for "<" button */
static menu_handler_t menu_handler_prev = {
    .start = menu_button_prev,
    .cycle = menu_button_prev,
    .init_delay = DELAY_CYCLE_SLOW,
    .cont_delay = DELAY_CYCLE_FAST,
    .cont_delay_cycles = 3
};

/* menu handler structure for "Select" button */
static menu_handler_t menu_handler_select =  {
    .start = menu_button_select,
    .cycle = toggle_inverse, // TODO add settings menu here later
    .init_delay = DELAY_LONG_PRESS,
    .cont_delay_cycles = 0
};

/* menu handler structure for ">" button */
static menu_handler_t menu_handler_next = {
    .start = menu_button_next,
    .cycle = menu_button_next,
    .init_delay = DELAY_CYCLE_SLOW,
    .cont_delay = DELAY_CYCLE_FAST,
    .cont_delay_cycles = 3
};

/* array for all the menu handler structures */
static menu_handler_t *menu_handlers[] = {
    &menu_handler_prev,
    &menu_handler_select,
    &menu_handler_next
};

/* pointer to the currently active menu handler structure */
static menu_handler_t *current_handler;

/**
 * Timer compare match interrupt callback.
 * This is called from the timer1 interrupt handler. So to keep it as
 * short as possible, only a flag is set here that is then checked and
 * further processed in the menu_poll() function.
 */
static void
menu_timer_callback(void)
{
    menu_timer_triggered = 1;
}


/**
 * Menu button press callback function.
 * Called when one of the three menu buttons is pressed. The pressed menu
 * button number is given in the arg parameter.
 *
 * @param arg Pressed button number, given as pointer to menu_button_t
 */
void
menu_button_press(void *arg)
{
    menu_button_t menu_button_index = *((menu_button_t *) arg);

    if (!pressed) {
        current_handler = menu_handlers[menu_button_index];

        if (current_handler->start != NULL) {
            current_handler->start();
        }

        if (!playback_ongoing()) {
            /*
             * don't do long press if playback is ongoing, i.e. any of the
             * chord buttons is pressed. Playback has priority and the long
             * press handling will take over timer1 and stop the playback.
             *
             * TODO make this some configuration setting later on
             */
            timer1_start(current_handler->init_delay, menu_timer_callback);
            cycle_counter = 0;
            cycle_handled = 0;
        }

        pressed = 1;
    }
}

/**
 * Button release callback function.
 * Called when one of the three menu buttons is released. The released menu
 * button number is given in the arg parameter.
 *
 * @param arg Released button number, given as pointer menu_button_t
 */
void
menu_button_release(void *arg __attribute__((unused)))
{
    pressed = 0;

    if (!playback_ongoing()) {
        /*
         * also here, don't interfere with timer1 if playback is ongoing.
         *
         * TODO same as in menu_button_press, this could be a setting
         */
        timer1_stop();
    }
}

/**
 * Menu button poll function.
 *
 * Called from the main loop to periodically check the menu button press
 * state, and call the button's cycle function for long press cases.
 */
void
menu_poll(void)
{
    if (menu_timer_triggered) {
        if (current_handler->cycle != NULL) {
            current_handler->cycle();
        }

        if (cycle_handled) {
            /* do nothing else */

        } else if (current_handler->cont_delay_cycles == 0) {
            /* one time shot, done with that */
            timer1_stop();
            cycle_handled = 1;

        } else if (++cycle_counter == current_handler->cont_delay_cycles) {
            /* next cycle step, speed it up and be done with it */
            timer1_start(current_handler->cont_delay, menu_timer_callback);
            cycle_handled = 1;
        }

        menu_timer_triggered = 0;
    }
}
