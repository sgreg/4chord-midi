/*
 * 4chord MIDI - USB MIDI
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
#ifndef _USB_H_
#define _USB_H_

#define USB_MIDI_CABLE_NUM      0
#define MIDI_CHANNEL_NUMBER     0

#define USB_CMD_MIDI_NOTE_ON    ((USB_MIDI_CABLE_NUM << 4) | 0x09)
#define USB_CMD_MIDI_NOTE_OFF   ((USB_MIDI_CABLE_NUM << 4) | 0x08)

#define MIDI_NOTE_ON    (0x90 | MIDI_CHANNEL_NUMBER)
#define MIDI_NOTE_OFF   (0x80 | MIDI_CHANNEL_NUMBER)

/**
 * Generic USB MIDI message send function.
 * USB MIDI messages are always 4 byte long (padding unused bytes with zero).
 *
 * See also chapter 4 in the Universal Serial Bus Device Class Definition
 * for MIDI Devices Release 1.0 document found at
 * https://usb.org/sites/default/files/midi10.pdf
 *
 * For more information on MIDI messages, refer to Summary of MIDI Messages,
 * found at https://www.midi.org/specifications/item/table-1-summary-of-midi-message
 *
 * @param byte0 USB cable number and code index number
 * @param byte1 MIDI message status byte
 * @param byte2 MIDI message data byte 0
 * @param byte3 MIDI message data byte 1
 */
void usb_send_midi_message(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);

/**
 * Send a MIDI "Note On" message over USB.
 * @param note MIDI note key number
 * @param velocity MIDI note velocity
 */
#define midi_msg_note_on(note, velocity) \
    usb_send_midi_message(USB_CMD_MIDI_NOTE_ON, MIDI_NOTE_ON, note, velocity)

/**
 * Send a MIDI "Note Off" message over USB.
 * @param note MIDI note key number
 * @param velocity MIDI note velocity
 */
#define midi_msg_note_off(note, velocity) \
    usb_send_midi_message(USB_CMD_MIDI_NOTE_OFF, MIDI_NOTE_OFF, note, velocity)

#endif
