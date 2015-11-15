/*
 * 4chord midi - SPI master communication for Nokia 3310 LCD
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
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


