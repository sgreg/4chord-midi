/*
 * 4chord midi - board configuration settings
 *
 * Copyright (C) 2015 Sven Gregori <svengregori@gmail.com>
 *
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <avr/io.h>

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
