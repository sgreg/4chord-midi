/*
 * 4chord midi - UART communication
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
 */
#ifndef _UART_H_
#define _UART_H_
#include <stdint.h>

/* baud rate values for U2Xn=0 */
#define UART_BRATE_2400_8MHZ    207
#define UART_BRATE_9600_8MHZ     51
#define UART_BRATE_9600_16MHZ   103
#define UART_BRATE_19200_8MHZ    25
#define UART_BRATE_19200_16MHZ   51
#define UART_BRATE_38400_8MHZ    12
#define UART_BRATE_38400_16MHZ   25

/**
 * Initialize UART with given baud rate value.
 * See list of UART_BRATE_* defines for some predefined baud rate values.
 *
 * @param brate UART baud rate
 */
void uart_init(int16_t brate);

/**
 * Transmit a single character via UART.
 * @param data Character to write
 */
void uart_putchar(char data);

/**
 * Print a newline via UART.
 */
void uart_newline(void);

/**
 * Print a given string via UART.
 * @param data String to print
 */
void uart_print(char *data);

/**
 * Print a given string residing in program space via UART.
 * @param data PROGMEM string to print
 */
void uart_print_pgm(const char *data);

/**
 * Print a given byte as hexadecimal value via UART.
 * Note, only single bytes are printed, also no 0x suffix is output
 * automatically, so add that if required manually.
 *
 * @param data Value to print as hexadecimal.
 */
void uart_puthex(char data);

/**
 * Print a given signed base 10 number via UART.
 * A number of minumum digits can be specified. If the given number has
 * less digits, the output is filled with leading zeros. If the number
 * has more digits, all digits are printed.
*
 * @param number Number to be printed.
 * @param digits Minimum number of digits to print.
 */
void uart_putint(int32_t number, int8_t digits);

/**
 * Read a single character from UART registers.
 * Note, this is a blocking a operation. The receive interrupt handler is
 * using this function, you probably shouldn't. Try uart_get_inbuf() instead.
 *
 * @return Received character
 */
char uart_getchar(void);

/**
 * Get the internal buffer for received characters.
 * The receive interrupt handler is filling the buffer.
 * If no new data was received, 0 is returned.
 *
 * @return Last received character, 0 if no data was received.
 */
char uart_get_inbuf(void);

/**
 * Reset the internal receive buffer.
 */
void uart_reset_inbuf(void);

#endif
