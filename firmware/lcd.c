/*
 * 4chord MIDI - Nokia LCD handling
 *
 * Copyright (C) 2020 Sven Gregori <sven@craplab.fi>
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
 * The Nokia 5110 LCD is a 48x84px display that is arranged in 6 rows of
 * 8x84px each. Each row is therefore 84 byte wide with each byte representing
 * the 8 pixel height of the row.
 * 
 * The 4chord MIDI user interface is arranged in 5 areas:
 *      1. Menu (playback mode, key, tempo)
 *      2. Chord key (C, D, E, F, G, A, B)
 *      3. Chord modifier (none, sharp, flat)
 *      4. Playback tempo
 *      5. Playback mode
 *
 * The areas are arranged on the display as following:
 * +------------------------------------------+
 * |111111111111111111111111111111111111111111|
 * |2222222222222222 33333333 4444444444444444|
 * |2222222222222222 33333333 4444444444444444|
 * |2222222222222222 888  777 5555555555555555|
 * |2222222222222222 888  777 5555555555555555|
 * |666666666666666666666666666666666666666666|
 * +------------------------------------------+
 * 
 * One character represents 8 byte height (i.e. one row) and 2 byte width.
 *
 * Areas x/y arrangements:
 *
 * 1 Menu           y0      x0-83   1*84 px
 * 2 Key            y1-4    x0-31   4*32 px
 * 3 Key mod        y1-2    x34-49  2*16 px
 * 4 Tempo          y1-2    x52-83  2*32 px
 * 5 Mode           y3-4    x52-83  2*32 px
 * 6 Chord list     y6      x0-84   1*64 px
 * 7 Metre          y3-4    x45-49  2*5  px
 * 8 Metronome      y3-4    x34-39  2*6  px
 */
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "fonts.h"
#include "lcd.h"
#include "menu.h"
#include "spi.h"
#include "xbmlib.h"

#define LCD_START_LINE_ADDR (66-2)
#define LCD_X_RES   84
#define LCD_Y_RES   48
#define LCD_MEMORY_SIZE     ((LCD_X_RES * LCD_Y_RES) / 8)

/* display arrangement for the menu area */
#define MENU_X   0
#define MENU_Y   0
#define MENU_W  84
#define MENU_H   1

/* display arrangement for the chord key area */
#define KEY_X    0
#define KEY_Y    1
#define KEY_W   32
#define KEY_H    4

/* display arrangement for the chord modifier area */
#define KEYMOD_X    32
#define KEYMOD_Y     1
#define KEYMOD_W    16
#define KEYMOD_H     2

/* display arrangement for the tempo areas */
#define TEMPO_DIGIT1_X  52
#define TEMPO_DIGIT2_X  62
#define TEMPO_DIGIT3_X  72
#define TEMPO_Y          1
#define TEMPO_W          8
#define TEMPO_H          2

/* display arrangement for the mode area */
#define MODE_X  52
#define MODE_Y   3
#define MODE_W  32
#define MODE_H   2

/* display arrangement for the chord list area */
#define LIST_CHORD_I_X   2
#define LIST_CHORD_V_X  23
#define LIST_CHORD_vi_X 44
#define LIST_CHORD_IV_X 65
#define LIST_Y           5
#define LIST_CHORD_W    17
#define LIST_H           1

/* display arrangement for the metre area */
#define METRE_X     45
#define METRE_TOP_Y  3
#define METRE_BOT_Y  4
#define METRE_W      5
#define METRE_H      1

/* display arrangement for the metronome area */
#define MET_X       34
#define MET_TOP_Y    3
#define MET_BOT_Y    4
#define MET_W        6
#define MET_H        1


/**
 * Initialize the LCD.
 */
void
lcd_init(void)
{
    /* taken from Olimex Arduino example */
    spi_send_command(0x21);
    spi_send_command(0xc8);
    spi_send_command(0x04 | !!(LCD_START_LINE_ADDR & (1u << 6)));
    spi_send_command(0x40 | (LCD_START_LINE_ADDR & ((1u << 6) -1)));
    spi_send_command(0x12);
    spi_send_command(0x20);
    spi_send_command(0x08);
    spi_send_command(0x0c);
}

/**
 * Clear the LCD by writing all zeros to it.
 */
void
lcd_clear(void)
{
    uint16_t addr;

    spi_send_command(0x80); // set X addr to 0x00
    spi_send_command(0x40); // set Y addr to 0x00

    for (addr = 0; addr < LCD_MEMORY_SIZE; addr++) {
        spi_send_data(0x00);
    }
}

/**
 * Display fullscreen image data on the LCD.
 * Note, data is expected to be stored in PROGMEM and contain the
 * full display size of data (i.e. LCD_MEMORY_SIZE bytes)
 *
 * @param data full screen PROGMEM data to display
 */
