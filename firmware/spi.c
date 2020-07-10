/*
 * 4chord MIDI - SPI communication for Nokia 5110 LCD
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
 */
#include <stdint.h>
#include <avr/io.h>
#include "config.h"

/* set SPI chip select pin high */
#define spi_cs_high()   do { SPI_CS_PORT |=  (1 << SPI_CS_PIN); } while (0)
/* set SPI chip select pin low */
#define spi_cs_low()    do { SPI_CS_PORT &= ~(1 << SPI_CS_PIN); } while (0)
/* set SPI data/command pin high */
#define spi_dc_high()   do { SPI_DC_PORT |=  (1 << SPI_DC_PIN); } while (0)
/* set SPI data/command pin low */
#define spi_dc_low()    do { SPI_DC_PORT &= ~(1 << SPI_DC_PIN); } while (0)

/**
 * Initialize SPI.
 *
 * SPI is set up as controller, Mode 0, F_CPU/4 clock speed, MSB first
 */
void
spi_init(void)
{
    /* Enable as controller */
    SPCR  = (1 << SPE) | (1 << MSTR);
    /* Mode 0 */
    SPCR |= (0 << CPOL) | (0 << CPHA);
    /* Clock F_CPU/4 */
    SPCR |= (0 << SPR0) | (0 << SPR1);
    /* Data direction MSB first */
    SPCR |= (0 << DORD);
}

/**
 * Send SPI command byte to LCD.
 * @param command Command to send via SPI
 */
void
spi_send_command(uint8_t command) {
    spi_cs_low();
    spi_dc_low();

    SPDR = command;
    while (!(SPSR & (1 << SPIF))) {
        /* wait */
    }

    spi_cs_high();
}

/**
 * Send SPI data byte to LCD.
 * @param data Data to send via SPI
 */
void
spi_send_data(uint8_t data) {
    spi_cs_low();
    spi_dc_high();

    SPDR = data;
    while (!(SPSR & (1 << SPIF))) {
        /* wait */
    }

    spi_cs_high();
}

