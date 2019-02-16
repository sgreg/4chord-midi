/*
 * 4chord MIDI - main
 *
 * Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
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
 * ATmega328 pin layout (TQFP32)
 *
 *   1  PD3     (unused)
 *   2  PD4     I   Button menu prev
 *   3  GND     -
 *   4  VCC     -
 *   5  GND     -
 *   6  VCC     -
 *   7  PB6     -   XTAL1
 *   8  PB7     -   XTAL2
 *
 *   9  PD5     O   LCD back light
 *  10  PD6     I   USB D-
 *  11  PD7     O   USB Reset
 *  12  PB0     O   LCD /Reset
 *  13  PB1     O   LCD D/C
 *  14  PB2     O   LCD /CS
 *  15  PB3     O   LCD MOSI / SerProg MOSI
 *  16  PB4     I   SerProg MISO
 *
 *  17  PB5     O   LCD SCK / SerProg SCK
 *  18  AVCC    -
 *  19  ADC6    (unused)
 *  20  AREF    -
 *  21  GND     -
 *  22  ADC7    (unused)
 *  23  PC0     I   Button Chord IV
 *  24  PC1     I   Button Chord vi
 *
 *  25  PC2     I   Button Chord V
 *  26  PC3     I   Button Chord I
 *  27  PC4     I   Button Menu Select
 *  28  PC5     I   Button Menu Next
 *  29  PC6     -   /Reset
 *  30  PD0     I   UART RXD
 *  31  PD1     O   UART TXD
 *  32  PD2     I   USB D+
 *
 */
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include "buttons.h"
#include "cli.h"
#include "eeprom.h"
#include "lcd.h"
#include "menu.h"
#include "xbmlib.h"
#include "intro.h"
#include "gui.h"
#include "playback.h"
#include "spi.h"
#include "timer.h"
#include "uart.h"
#include "usbdrv/usbdrv.h"

/** number of steps to increase the PWM timer value during the intro */
#define INTRO_PWM_STEP_INCREASE 4

/** number of PWM timer increase steps per single intro frame */
#define INTRO_PWM_STEPS_PER_FRAME 5


int
main(void) {
    /* half the frame count of the intro animation, start USB at that point */
    const uint8_t frame_count_usb_threshold = intro_frames_count >> 1;
    /* PWM timer value for fading LCD back light during intro animation */
    uint8_t pwm_value;
    /* intro animation frame count */
    uint8_t frame;
    /* a happy little counter */
    uint8_t i;

    /* set PB0, PB1, PB2, PB3, PB4 as output, rest input */
    DDRB  = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5);
    /* set PB2 high, all other outputs low, enable pullups for all inputs */
    PORTB = ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB5));
    /* set port C all inputs */
    DDRC = 0x00;
    /* enable pullups for all inputs */
    PORTC = 0xff;
    /* set PD1 and PD5 as output, rest input. */
    DDRD  = (1 << DDD1) | (1 << DDD5);
    /* set outputs high, enable pullups for all inputs except V-USB ones */
    PORTD = ~((1 << PD2) | (1 << PD5) | (1 << PD6) | (1 << PD7)) & 0xff;

    uart_init(UART_BRATE_38400_12MHZ);
    uart_clear_screen();

    spi_init();
    lcd_rst_low();

    /* this should give enough delay for the LCD to reset */
    eeprom_init();
    cli_print();

    lcd_rst_high();
    lcd_init();

    /* set up LCD backlight timer and show the first intro animation frame */
    timer0_init_pwm();

    sei();

    /*
     * disconnect USB for a while to get (re-)enumeration ongoing
     * see also http://vusb.wikidot.com/driver-api
     */
    usbDeviceDisconnect();

    /*
     * show the intro animation and fade the back light up
     *  - linear PWM duty cycle increase every 10ms
     *  - show next frame after every 50ms
     * after half the animation frames, connect and initialize USB
     * every animation frame after that, start polling to keep the connection
     */
    for (frame = 0, pwm_value = 0; frame < intro_frames_count; frame++) {
        for (i = 0; i < INTRO_PWM_STEPS_PER_FRAME; i++) {
            timer0_set_pwm(pwm_value);
            pwm_value += INTRO_PWM_STEP_INCREASE;
            _delay_ms(10);
        }

        lcd_write_frame(&intro_frames[frame]);

        if (frame == frame_count_usb_threshold) {
            usbDeviceConnect();
            usbInit();
        } else if (frame > frame_count_usb_threshold) {
            usbPoll();
        }
    }

    /* wait some more to show the logo and slowly get the back light all up */
    while (pwm_value) {
        timer0_set_pwm(pwm_value++);
        usbPoll();
        _delay_ms(50);
    }

    /* map buttons to inputs */
    button_map_port(BUTTON_MENU_PREV,   &PIND, 4);
    button_map_port(BUTTON_MENU_SELECT, &PINC, 5);
    button_map_port(BUTTON_MENU_NEXT,   &PINC, 4);
    button_map_port(BUTTON_CHORD_I,     &PINC, 3);
    button_map_port(BUTTON_CHORD_V,     &PINC, 2);
    button_map_port(BUTTON_CHORD_vi,    &PINC, 1);
    button_map_port(BUTTON_CHORD_IV,    &PINC, 0);

    /* clear the display and show the menu */
    lcd_clear();
    menu_init();

    /* let the magic begin */
    while (1) {
        usbPoll();
        button_input_loop();
        playback_poll();
        menu_poll();
        cli_poll();
    }
}

