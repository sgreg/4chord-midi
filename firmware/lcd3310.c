/*
 * 4chord midi - Nokia 3310 LCD handling
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 *
 * The Nokia LCD3310 LCD is a 48x84px display that is arranged in 6 rows of
 * 8x84px each. Each row is therefore 84 byte wide with each byte representing
 * the 8 pixel height of the row.
 * 
 * For more information on the Nokia 3310 LCD, check Olimex github
 * https://github.com/OLIMEX/UEXT-MODULES/tree/master/MOD-LCD3310
 *
 *
 * The 4chord midi user interface is arranged in 5 areas:
 *      1. Menu (playback mode, key, tempo)
 *      2. Chord key (C, D, E, F, G, A, B)
 *      3. Chord modifier (none, sharp, flat)
 *      4. Playback tempo
 *      5. Playback mode
 *
 * The areas are arranged on the display as following:
 * +------------------------------------------+
 * |111111111111111111111111111111111111111111|
 * |                                          |
 * |2222222222222222 33333333 4444444444444444|
 * |2222222222222222 33333333 4444444444444444|
 * |2222222222222222          5555555555555555|
 * |2222222222222222          5555555555555555|
 * +------------------------------------------+
 * 
 * One character represents 8 byte height (i.e. one row) and 2 byte width.
 *
 * Areas x/y and memory arrangements:
 *
 * 1 Menu       y0, x0-83, 1*84 bytes
 *              mem: 0-83
 * 2 Key        y2-5, x0-31, 4*32 bytes
 *              mem: 168-199, 252-283, 336-367, 420-451
 * 3 Key mod    y2-3 x34-49, 2*16 bytes
 *              mem: 202-217, 286-301
 * 4 Tempo      y2-3 x52-83, 2*32 bytes
 *              mem: 220-251, 304-335
 * 5 Mode       y4-5 x52-83, 2*32 bytes
 *              mem: 388-419, 472-503
 */
#include <string.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "config.h"
#include "spi.h"

/* set LCD reset pin high */
#define lcd_rst_high()  do { LCD_RESET_PORT |=  (1 << LCD_RESET_PIN); } while (0)
/* set LCD reset pin low */
#define lcd_rst_low()   do { LCD_RESET_PORT &= ~(1 << LCD_RESET_PIN); } while (0)

#define LCD_START_LINE_ADDR (66-2)
#define LCD_X_RES           84
#define LCD_Y_RES           48

#define LCD_CACHE_SIZE      ((LCD_X_RES * LCD_Y_RES) / 8)
/* internal representation of LCD memory */
unsigned char lcd_memory[LCD_CACHE_SIZE];

/* memory arrangement for the menu area */
#define MEM_MENU_START  0
#define MEM_MENU_COLS  84
#define MEM_MENU_ROWS   1

/* memory arrangement for the chord key area */
#define MEM_KEY_START 168
#define MEM_KEY_COLS   32
#define MEM_KEY_ROWS    4

/* memory arrangement for the chord modifier area */
#define MEM_KEYMOD_START 202
#define MEM_KEYMOD_COLS   16
#define MEM_KEYMOD_ROWS    2

/* memory arrangement for the tempo area */
#define MEM_TEMPO_DIGIT1_START 220
#define MEM_TEMPO_DIGIT2_START 230
#define MEM_TEMPO_DIGIT3_START 240
#define MEM_TEMPO_COLS 8
#define MEM_TEMPO_ROWS 2

/* memory arrangement for the mode area */
#define MEM_MODE_START 388
#define MEM_MODE_COLS   32
#define MEM_MODE_ROWS    2


/**
 * Generic function to write an area inside the internal memory.
 *
 * @param data Graphic data to write
 * @param mem_start Memory starting address
 * @param cols Number of colums to write
 * @param rows Number of rows to write
 */
static void
lcd_set_mem(const unsigned char *data, uint16_t mem_start, uint8_t cols, uint8_t rows)
{
    int i;
    int mem_idx = mem_start;
    int data_idx = 0;

    for (i = 0; i < rows; i++, mem_idx += LCD_X_RES, data_idx += cols) {
        memcpy_P(lcd_memory + mem_idx, data + data_idx, cols);
    }
}

