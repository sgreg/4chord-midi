/*
 * 4chord midi - main
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
 * ATmega88 pin layout (PDIP)
 *
 *   1  /Reset
 *   2  PD0     I   UART RXD
 *   3  PD1     O   UART TXD
 *   4  PD2     -   INT0 USB D+
 *   6  PD3     O   USB Reset
 *   5  PD4     -   USB D-
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
#include "usbdrv/usbdrv.h"

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
    uint8_t i;

    /* set PB0, PB1, PB2, PB3, PB4 as output, rest input */
    DDRB  = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5);
    /* set PB2 high, all other outputs low, enable pullups for all inputs */
    PORTB = ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB5));
    /* set port C all inputs */
    DDRC = 0x00;
    /* enable pullups for all inputs */
    PORTC = 0xff;
    /* set PB1 as output, rest input. */
    DDRD  = (1 << DDD1);
    /* set PB1 high, enable pullups for all inputs except V-USB ones */
    PORTD = ~((1 << PD1) | (1 << PD2) | (1 << PD3) | (1 << PD4));

    uart_init(UART_BRATE_38400_16MHZ);
    uart_print_pgm(uart_banner);

    lcd_init();
    gui_printlogo();

    /* set up V-USB, see also http://vusb.wikidot.com/driver-api */
    usbDeviceDisconnect();
    _delay_ms(300);
    usbDeviceConnect();
    usbInit();

    sei();

    /* delay to display GUI logo, but also poll USB or else USB fails */
    for (i = 0; i < 150; i++) {
        usbPoll();
        _delay_ms(10);
    }

    /* map buttons to inputs */
    button_map_port(BUTTON_MENU_PREV,   &PIND, 5);
    button_map_port(BUTTON_MENU_SELECT, &PIND, 6);
    button_map_port(BUTTON_MENU_NEXT,   &PIND, 7);
    button_map_port(BUTTON_CHORD_1,     &PINC, 0);
    button_map_port(BUTTON_CHORD_2,     &PINC, 1);
    button_map_port(BUTTON_CHORD_3,     &PINC, 2);
    button_map_port(BUTTON_CHORD_4,     &PINC, 3);
    
    lcd_clear();
    menu_init();

    while (1) {
        usbPoll();
        button_input_loop();
    }
}

