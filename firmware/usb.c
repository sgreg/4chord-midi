/*
 * 4chord midi - USB MIDI
 *
 * Copyright (C) 2018 Sven Gregori <sven@craplab.fi>
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
#include <stdint.h>
#include <util/delay.h>
#include "uart.h"
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

/*
 * USB MIDI device and configuration descriptor setup
 * For more details, refer to the Universal Serial Bus Device Class Definition
 * for MIDI Devices Release 1.0 document found at
 * http://www.usb.org/developers/docs/devclass_docs/midi10.pdf
 *
 * The descriptor definition is based and taken from the example found in
 * the above mentioned document, Appendix B.1. and was adjusted to provide
 * one interface (MIDI IN Jack) with one bulk endpoint.
 */

/*
 * USB MIDI adapter device descriptor.
 * Based on the USB device class definition example found in Appendix B.1
 */
const char usb_midi_device_descriptor[] PROGMEM = {
    0x12,                   /* [1] size of this descriptor in bytes (18)    */
    USBDESCR_DEVICE,        /* [1] descriptor type (DEVICE)                 */
    0x10, 0x01,             /* [2] supported USB version (1.10)             */
    0x00,                   /* [1] device class, defined at interface level */
    0x00,                   /* [1] subclass, unused                         */
    0x00,                   /* [1] protocol, unused                         */
    0x08,                   /* [1] max packet size in bytes (8)             */
    USB_CFG_VENDOR_ID,      /* [2] vendor ID (see usbconfig.h)              */
    USB_CFG_DEVICE_ID,      /* [2] device ID (see usbconfig.)               */
    USB_CFG_DEVICE_VERSION, /* [2] device release version (see usbconfig.h) */
    0x01,                   /* [1] manufacturer string index (1)            */
    0x02,                   /* [1] product string index (2)                 */
    0x00,                   /* [1] serial number string index, unused       */
    0x01,                   /* [1] number of configurations (1)             */
};

/*
 * USB MIDI configuration adapter descriptor.
 * Based on the USB device class definition example found in Appendix B.2-B.6
 */
const char usb_midi_config_descriptor[] PROGMEM = {
/* B.2   Configuration Descriptor */
    0x09,               /* [1] size of this descriptor in bytes (9)         */
    USBDESCR_CONFIG,    /* [1] descriptor type (CONFIGURATION)              */
    0x3f, 0x00,         /* [2] total length of descriptor in bytes (63)     */
    0x02,               /* [1] number of interfaces (2)                     */
    0x01,               /* [1] ID of this configuration (1)                 */
    0x00,               /* [1] configuration, unused                        */
    USBATTR_BUSPOWER,   /* [1] attributes                                   */
    0x32,               /* [1] max USB power in 2mA units (50 == 100mA)     */

/* B.3   AudioControl Interface Descriptor */
/* B.3.1 Standard AudioControl Interface Descriptor                         */
    0x09,               /* [1] size of this descriptor in bytes (9)         */
    USBDESCR_INTERFACE, /* [1] descriptor type (INTERFACE)                  */
    0x00,               /* [1] index of this interface (0)                  */
    0x00,               /* [1] index of this setting (0)                    */
    0x00,               /* [1] number of endpoints to follow (0)            */
    0x01,               /* [1] interface class (AUDIO)                      */
    0x01,               /* [1] interface sublass (AUDIO_CONTROL)            */
    0x00,               /* [1] interface protocol, unused                   */
    0x00,               /* [1] interface, unused                            */

/* B.3.2 Class-specific AudioControl Interface Descriptor */
    0x09,               /* [1] size of this descriptor in bytes (9)         */
    0x24,               /* [1] descriptor type (CS_INTERFACE)               */
    0x01,               /* [1] header subtype                               */
    0x00, 0x01,         /* [2] revision of class specification (1.0)        */
    0x09, 0x00,         /* [2] total size of class spec descriptor (9)      */
    0x01,               /* [1] number of streaming interfaces (1)           */
    0x01,               /* [1] MIDIStreaming iface 1 belongs to this iface  */

/* B.4   MIDIStreaming (MS) Interface Descriptors */
/* B.4.1 Standard MIDIStreaming Interface Descriptor */
    0x09,               /* [1] size of this descriptor in bytes (9)         */
    USBDESCR_INTERFACE, /* [1] descriptor type (INTERFACE)                  */
    0x01,               /* [1] index of this interface (1)                  */
    0x00,               /* [1] index of this alternate setting (0)          */
    0x01,               /* [1] number of endpoints to follow (1)            */
    0x01,               /* [1] interface class (AUDIO)                      */
    0x03,               /* [1] interface sublass (MIDSTREAMING)             */
    0x00,               /* [1] interface protocol, unused                   */
    0x00,               /* [1] interface, unused                            */

/* B.4.2 Class-specific MIDIStreaming Interface Descriptor */
    0x07,               /* [1] size of this descriptor in bytes (7)         */
    0x24,               /* [1] descriptor type (CS_INTERFACE)               */
    0x01,               /* [1] header subtype                               */
    0x00, 0x01,         /* [2] revision of class specification (1.0)        */
    0x41, 0x00,         /* [2] total size of class spec descriptor (65)     */

/* B.4.3 MIDI IN Jack Descriptor Embedded */
    0x06,               /* [1] size of this descriptor in bytes (6)         */
    0x24,               /* [1] descriptor type (CS_INTERFACE)               */
    0x02,               /* [1] header subtype (MIDI_IN_JACK)                */
    0x01,               /* [1] jack type (EMBEDDED)                         */
    0x01,               /* [1] jack ID (1)                                  */
    0x00,               /* [1] unused */

/* B.6   Bulk IN Endpoint Descriptors */
/* B.6.1 Standard Bulk IN Endpoint Descriptor */
    0x09,               /* [1] size of this descriptor in bytes (9)         */
    USBDESCR_ENDPOINT,  /* [1] descriptor type (ENDPOINT)                   */
    0x81,               /* [1] endpoint address (IN 1)                      */
    0x03,               /* [1] attribute (interrupt endpoint)               */
    0x08, 0x00,         /* [2] max packet size (8)                          */
    0x0a,               /* [1] interval in ms (10)                          */
    0x00,               /* [1] refresh                                      */
    0x00,               /* [1] sync address                                 */

/* B.6.2 Class-specific MS Bulk IN Endpoint Descriptor */
    0x05,               /* [1] size of this descriptor in bytes (5)         */
    0x25,               /* [1] descriptor type (CS_ENDPOINT)                */
    0x01,               /* [1] descriptor subtype (MS_GENERAL)              */
    0x01,               /* [1] number of embedded MIDI OUT jacks (1)        */
    0x01,               /* [1] id of the embedded MIDI OUT jack (1)         */
};