/**
 * Hardware reset the display
 */
static void
lcd_reset(void)
{
    lcd_rst_low();
    _delay_ms(1000);
    lcd_rst_high();
}

/**
 * Initialize the LCD.
 */
void
lcd_init(void)
{
    spi_init();
    lcd_reset();

    /* taken from Olimex Arduino example */
    spi_send_command(0x21);
    spi_send_command(0xc8);
    spi_send_command(0x04 | !!(LCD_START_LINE_ADDR & (1u << 6)));
    spi_send_command(0x40 | (LCD_START_LINE_ADDR & ((1u << 6) -1)));
    spi_send_command(0x14);
    spi_send_command(0x20);
    spi_send_command(0x08);
    spi_send_command(0x0c);
}

/**
 * Update the LCD content.
 * Transfers the internal memory content via SPI to the LCD
 */
void
lcd_update(void)
{
    uint8_t x, y;

    for (y = 0; y < LCD_Y_RES / 8; y++)
    {
        spi_send_command(0x80);
        spi_send_command(0x40 | y);
        for (x = 0; x < LCD_X_RES; x++)
        {
            spi_send_data(lcd_memory[y * LCD_X_RES + x]);
        }
    }
}

/**
 * Clear the LCD internal memory.
 * To actually clear the display, call lcd_update() afterwards.
 */
void
lcd_clear(void)
{
    memset(&lcd_memory, 0, LCD_CACHE_SIZE);
}

/**
 * Write a fullscreen image to the LCD internal memory.
 * Call lcd_update() to display the image on the LCD itself.
 */
void
lcd_fullscreen(const unsigned char *data)
{
    memcpy_P(lcd_memory, data, LCD_CACHE_SIZE);
    lcd_update();
}

/**
 * Write the menu area to the LCD internal memory.
 * @param menu Menu graphics to be written to internal memory
 */
void
lcd_set_menu(const unsigned char *menu)
{
    lcd_set_mem(menu, MEM_MENU_START, MEM_MENU_COLS, MEM_MENU_ROWS);
}

/**
 * Write the chord key area to the LCD internal memory.
 * @param key Key graphic to be writte to internal memory
 */
static void
lcd_set_key(const unsigned char *key)
{
    lcd_set_mem(key, MEM_KEY_START, MEM_KEY_COLS, MEM_KEY_ROWS);
}

/**
 * Write the chord modifier area to the LCD internal memory.
 * @param modifier Modifier graphics to be written to internal memory
 */
static void
lcd_set_key_mod(const unsigned char *modifier)
{
    lcd_set_mem(modifier, MEM_KEYMOD_START, MEM_KEYMOD_COLS, MEM_KEYMOD_ROWS);
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
    lcd_set_key(chord[0]);
    lcd_set_key_mod(chord[1]);
}

/**
 * Write the tempo area to the LCD internal memory.
 * The tempo is displayed as three digits, each digit is written separetely.
 *
 * @param digits Array of three graphics, each containing the digit to display
 *               with index 0 holding the 100's value.
 */
void
lcd_set_tempo(const unsigned char *digits[3])
{
    lcd_set_mem(digits[0], MEM_TEMPO_DIGIT1_START, MEM_TEMPO_COLS, MEM_TEMPO_ROWS);
    lcd_set_mem(digits[1], MEM_TEMPO_DIGIT2_START, MEM_TEMPO_COLS, MEM_TEMPO_ROWS);
    lcd_set_mem(digits[2], MEM_TEMPO_DIGIT3_START, MEM_TEMPO_COLS, MEM_TEMPO_ROWS);
}

/**
 * Write the mode area to the LCD internal memory.
 * @param mode Mode graphics to be written to internal memory
 */
void
lcd_set_mode(const unsigned char *mode)
{
    lcd_set_mem(mode, MEM_MODE_START, MEM_MODE_COLS, MEM_MODE_ROWS);
}

