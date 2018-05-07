#
# 4chord MIDI - Makefile
#
# Copyright (C) 2017 Sven Gregori <sven@craplab.fi>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see http://www.gnu.org/licenses/
#

PROGRAM = 4chordmidi

OBJS  = main.o graphics.o spi.o uart.o lcd.o nokia_gfx.o buttons.o gui.o menu.o playback.o usb.o timer.o cli.o fonts.o
OBJS += usbdrv/usbdrv.o usbdrv/usbdrvasm.o
OBJS += playback_mode_chord.o playback_mode_chord_arpeggio.o playback_mode_chord_arpeggio_octave.o playback_mode_arpeggio.o playback_mode_arpeggio_octave.o

include ../common.mk

cli.o: CFLAGS += -DBUILD_DATE_STRING="\"$(shell /bin/date +%d%m%y-%H%M%S)\""

default: $(PROGRAM).hex $(PROGRAM).bin

$(PROGRAM).bin: $(PROGRAM).elf
	@echo "[CP]  $@"
	@$(OBJCOPY) -O binary -R .eeprom $< $@

$(PROGRAM).eep: $(PROGRAM).elf
	@echo "[CP]  $@"
	@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $@

program:
	@echo "\n  +---------------------------------------------------+"
	@echo "  |                      WARNING                      |"
	@echo "  | Flashing 4chord MIDI firmware WITHOUT bootloader! |"
	@echo "  +---------------------------------------------------+\n"
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(PROGRAM).hex

program-eeprom: $(PROGRAM).eep
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$<

distclean::
	@echo "[RM]  $(PROGRAM).bin"
	@rm -f $(PROGRAM).bin
	@echo "[RM]  $(PROGRAM).eep"
	@rm -f $(PROGRAM).eep

.PHONY: program program-eeprom distclean

