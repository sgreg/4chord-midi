#ifndef _USB_H_
#define _USB_H_

void usb_send_midi_message(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);
#define midi_msg_note_on(note, velocity) usb_send_midi_message(0x09, 0x90, note, velocity)
#define midi_msg_note_off(note, velocity) usb_send_midi_message(0x08, 0x80, note, velocity)

#endif
