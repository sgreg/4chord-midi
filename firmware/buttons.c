/*
 * 4chord midi - Input button handling
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#include <stdio.h>
#include <avr/io.h>
#include "buttons.h"
#include "chords.h"
#include "gui.h"


/* internal button states */
typedef enum {
    STATE_RELEASED,
    STATE_PRESSED
} button_state;

/* button handler callback function */
typedef void (*button_callback)(void *);

/*
 * Internal button handler structure
 */
struct button_handler {
    /* set button active, ignore state handling if not */ 
    uint8_t active;
    /* controller port name connected to button */
    volatile uint8_t *port;
    /* controller port pin number connected to button */
    uint8_t pin;
    /* internal state of button */
    button_state state;
    /* previous state of button */
    button_state laststate;
    /* if set, pressed callback handler is called only once, otherwise periodically */
    uint8_t oneshot;
    /* button release and press callback handlers */
    union {
        button_callback callbacks[2];
        struct {
            button_callback released_cb;
            button_callback pressed_cb;
        };
    };
    /* button specific argument passed to callback handlers */
    void *callback_arg;
};

/* set up button handling and callbacks */
static struct button_handler button_handlers[BUTTON_MAX] = {
    {   /* BUTTON_MENU_PREV */
        .oneshot = 1,
        .callbacks = {
            NULL,
            gui_menu_prev
        }
    },
    {   /* BUTTON_MENU_SELECT */
        .oneshot = 1,
        .callbacks = {
            NULL,
            gui_menu_select
        }
    },
    {   /* BUTTON_MENU_NEXT */
        .oneshot = 1,
        .callbacks = {
            NULL,
            gui_menu_next
        }
    },
    {   /* BUTTON_CHORD_1 */
        .oneshot = 0,
        .callbacks = {
            chord_release,
            chord_press,
        },
        .callback_arg = (uint8_t *) 0
    },
    {   /* BUTTON_CHORD_2 */
        .oneshot = 0,
        .callbacks = {
            chord_release,
            chord_press,
        },
        .callback_arg = (uint8_t *) 1
    },
    {   /* BUTTON_CHORD_3 */
        .oneshot = 0,
        .callbacks = {
            chord_release,
            chord_press,
        },
        .callback_arg = (uint8_t *) 2
    },
    {   /* BUTTON_CHORD_4 */
        .oneshot = 0,
        .callbacks = {
            chord_release,
            chord_press,
        },
        .callback_arg = (uint8_t *) 3
    }
};


/**
 * Poll all active mapped input ports.
 * Sets interal state of button handlers according to current button state.
 */
static void
buttons_poll(void)
{
    uint8_t i;
    struct button_handler *handler;

    for (i = 0; i < BUTTON_MAX; i++) {
        handler = &button_handlers[i];
        if (handler->active && (*handler->port & (1 << handler->pin)) == 0) {
            handler->state = STATE_PRESSED;
        } else {
            handler->state = STATE_RELEASED;
        }
    }
}


/**
 * Handle all active buttons after polling.
 * If button port is active and has a callback function for the current state,
 * the callback function is called.
 * If the button is pressed, the callback is called either once or every time
 * the function is run, depending on the handler's oneshot value.
 * If the button is released, the callback is called only once regardless of
 * the oneshot value.
 */
static void
buttons_handle(void)
{
    uint8_t i;
    struct button_handler *handler;

    for (i = 0; i < BUTTON_MAX; i++) {
        handler = &button_handlers[i];

        if (handler->active && handler->callbacks[handler->state]) {
            if (handler->state != handler->laststate ||
               (!handler->oneshot && handler->state != STATE_RELEASED))
            {
                handler->callbacks[handler->state](&handler->callback_arg);
            }
        }
        handler->laststate = handler->state;
    }
}


/**
 * Button input loop function.
 * Polls all mapped buttons and handles press/release callback functions.
 * Call this from inside the main loop.
 */
void
button_input_loop(void)
{
    buttons_poll();
    buttons_handle();
}


/**
 * Map controller input port pin to internal button handler.
 *
 * @param button Button name to assign based on button_name enumeration
 * @param port   Pointer to input port as defined in <avr/io.h>
 * @param pin    Pin number of the given port
 * @return       0 on success, -1 if button parameter is invalid
 */
int8_t
button_map_port(button_name button, volatile uint8_t *port, uint8_t pin)
{
    if (button >= BUTTON_MAX) {
        return -1;
    }

    button_handlers[button].port = port;
    button_handlers[button].pin  = pin;
    button_handlers[button].active = 1;

    return 0;
}

