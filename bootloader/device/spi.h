/*
 * 4chord MIDI bootloader - SPI master communication for Nokia LCD
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
#ifndef _SPI_H_
#define _SPI_H_
#include <stdint.h>

/**
 * Initialize SPI master.
 */
void spi_init(void);

/**
 * Send SPI command.
 * @param command Command to send via SPI
 */
void spi_send_command(uint8_t command);

/**
 * Send SPI data.
 * @param data Data to send via SPI
 */
void spi_send_data(uint8_t data);

#endif
