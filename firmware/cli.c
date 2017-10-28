/*
 * 4chord MIDI - UART command line interface
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
#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "menu.h"
#include "playback.h"
#include "uart.h"

/* 4chord MIDI banner */
static const char cli_banner[] PROGMEM =
"\f\r\n\
         ___      _                   _ \r\n\
        /   |    | |                 | |       _________   _   ______   _\r\n\
       / /| | ___| |__   ___  _ __ __| |      |  _   _  \\ | | |____  \\ | |\r\n\
      / /_| |/ __| '_ \\ / _ \\| '__/ _` |      | | | | | | | |  _   | | | |\r\n\
      \\___  | (__| | | | (_) | | | (_| |      | | | | | | | | | |__| | | |\r\n\
          |_/\\___|_| |_|\\___/|_|  \\__,_|      |_| |_| |_| |_| |______/ |_|\r\n\
\r\n";

/* Command help text */
static const char cli_help[] PROGMEM =
"\r\n\
    [1]     Play chord I\r\n\
    [2]     Play chord V\r\n\
    [3]     Play chord vi\r\n\
    [4]     Play chord IV\r\n\
  [Space]   Stop playback\r\n\
    [<]     Menu previous\r\n\
  [Enter]   Menu Select\r\n\
    [>]     Menu next\r\n\
    [h]     Print this help\r\n\
    [?]     About 4chord MIDI\r\n\
";

/* 4chord MIDI about text */
static const char cli_about[] PROGMEM =
"\r\n\r\n\
4chord MIDI - The Four Chord USB MIDI Keyboard\r\n\
http://4chord.craplab.fi\r\n\r\n\
Copyright (C) 2017 Sven Gregori <sven@craplab.fi>\r\n\r\n\
Firmware version:  " FOURCHORD_MIDI_VERSION " built " BUILD_DATE_STRING "\r\n\
Hardware revision: B (08-2017)\r\n\r\n\
Firmware is licensed under the GNU General Public License version 2\r\n\
Hardware is licensed under the CERN Open Hardware License version 1.2\r\n\
USB connectivity implemented using Object Development's V-USB library\r\n\r\n\
";

#define NO_BUTTON 0xff
/* last pressed button for playback callback handler parameter */
static uint8_t button = NO_BUTTON;

/* command data read from UART */
static char cmd;
/* last command data read from UART */
static char last;


static void
playback_handle(uint8_t new_button)
{
    if (button != NO_BUTTON) {
        playback_button_release(&button);
    }

    button = new_button;

    if (button != NO_BUTTON) {
        playback_button_press(&button);
    }
}


/**
 * Print the command line interface to UART
 */
void
cli_print(void)
{
    uart_print_pgm(cli_banner);
    uart_print_pgm(cli_help);
}

/**
 * Poll command line interface for input from UART and handle it.
 */
void
cli_poll(void)
{
    cmd = uart_get_inbuf();

    if (cmd != last) {
        last = cmd;
        switch (cmd) {
            case '1':
            case '2':
            case '3':
            case '4':
                playback_handle(cmd - '1');
                break;
            case ' ':
                playback_handle(NO_BUTTON);
                break;
            case '-':
            case '<':
            case 'a':
                menu_button_prev();
                break;
            case '\r':
            case 's':
                menu_button_select();
                break;
            case '+':
            case '>':
            case 'd':
                menu_button_next();
                break;
            case 'h':
                uart_print_pgm(cli_help);
                break;
            case '?':
                uart_print_pgm(cli_about);
                break;
        }
        uart_reset_inbuf();
    }
}

