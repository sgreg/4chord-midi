/*
 * 4chord midi - SPI master communication for Nokia 3310 LCD
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
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