static void
lcd_write_full_frame(const uint8_t *data)
{
    uint16_t addr;

    spi_send_command(0x80); // set X addr to 0x00
    spi_send_command(0x40); // set Y addr to 0x00

    for (addr = 0; addr < LCD_MEMORY_SIZE; addr++) {
        spi_send_data(pgm_read_byte(&data[addr]));
    }
}

/**
 * Display diff frame image on the LCD.
 * Writes given diff frame data one by one to the x and y position
 * the diffs' addresses correspond to.
 *
 * @param frame diff frame date to display
 */
static void
lcd_write_diff_frame(const struct xbmlib_diff_frame *frame)
{
    uint8_t diff_index;
    uint8_t diffcnt = pgm_read_byte(&(frame->diffcnt));
    struct xbmlib_diff diff;
    uint8_t x, y;
    uint16_t full_addr;
    uint8_t last_addr = 0;
    uint8_t offset = 0;

    for (diff_index = 0; diff_index < diffcnt; diff_index++) {
        diff.addr = pgm_read_word(&(frame->diffs[diff_index].addr));
        diff.data = pgm_read_byte(&(frame->diffs[diff_index].data));

        if (last_addr > diff.addr) {
            offset++;
        }

        last_addr = diff.addr;
        full_addr = diff.addr + 256 * offset;

        y = full_addr / LCD_X_RES;
        x = full_addr - y * LCD_X_RES;

        spi_send_command(0x80 | x); // set X addr to `x`
        spi_send_command(0x40 | y); // set Y addr to `y`
        spi_send_data(diff.data);
    }
}

/**
 * Display key diff frame image on the LCD.
 * Writes given key diff frame data one by one to the x and y position
 * the diffs' addresses correspond to. For all other addresses, the
 * base_value stored in the given diff frame struct is written.
 *
 * @param frame key diff frame date to display
 */
static void
lcd_write_key_diff_frame(const struct xbmlib_key_diff_frame *frame)
{
    uint8_t diff_index;
    uint8_t base_value = pgm_read_byte(&(frame->base_value));
    uint8_t diffcnt = pgm_read_byte(&(frame->diffcnt));
    struct xbmlib_diff diff;
    uint16_t full_addr;
    uint8_t last_addr = 0;
    uint8_t offset = 0;
    uint16_t current_addr = 0;

    spi_send_command(0x80); // set X addr to 0x00
    spi_send_command(0x40); // set Y addr to 0x00

    for (diff_index = 0; diff_index < diffcnt; diff_index++) {
        diff.addr = pgm_read_word(&(frame->diffs[diff_index].addr));
        diff.data = pgm_read_byte(&(frame->diffs[diff_index].data));

        if (last_addr > diff.addr) {
            offset++;
        }

        last_addr = diff.addr;
        full_addr = diff.addr + 256 * offset;

        while (current_addr++ < full_addr) {
            spi_send_data(base_value);
        }

        spi_send_data(diff.data);
    }

    while (current_addr++ < LCD_MEMORY_SIZE) {
        spi_send_data(base_value);
    }
}

/**
 * Display the given xbmlib frame on the display.
 * Wrapper function for all nokia_lcd_write_*_frame() functions, taking
 * a xbmlib frame struct and selects the right write function based on
 * the frame's type.
 *
 * @param frame xbmlib frame data referencing the actual data to display
 */
void
lcd_write_frame(const struct xbmlib_frame *frame)
{
    uint8_t frame_type = pgm_read_byte(&frame->type);
    void *ptr = pgm_read_ptr(&frame->data);

    switch (frame_type) {
        case TYPE_FULL:
            lcd_write_full_frame(ptr);
            break;
        case TYPE_DIFF:
            lcd_write_diff_frame(ptr);
            break;
        case TYPE_KEY_DIFF:
            lcd_write_key_diff_frame(ptr);
            break;
    }
}

typedef enum {
    MEMTYPE_RAM,
    MEMTYPE_PROGMEM
} memtype_t;

/**
 * Generic function to display data in a specified area on the LCD.
 * The displayed data can reside either in RAM or PROGMEM and is treated
 * like either one according to the given memtype paramter.
 *
 * @param data Graphic data to display
 * @param memtype Memory type to read the data from, RAM or PROGMEM
 * @param x Column start position on display (0 ... LCD_X_RES)
 * @param y Row start position on display (0 ... (LCD_Y_RES / 8))
 * @param w Data width, i.e. number of columns to display
 * @param h Data height, i.e. number of rows to display
 */
static void
lcd_set(const unsigned char *data, memtype_t memtype,
        uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t row, col;
    uint8_t cnt = 0;

    for (row = 0; row < h; row++) {
        spi_send_command(0x80 | x);
        spi_send_command(0x40 | (y + row));
        for (col = 0; col < w; col++) {
            if (memtype == MEMTYPE_PROGMEM) {
                spi_send_data(pgm_read_byte(&(data[cnt++])));
            } else {
                spi_send_data(data[cnt++]);
            }
        }
    }
}