/**
 * V-USB descriptor setup callback function
 */
uchar
usbFunctionDescriptor(usbRequest_t * request)
{
    if (request->wValue.bytes[1] == USBDESCR_DEVICE) {
        usbMsgPtr = (uchar *) usb_midi_device_descriptor;
        return sizeof(usb_midi_device_descriptor);

    } else {    /* must be config descriptor */
        usbMsgPtr = (uchar *) usb_midi_config_descriptor;
        return sizeof(usb_midi_config_descriptor);
    }
}

/**
 * V-USB setup callback function.
 */
uchar
usbFunctionSetup(uchar data[8])
{
    //usbRequest_t *rq = (void *) data;
    uchar i;

    uart_print("usb setup: ");
    for (i = 0; i < 8; i++) {
        uart_puthex(data[i]);
        uart_putchar(' ');
    }
    uart_newline();

    return 0;
}

/**
 * V-USB read callback function
 */
uchar
usbFunctionRead(uchar *data, uchar len)
{
    memset(data, 0x00, len);
    return len;
}

/**
 * V-USB write callback function
 */
uchar
usbFunctionWrite(uchar *data __attribute__((unused)), uchar len __attribute__((unused)))
{
    return 1;
}

/**
 * V-USB interrupt/bulk write callback function
 */
void
usbFunctionWriteOut(uchar * data, uchar len)
{
    uchar i;

    uart_print("writeout message length ");
    uart_putint(len, 1);
    uart_print(": ");
    for (i = 0; i < len; i++) {
        uart_puthex(data[i]);
    }
    uart_newline();
}



/**
 * Generic USB MIDI message send function.
 * USB MIDI messages are always 4 byte long (padding unused bytes with zero).
 *
 * See also chapter 4 in the Universal Serial Bus Device Class Definition
 * for MIDI Devices Release 1.0 document found at
 * http://www.usb.org/developers/docs/devclass_docs/midi10.pdf
 *
 * For more information on MIDI messages, refer to Summary of MIDI Messages,
 * found at http://www.midi.org/techspecs/midimessages.php
 *
 * @param byte0 USB cable number and code index number
 * @param byte1 MIDI message status byte
 * @param byte2 MIDI message data byte 0
 * @param byte3 MIDI message data byte 1
 */
void
usb_send_midi_message(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
    static uint8_t midi_buf[4];
    uint8_t retries = 10;

    while (--retries) {
        if (usbInterruptIsReady()) {
            midi_buf[0] = byte0;
            midi_buf[1] = byte1;
            midi_buf[2] = byte2;
            midi_buf[3] = byte3;
            usbSetInterrupt(midi_buf, 4);
            return;
        }
        _delay_ms(2);
    }
    uart_print("USB send fail\r\n");
}

