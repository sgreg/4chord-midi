/*
 * 4chord MIDI bootloader - Main
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
#include <string.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#ifndef DEBUG
#include "lcd.h"
#include "gfx.h"
#include "spi.h"
#endif
#include "uart.h"
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#define VERSION "1.0"
uint8_t banner[] = "4chord MIDI bootloader " VERSION;

void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void program(void);

static uint16_t recv_len;
static uint16_t recv_cnt;
static uint8_t recv_all;

typedef struct {
    uint8_t page;
    uint8_t size;
    uint8_t data[SPM_PAGESIZE];
} recv_chunk_t;

uint8_t number_of_pages;

static recv_chunk_t recv_data;
static uint8_t *recv_ptr;

static uint8_t repl_len;
static uint8_t repl_cnt;

#define EEPROM_BUF_MAX 16
static uint8_t eeprom_buf[EEPROM_BUF_MAX];

#define CMD_HELLO               0x01
#define CMD_FWUPDATE_INIT       0x10
#define CMD_FWUPDATE_MEMPAGE    0x11
#define CMD_FWUPDATE_VERIFY     0x12
#define CMD_FWUPDATE_FINALIZE   0x13
#define CMD_EEPROM_READ         0x20
#define CMD_EEPROM_WRITE        0x30
#define CMD_BYE                 0xf0
#define CMD_RESET               0xfa

#define ST_IDLE     0
#define ST_HELLO    1
#define ST_FWUPDATE 2
#define ST_RESET    3
uint8_t state = ST_IDLE;

#define HELLO_VALUE 0x4d6f
#define HELLO_INDEX 0x6921

#ifndef DEBUG
#define PROGRESS_BAR_LEN        LCD_X_RES
#define PROGRESS_BAR_ROW_INDEX  5
void clear_progress_bar(void);
#endif

/**
 * V-USB setup callback function.
 */
uchar
usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *) data;

    switch (rq->bRequest) {
        case CMD_HELLO:
            if (state == ST_IDLE &&
                    rq->wValue.word == HELLO_VALUE &&
                    rq->wIndex.word == HELLO_INDEX)
            {
                state = ST_HELLO;
#ifdef DEBUG
                uart_print("HELLO\r\n");
#else
                clear_progress_bar();
#endif
                usbMsgPtr = banner;
                return sizeof(banner);
            }
            break;

        case CMD_FWUPDATE_INIT:
            if (state == ST_HELLO) {
                state = ST_FWUPDATE;
                number_of_pages = rq->wValue.word;
#ifdef DEBUG
                uart_print("INIT: ");
                uart_putint(number_of_pages, 1);
                uart_print(" pages\r\n");
#endif
            }
            break;

        case CMD_FWUPDATE_MEMPAGE:
            if (state == ST_FWUPDATE) {
                recv_cnt = 0;
                recv_len = rq->wLength.word;
                return USB_NO_MSG;
            }
            break;

        case CMD_FWUPDATE_VERIFY:
            if (state == ST_FWUPDATE) {
                boot_rww_enable();
                repl_len = rq->wLength.word;
                repl_cnt = 0;
#ifdef DEBUG
                uart_print("VERIFY: page ");
                uart_putint(recv_data.page, 1);
                uart_print(" len ");
                uart_putint(repl_len, 1);
                uart_newline();
#endif

                return USB_NO_MSG;
            }
            break;

        case CMD_FWUPDATE_FINALIZE:
            if (state == ST_FWUPDATE) {
#ifdef DEBUG
                uart_print("FINALIZE\r\n");
#endif
                boot_rww_enable();
                state = ST_HELLO;
            }
            break;

        case CMD_EEPROM_READ:
            if (state == ST_HELLO) {
                uint8_t len = (rq->wIndex.word > EEPROM_BUF_MAX) ? EEPROM_BUF_MAX : rq->wIndex.bytes[0];
                eeprom_read_block(eeprom_buf, (uint8_t *) rq->wValue.word, len);
#ifdef DEBUG
                uart_print("EEPROM addr ");
                uart_puthex(rq->wValue.bytes[0]);
                uart_print(": ");
                uint8_t i;
                for (i = 0; i < len; i++) {
                    uart_puthex(eeprom_buf[i]);
                }
                uart_newline();
#endif
                usbMsgPtr = eeprom_buf;
                return len;
            }
            break;

        case CMD_EEPROM_WRITE:
            if (state == ST_HELLO) {
#ifdef DEBUG
                uart_print("EEPROM write addr ");
                uart_puthex(rq->wValue.bytes[0]);
                uart_print(": ");
                uart_puthex(rq->wIndex.bytes[0]);
                uart_newline();
#endif
                eeprom_update_byte((uint8_t *) rq->wValue.word, rq->wIndex.bytes[0]);
            }
            break;

        case CMD_BYE:
#ifdef DEBUG
            uart_print("BYE\r\n");
#endif
            state = ST_IDLE;
            break;

        case CMD_RESET:
            if (state == ST_IDLE) {
#ifdef DEBUG
                uart_print("\r\nRESET\r\n");
#endif
                state = ST_RESET;
            }
            break;
    }
    return 0;
}

/**
 * V-USB read callback function
 */