/* shortcuts for lcd_set */
#define lcd_set_pgm(d, x, y, w, h) lcd_set(d, MEMTYPE_PROGMEM, x, y, w, h)
#define lcd_set_mem(d, x, y, w, h) lcd_set(d, MEMTYPE_RAM, x, y, w, h)


/**
 * Dislay the menu on the LCD.
 *
 * @param menu Menu graphic data to display
 */
void
lcd_set_menu(const unsigned char *menu)
{
    lcd_set_pgm(menu, MENU_X, MENU_Y, MENU_W, MENU_H);
}

/**
 * Display the chord key on the LCD.
 *
 * @param key Key graphic data to display
 */
static void
lcd_set_key(const unsigned char *key)
{
    lcd_set_pgm(key, KEY_X, KEY_Y, KEY_W, KEY_H);
}

/**
 * Display the chord modifier on the LCD.
 *
 * @param modifier Modifier graphic data to display
 */
static void
lcd_set_key_mod(const unsigned char *modifier)
{
    lcd_set_pgm(modifier, KEYMOD_X, KEYMOD_Y, KEYMOD_W, KEYMOD_H);
}

/**
 * Write the full chord area to the LCD internal memory.
 * This writes both, the chord key and the chord modifier.
 *
 * @param chords Array of two graphics for the chord key (index 0) and the
 *               chord modifier (index 1)
 */
void
lcd_set_chord(const unsigned char *chord[2])
{
    uint8_t i;

    lcd_set_key(chord[0]);
    lcd_set_key_mod(chord[1]);

    for (i = 0; i < 4; i++) {
        lcd_set_list_chord(i, 0);
    }
}

/**
 * Display the tempo on the LCD.
 * The tempo is displayed as three digits, each digit is written separetely.
 *
 * @param digits Array of three graphics, each containing the digit to display
 *               with index 0 holding the 100's value.
 */
void
lcd_set_tempo(const unsigned char *digits[3])
{
    lcd_set_pgm(digits[0], TEMPO_DIGIT1_X, TEMPO_Y, TEMPO_W, TEMPO_H);
    lcd_set_pgm(digits[1], TEMPO_DIGIT2_X, TEMPO_Y, TEMPO_W, TEMPO_H);
    lcd_set_pgm(digits[2], TEMPO_DIGIT3_X, TEMPO_Y, TEMPO_W, TEMPO_H);
}

/**
 * Display the playback mode on the LCD.
 *
 * @param mode Playback mode graphic data to display
 */
void
lcd_set_mode(const unsigned char *mode)
{
    lcd_set_pgm(mode, MODE_X, MODE_Y, MODE_W, MODE_H);
}


/*
 * Mapping chord names and modifiers for each key to font data.
 *
 * TODO not too happy with this solution, but didn't come up with any
 *      better idea yet, so this will do for now. This will either way
 *      require some rework when adding option to specify how to name
 *      the chords between A and C (since A#, Bb, B and B, H are all
 *      valid choices depending on taste and location). On top of that,
 *      it might be generally nice to have the choice to generally go
 *      sharp or flat - i.e. you might prefer Db over C# as notattion.
 *      I'll worry about improvements when that time has come then.
 *
 *      Note to future self: Sorry.
 */
static const uint8_t chord_list_map[PLAYBACK_KEY_MAX][4][2] PROGMEM = {
    { {MAJ_C, NONE }, {MAJ_G, NONE }, {MIN_A, NONE }, {MAJ_F, NONE } },
    { {MAJ_C, SHARP}, {MAJ_G, SHARP}, {MIN_B, FLAT }, {MAJ_F, SHARP} },
    { {MAJ_D, NONE }, {MAJ_A, NONE }, {MIN_B, NONE }, {MAJ_G, NONE } },
    { {MAJ_D, SHARP}, {MAJ_B, FLAT }, {MIN_C, NONE }, {MAJ_G, SHARP} },
    { {MAJ_E, NONE }, {MAJ_B, NONE }, {MIN_C, SHARP}, {MAJ_A, NONE } },
    { {MAJ_F, NONE }, {MAJ_C, NONE }, {MIN_D, NONE }, {MAJ_B, FLAT } },
    { {MAJ_F, SHARP}, {MAJ_C, SHARP}, {MIN_D, SHARP}, {MAJ_B, NONE } },
    { {MAJ_G, NONE }, {MAJ_D, NONE }, {MIN_E, NONE }, {MAJ_C, NONE } },
    { {MAJ_G, SHARP}, {MAJ_D, SHARP}, {MIN_F, NONE }, {MAJ_C, SHARP} },
    { {MAJ_A, NONE }, {MAJ_E, NONE }, {MIN_F, SHARP}, {MAJ_D, NONE } },
    { {MAJ_B, FLAT }, {MAJ_F, NONE }, {MIN_G, NONE }, {MAJ_D, SHARP} },
    { {MAJ_B, NONE }, {MAJ_F, SHARP}, {MIN_G, SHARP}, {MAJ_E, NONE } },
};

