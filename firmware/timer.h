/*
 * 4chord MIDI - Timer handling
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
#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdint.h>

/* number of clock cycles in one timer cycle with prescaler 1024 */
#define TICKS_PER_CYCLE (F_CPU >> 10)

/* timer interrupt handler callback function */
typedef void (*timer_callback_t)(void);


/**
 * Setup 8bit timer to Fast PWM mode to control LCD back light.
 */
void timer0_init_pwm(void);

/**
 * Set PWM value, i.e. timer0 output compare register.
 *
 * Back light is
 *  - on  for the counting duration of 0 .. val
 *  - off for the counting duration of val .. 255
 * or in other worlds, the higher val, the brighter the back light.
 *
 * @param val Output compare value
 */
#define timer0_set_pwm(val) do { OCR0B = val; } while (0)

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