uchar
usbFunctionRead(uchar *data, uchar len)
{
    uint8_t i;
    uint16_t address = ((recv_data.page - 1) << 7) + repl_cnt;

    if (len > repl_len - repl_cnt) {
        len = repl_len - repl_cnt;
    }
    repl_cnt += len;
#ifdef DEBUG
    uart_print("read ");
#endif
    for (i = 0; i < len; i++) {
        *data = pgm_read_byte((void *) address++);
#ifdef DEBUG
        uart_puthex(*data);
#endif
        data++;
    }
#ifdef DEBUG
    uart_newline();
#endif

    return len;
}

/**
 * V-USB write callback function
 */
uchar
usbFunctionWrite(uchar *data, uchar len)
{
    uint8_t i;

    recv_ptr = (uint8_t *) &recv_data;

    for (i = 0; recv_cnt < recv_len && i < len; i++, recv_cnt++) {
        recv_ptr[recv_cnt] = data[i];
    }

    if (recv_cnt == recv_len) {
        recv_all = 1;
        program();
    }

    return (recv_cnt == recv_len);
}

void program(void)
{
    uint16_t address = (recv_data.page - 1) << 7;
    uint8_t i;
    uint8_t sreg;
    uint8_t *buf = (uint8_t *) &recv_data.data;

    sreg = SREG;
    boot_page_erase(address);
    boot_spm_busy_wait();
    for (i = 0; i < recv_data.size; i += 2) {
        uint16_t word = *buf++;
        word += (*buf++) << 8;
        boot_page_fill(address + i, word);
    }
    boot_page_write(address);
    boot_spm_busy_wait();

    SREG = sreg;

#ifndef DEBUG
    uint16_t tmp = recv_data.page * PROGRESS_BAR_LEN;
    uint8_t progress = (int) (tmp / number_of_pages);
    spi_send_command(0x80 | progress);
    spi_send_command(0x40 | PROGRESS_BAR_ROW_INDEX);
    spi_send_data(0xff);
#endif
}

#ifndef DEBUG
void clear_progress_bar(void)
{
    uint8_t i;

    for (i = 0; i < PROGRESS_BAR_LEN; i++) {
        spi_send_command(0x80 | i);
        spi_send_command(0x40 | PROGRESS_BAR_ROW_INDEX);
        spi_send_data(0x00);
    }
}
#endif

void wdt_init(void)
{
    MCUSR=0;
    wdt_disable();
}

int
main(void)
{
#ifdef DEBUG
    uint8_t i;
#endif
    uint8_t shutdown_counter = 0;

    /* set PB0, PB1, PB2, PB3, PB4 as output, rest input */
    DDRB  = (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3) | (1 << DDB5);
    /* set PB2 high, all other outputs low, enable pullups for all inputs */
    PORTB = ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB5));
    /* set port C all inputs */
    DDRC = 0x00;
    /* enable pullups for all inputs */
    PORTC = 0xff;
    /* set PD1 and PD5 as output, rest input. */
    DDRD  = (1 << DDD1) | (1 << DDD5);
    /* set outputs low, enable pullups for all inputs except V-USB ones */
    PORTD = ~((1 << PD1) | (1 << PD2) | (1 << PD5) | (1 << PD6) | (1 << PD7)) & 0xff;

    /* Shift interrupt vector to bootloader space */
    MCUCR = (1 << IVCE);
    MCUCR = (1 << IVSEL);

    uart_init(UART_BRATE_38400_12MHZ);
    uart_putchar('\f');
    uart_print((char *) banner);
    uart_newline();
    uart_print("Pin state: ");
    uart_putchar(((PINC & (1 << 5)) == 0) ? '1' : '0');
    uart_newline();
    
    /* Check input port if "Select" is pressed */
    if ((PINC & (1 << PC5)) != 0) {
        /* Nope. Put interrupt vector back in order... */
        MCUCR = (1 << IVCE);
        MCUCR = 0;
        /* ...and jump to application */
        asm("jmp 0000");
    }

    /* Turn on LCD backlight */
    PORTD |= (1 << PD5);

    uart_print("Welcome\r\n");

#ifndef DEBUG
    lcd_init();
    lcd_fullscreen(gfx_splash);
#endif

    usbDeviceDisconnect();
    _delay_ms(300);
    usbDeviceConnect();
    usbInit();

    sei();
    while (1) {
        usbPoll();
        if (state == ST_FWUPDATE) {
            if (recv_all) {
#ifdef DEBUG
                uart_print("page ");
                uart_putint(recv_data.page, 2);
                uart_print(" addr ");
                uart_putint((recv_data.page - 1) << 7, 5);
                uart_print(" with ");
                uart_putint(recv_data.size, 3);
                uart_print(" bytes: ");
                
                for (i = 0; i < SPM_PAGESIZE && i < recv_data.size; i++) {
                    if ((i & 0xf) == 0) {
                        uart_newline();
                    }
                    uart_puthex(recv_data.data[i]);
                    uart_putchar(' ');
                }
                uart_newline();
#endif
                recv_all = 0;
            }

        } else if (state == ST_RESET) {
            /*
             * Reset initiated, delay it for a few loop cycles to make sure
             * that USB communication properly finishes before disconeccting
             * so the other side won't get any pipe errors or the like.
             */
            if (++shutdown_counter == 10) {
                break;
            }
        } else if ((PINC & ((1 << 0) | (1 << 3))) == 0) {
            break;
        }
    }

    usbDeviceDisconnect();

    cli();
    MCUCR = (1 << IVCE);
    MCUCR = 0;
    wdt_enable(WDTO_60MS);
    while (1);
}

