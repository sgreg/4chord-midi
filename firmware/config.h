/*
 * 4chord midi - board configuration settings
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
#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <avr/io.h>

#define FOURCHORD_MIDI_VERSION "1.0"

/* SPI chip select port name */
#define SPI_CS_PORT PORTB
/* SPI chip select pin number */
#define SPI_CS_PIN  PB2

/* SPI data/command port name */
#define SPI_DC_PORT PORTB
/* SPI data/command pin number */
#define SPI_DC_PIN  PB1

/* LCD reset port name */
#define LCD_RESET_PORT  PORTB
/* LCD reset pin number */
#define LCD_RESET_PIN   PB0

#endif