/* Chord list position offsets. */
static uint8_t chord_list_offsets[] = {
    LIST_CHORD_I_X,
    LIST_CHORD_V_X,
    LIST_CHORD_vi_X,
    LIST_CHORD_IV_X
};

/**
 * Display a single chord in the chord list area, highlighted or normal.
 * Chords are based on the currently selected key set through the menu.
 *
 * @param chord_num Chord number index of the currently selected key
 * @param highlighted Set chord display highlighted (1) or normal (0)
 */
void
lcd_set_list_chord(uint8_t chord_num, uint8_t highlighted)
{
    uint8_t i;
    uint8_t buf[LIST_CHORD_W];
    uint8_t val  = (highlighted) ? 0xff : 0x00;
    uint8_t key  = menu_get_current_playback_key();
    uint8_t name = pgm_read_byte(&(chord_list_map[key][chord_num][0]));
    uint8_t mod  = pgm_read_byte(&(chord_list_map[key][chord_num][1]));

    /* fill or clear the whole block, depending on highlighting */
    memset(buf, val, LIST_CHORD_W);

    if (highlighted) {
        /* add some rounded corners. Gotta have some rounded corners.. */
        buf[0] = 0x3c;
        buf[1] = 0x7e;
        buf[LIST_CHORD_W - 2] = 0x7e;
        buf[LIST_CHORD_W - 1] = 0x3c;
    }

    for (i = 0; i < FONT_CHORD_WIDTH; i++) {
        /*
         * set chord data itself, name and modifier if there's one.
         * Chord is always displayed centered, so positioning depends on
         * having a modifier. Yeah, those position offsets are random
         * magic numbers now, get over it.
         */
        if (mod) {
            buf[i + 3] = pgm_read_byte(&(font_chord_keys[name][i])) ^ val;
            buf[i + 9] = pgm_read_byte(&(font_chord_mods[mod][i]))  ^ val;
        } else {
            buf[i + 6] = pgm_read_byte(&(font_chord_keys[name][i])) ^ val;
        }
    }

    lcd_set_mem(buf, chord_list_offsets[chord_num],
            LIST_Y, LIST_CHORD_W, LIST_H);
}

/**
 * Display the playback metre.
 *
 * @param top Top part of the metre
 * @param bottom Bottom part of the metre
 *
 * TODO figure out how top and bottom parts are actually called..
 */
void
lcd_set_metre(uint8_t top, uint8_t bot)
{
    lcd_set_pgm(font_metre[top], METRE_X, METRE_TOP_Y,
            FONT_METRE_WIDTH, METRE_H);

    lcd_set_pgm(font_metre[bot], METRE_X, METRE_BOT_Y,
            FONT_METRE_WIDTH, METRE_H);
}


/* metronome "graphics", a big and small dot, plus one to clear the area */
static const uint8_t met_big[]   PROGMEM = {0x18, 0x3c, 0x7e, 0x7e, 0x3c, 0x18};
static const uint8_t met_small[] PROGMEM = {0x00, 0x18, 0x3c, 0x3c, 0x18, 0x00};
static const uint8_t met_clear[] PROGMEM = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/**
 * Display the metronome beat.
 *
 * Beat is based on the playback cycle, i.e 1/8th notes, and triggered
 * from there. If the playback mode has no cycle callback - such as the
 * simple chord playback mode, the metronome won't be displayed.
 *
 * Otherwise, if the beat is zero, a big dot is displayed in the top 
 * area to indicate the start of the next bar. Any other even numbered
 * beat will display a small dot in the bottom area. Even numbered beats
 * clear the area and make it all blink.
 *
 * @param beat Beat number based on the playback cycle count
 */
void
lcd_set_metronome(uint8_t beat)
{
    if (beat == 0) {
        lcd_set_pgm(met_big,   MET_X, MET_TOP_Y, MET_W, MET_H);
        lcd_set_pgm(met_clear, MET_X, MET_BOT_Y, MET_W, MET_H);

    } else if ((beat & 0x01) == 0) {
        lcd_set_pgm(met_clear, MET_X, MET_TOP_Y, MET_W, MET_H);
        lcd_set_pgm(met_small, MET_X, MET_BOT_Y, MET_W, MET_H);

    } else {
        lcd_set_pgm(met_clear, MET_X, MET_TOP_Y, MET_W, MET_H);
        lcd_set_pgm(met_clear, MET_X, MET_BOT_Y, MET_W, MET_H);
    }
}

