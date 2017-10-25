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
#include <stdint.h>
#include <avr/io.h>

/* SPI chip select port name */
#define SPI_CS_PORT PORTB
/* SPI chip select pin number */
#define SPI_CS_PIN  PB2

/* SPI data/command port name */
#define SPI_DC_PORT PORTB
/* SPI data/command pin number */
#define SPI_DC_PIN  PB1


/* set SPI chip select pin high */
#define spi_cs_high()   do { SPI_CS_PORT |=  (1 << SPI_CS_PIN); } while (0)
/* set SPI chip select pin low */
#define spi_cs_low()    do { SPI_CS_PORT &= ~(1 << SPI_CS_PIN); } while (0)
/* set SPI data/command pin high */
#define spi_dc_high()   do { SPI_DC_PORT |=  (1 << SPI_DC_PIN); } while (0)
/* set SPI data/command pin low */
#define spi_dc_low()    do { SPI_DC_PORT &= ~(1 << SPI_DC_PIN); } while (0)

/**
 * Initialize SPI master.
 */
void
spi_init(void)
{
    /* master, SPI mode 0, MSB first */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0) | (0 << DORD);
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

