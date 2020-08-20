/*
 * 4chord MIDI - Timer handling
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
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

/* callback function to call on timer interrupt */
static timer_callback_t timer1_callback;


/**
 * Setup 8bit timer to Fast PWM mode to control LCD back light.
 */
void
timer0_init_pwm(void)
{
    /* Fast PWM mode 3, PD5 = 1 on BOTTOM, 0 on OCR0B match */
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    /* prescaler 64 */
    TCCR0B = (1 << CS01) | (1 << CS00);
    /* Zero output compare, i.e. lights off */
    OCR0B  = 0;
}


/**
 * Start 16bit timer in given interval, executing given callback function
 * on compare match.
 * Note, the prescaler is set to 1024.
 *
 * @param value Timer count value
 * @param callback Callback function executed when timer compare interrupt hits
 */
void
timer1_start(uint16_t value, timer_callback_t callback)
{
    timer1_callback = callback;
    TCNT1  = 0x00; /* reset to zero */
    OCR1A  = value; /* set compare match value */
    TIMSK1 = (1 << OCIE1A); /* enable compare match interrupt */
    TCCR1A = 0x00; /* no output ports, no PWM mode - value never change */
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); /* CTC mode, prescaler 1024 */
    TCCR1C = 0x00;
}

/**
 * Stop 16bit timer.
 */
void
timer1_stop(void)
{
    TCCR1B = 0x00; /* no clock source */
    timer1_callback = NULL;
}

/**
 * Timer intterupt handler.
 * Call timer1_callback is it is set.
 */
SIGNAL(TIMER1_COMPA_vect)
{
    if (timer1_callback != NULL) {
        timer1_callback();
    }
}

