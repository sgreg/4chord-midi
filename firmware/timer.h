/*
 * 4chord midi - Timer handling for progressive playback
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
#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdint.h>

/* number of clock cycles in one timer cycle with prescaler 1024 */
#define TICKS_PER_CYCLE (F_CPU >> 10)

/* timer interrupt handler callback function */
typedef void (*timer_callback_t)(void);


/**
 * Start 16bit timer in given interval, executing given callback function
 * on compare match.
 * Note, the prescaler is set to 1024.
 *
 * @param value Timer count value
 * @param callback Callback function executed when timer compare interrupt hits
 */
void timer1_start(uint16_t value, timer_callback_t callback);

/**
 * Stop 16bit timer.
 */
void timer1_stop(void);

#endif
