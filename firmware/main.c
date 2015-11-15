/*
 * 4chord midi - main
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 *
 * ATmega88 pin layout (PDIP)
 *
 *   1  /Reset
 *   2  PD0     I   UART RXD
 *   3  PD1     O   UART TXD
 *   4  PD2     (unused)
 *   5  PD3     (unused)
 *   6  PD4     (unused)
 *   7  VCC     -
 *   8  GND     -
 *   9  PB6     -   (XTAL1)
 *  10  PB7     -   (XTAL2)
 *  11  PD5     I   Button -
 *  12  PD6     I   Button Select
 *  13  PD7     I   Button + 
 *  14  PB0     O   LCD Reset
 *
 *  15  PB1     O   LCD D/C
 *  16  PB2     O   LCD /CS
 *  17  PB3     O   LCD MOSI / SerProg MOSI
 *  18  PB4     I   SerProg MISO
 *  19  PB5     O   LCD SCK / SerProg SCK
 *  20  AVCC    -
 *  21  AREF    -
 *  22  GND     -
 *  23  PC0     I   Button Chord I
 *  24  PC1     I   Button Chord V
 *  25  PC2     I   Button Chord vi
 *  26  PC3     I   Button Chord IV
 *  27  PC4     (unused)
 *  28  PC5     (unused)
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "lcd3310.h"
#include "buttons.h"
#include "gui.h"
#include "uart.h"

const char uart_banner[] PROGMEM =
"\f\r\n\
         ___      _                   _ \r\n\
        /   |    | |                 | |       _________   _   ______   _\r\n\
       / /| | ___| |__   ___  _ __ __| |      |  _   _  \\ | | |____  \\ | |\r\n\
      / /_| |/ __| '_ \\ / _ \\| '__/ _` |      | | | | | | | |  _   | | | |\r\n\
      \\___  | (__| | | | (_) | | | (_| |      | | | | | | | | | |__| | | |\r\n\
          |_/\\___|_| |_|\\___/|_|  \\__,_|      |_| |_| |_| |_| |______/ |_|\r\n\
\r\n";

int
main(void) {
    /* set PB0, PB1, PB2, PB3, PB4 as output, rest input */
    DDRB  = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5);
    /* set PB2 high, all other outputs low, enable pullups for all inputs */
    PORTB = ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB5));
    /* set port C all inputs */
    DDRC = 0x00;
    /* enable pullups for all inputs */
    PORTC = 0xff;
    /* set PB1 as output, rest input */
    DDRD  = (1 << DDD1);
    /* set PB1 high, enable pullups for all inputs */
    PORTD = ~(1 << PD1);

    uart_init(UART_BRATE_38400_8MHZ);
    lcd_init();

    uart_print_pgm(uart_banner);
    gui_printlogo();
    _delay_ms(2000);

    /* map buttons to inputs */
    button_map_port(BUTTON_MENU_PREV,   &PIND, 5);
    button_map_port(BUTTON_MENU_SELECT, &PIND, 6);
    button_map_port(BUTTON_MENU_NEXT,   &PIND, 7);
    button_map_port(BUTTON_CHORD_1,     &PINC, 0);
    button_map_port(BUTTON_CHORD_2,     &PINC, 1);
    button_map_port(BUTTON_CHORD_3,     &PINC, 2);
    button_map_port(BUTTON_CHORD_4,     &PINC, 3);
    
    gui_start();

    sei();

    while (1) {
        button_input_loop();
        _delay_ms(10);
    }
}

