/*
 * 4chord midi - LCD graphical user interface
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 *
 * TODO  move current settings (menu, tempo, key, mode) somewhere else to handle better
 *
 */
#include <stdint.h>
#include "graphics.h"
#include "lcd3310.h"

/* currently selected menu item */
uint8_t menu_current;
/* currently selected tempo */
uint8_t tempo_current;
/* currently selected mode */
uint8_t mode_current;
/* currently selected chord */
uint8_t chord_current;

typedef enum {
    MENU_MODE,
    MENU_KEY,
    MENU_TEMPO,
    MENU_MAX
} gui_menu_items;

typedef enum {
    CHORD_C,
    CHORD_C_SHARP,
    CHORD_D,
    CHORD_D_SHARP,
    CHORD_E,
    CHORD_F,
    CHORD_F_SHARP,
    CHORD_G,
    CHORD_G_SHARP,
    CHORD_A,
    CHORD_B_FLAT,
    CHORD_B,
    CHORD_MAX
} gui_chord_items;

typedef enum {
    MODE_CHORD,
    MODE_ARP,
    MODE_ARP2,
    MODE_FOO,
    MODE_MAX
} gui_mode_items;

#define TEMPO_MIN 40
#define TEMPO_DEFAULT 120
#define TEMPO_MAX 180

/* graphics data array for menus */
static const unsigned char *menus[] = {
    gfx_menu_mode,
    gfx_menu_key,
    gfx_menu_tempo
};

/* graphics data array for tempo digits */
static const unsigned char *tempo_digits[] = {
    gfx_tempo_0,
    gfx_tempo_1,
    gfx_tempo_2,
    gfx_tempo_3,
    gfx_tempo_4,
    gfx_tempo_5,
    gfx_tempo_6,
    gfx_tempo_7,
    gfx_tempo_8,
    gfx_tempo_9,
};

/* graphics data array for modes */
static const unsigned char *modes[] = {
    gfx_mode_0,
    gfx_mode_1,
    gfx_mode_2,
    gfx_mode_3
};

/* graphics data array for key chords and modifiers */
static const unsigned char *chords[][2] = {
    {gfx_key_c, gfx_key_none},
    {gfx_key_c, gfx_key_sharp},
    {gfx_key_d, gfx_key_none},
    {gfx_key_d, gfx_key_sharp},
    {gfx_key_e, gfx_key_none},
    {gfx_key_f, gfx_key_none},
    {gfx_key_f, gfx_key_sharp},
    {gfx_key_g, gfx_key_none},
    {gfx_key_g, gfx_key_sharp},
    {gfx_key_a, gfx_key_none},
    {gfx_key_b, gfx_key_flat},
    {gfx_key_b, gfx_key_none},
};


/**
 * Set next menu item and update the LCD
 */
static void
menu_next(void)
{
    if (++menu_current == MENU_MAX) {
        menu_current = 0;
    }

    lcd_set_menu(menus[menu_current]);
    lcd_update();
}


/**
 * Decrement the playback mode and update the LCD.
 */
static void
mode_next(void) {
    if (++mode_current == MODE_MAX) {
        mode_current = 0;
    }

    lcd_set_mode(modes[mode_current]);
    lcd_update();
}

/**
 * Increment the playback mode and update LCD.
 */
static void
mode_prev(void) {
    if (--mode_current == 0xff) {
        mode_current = MODE_MAX - 1;
    }

    lcd_set_mode(modes[mode_current]);
    lcd_update();
}


/**
 * Write the currently selected key chord to the LCD memory.
 * Note, no LCD update is performed, call lcd_update() manually.
 */
static void
set_chord(const unsigned char *chord[2])
{
    lcd_set_chord(chord);
}

/**
 * Increment the key chord and update the LCD.
 */
static void
key_next(void) {
    if (++chord_current == CHORD_MAX) {
        chord_current = 0;
    }

    set_chord(chords[chord_current]);
    lcd_update();
}

/**
 * Decrement the key chord and update the LCD.
 */
static void
key_prev(void) {
    if (--chord_current == 0xff) {
        chord_current = CHORD_MAX - 1;
    }

    set_chord(chords[chord_current]);
    lcd_update();
}


/**
 * Set tempo to given value and update the LCD.
 * Takes tempo as 8 bit integer (>255bpm won't be supported anyway) and
 * splits it in three single digits. The digits' representing graphics
 * are then transferred to the LCD's tempo area.
 *
 * @param tempo New tempo
 */
static void
set_tempo(uint8_t tempo)
{
    const unsigned char *digit_graphics[3];
    uint8_t digits[3];
    uint8_t one, ten, hundred;

    /* split integer to three digits */
    one = tempo / 10;
    digits[2] = tempo - (one * 10);
    ten = one / 10;
    digits[1] = one - (ten * 10);
    hundred = ten / 10;
    digits[0] = ten - (hundred * 10);

    digit_graphics[0] = (digits[0] == 0) ? gfx_tempo_none : tempo_digits[digits[0]];
    digit_graphics[1] = tempo_digits[digits[1]];
    digit_graphics[2] = tempo_digits[digits[2]];

    lcd_set_tempo(digit_graphics);
    lcd_update();
}

/**
 * Increase tempo and call set_tempo() to update the LCD
 */
static void
tempo_up(void) {
    if (tempo_current < TEMPO_MAX) {
        tempo_current++;
        set_tempo(tempo_current);
    }
}

/**
 * Decrease tempo and call set_tempo() to update the LCD
 */
static void
tempo_down(void) {
    if (tempo_current > TEMPO_MIN) {
        tempo_current--;
        set_tempo(tempo_current);
    }
}

/**
 * Print the GUI on the display
 */
void
gui_start(void)
{
    menu_current  = MENU_MODE;
    chord_current = CHORD_C;
    tempo_current = TEMPO_DEFAULT;
    mode_current  = MODE_CHORD;

    lcd_clear();
    lcd_set_menu(menus[menu_current]);
    lcd_set_chord(chords[chord_current]);
    set_tempo(tempo_current);
    lcd_set_mode(modes[mode_current]);
    lcd_update();
}

/**
 * Print the startup logo on the display
 */
void
gui_printlogo(void)
{
    lcd_fullscreen(gfx_logo);
}


/* functions to call from gui_menu_next() depending on the current state */
static void (*menu_next_handlers[3])(void) = {
    mode_next,
    key_next,
    tempo_up
};

/**
 * Button press callback function for Menu Next button
 * @param arg Unused
 */
void
gui_menu_next(void *arg __attribute__((unused)))
{
    menu_next_handlers[menu_current]();
}

/* functions to call from gui_menu_prev() depending on the current state */
void (*menu_prev_handlers[3])(void) = {
    mode_prev,
    key_prev,
    tempo_down
};

/**
 * Button press callback function for Menu Previous button
 * @param arg Unused
 */
void
gui_menu_prev(void *arg __attribute__((unused)))
{
    menu_prev_handlers[menu_current]();
}


/**
 * Button press callback function for Menu Select button
 * @param arg Unused
 */
void
gui_menu_select(void *arg __attribute__((unused)))
{
    menu_next();
}

