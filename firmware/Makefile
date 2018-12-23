#
# 4chord MIDI - Makefile
#
# Copyright (C) 2018 Sven Gregori <sven@craplab.fi>
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
EEPROM_FILE = $(PROGRAM).eep

OBJS  = main.o gfx.o intro.o spi.o uart.o lcd.o buttons.o gui.o menu.o playback.o usb.o timer.o cli.o fonts.o eeprom.o
OBJS += usbdrv/usbdrv.o usbdrv/usbdrvasm.o
OBJS += playback_mode_chord.o playback_mode_chord_arpeggio.o playback_mode_chord_arpeggio_octave.o playback_mode_arpeggio.o playback_mode_arpeggio_octave.o

include ../common.mk

cli.o: CFLAGS += -DBUILD_DATE_STRING="\"$(shell /bin/date +%Y%m%d-%H%M%S)\""

default: $(PROGRAM).hex $(PROGRAM).bin

$(PROGRAM).bin: $(PROGRAM).elf
	@echo "[CP]  $@"
	@$(OBJCOPY) -O binary -R .eeprom $< $@

program: $(PROGRAM).hex
	@echo ""
	@echo "  +---------------------------------------------------+"
	@echo "  |                      WARNING                      |"
	@echo "  | Flashing 4chord MIDI firmware WITHOUT bootloader! |"
	@echo "  +---------------------------------------------------+"
	@echo ""
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(PROGRAM).hex


eeprom: $(EEPROM_FILE)

$(EEPROM_FILE): $(PROGRAM).elf
	@echo "[CP]  $@.bin"
	@$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O binary $< $@.bin
	@echo "[CP]  $@"
	@$(OBJCOPY) -O ihex -I binary $@.bin $@

program-eeprom: eeprom
	@echo ""
	@echo "  +-----------------------------------------------------+"
	@echo "  |                       WARNING                       |"
	@echo "  |                                                     |"
	@echo "  |  You are about to overwrite the EEPROM content and  |"
	@echo "  |  replace all existing values with your own custom   |"
	@echo "  |  data, which may result in your device starting to  |"
	@echo "  |  act funny, or fallback mechanisms in the firmware  |"
	@echo "  |  might corrupt your custom data.                    |"
	@echo "  |                                                     |"
	@echo "  |  PROCEED AT OWN RISK!                               |"
	@echo "  |                                                     |"
	@echo "  +-----------------------------------------------------+"
	@echo ""
	@read -p "Are you sure you want to replace the EEPROM content? [y/N] " choice; \
	[ "$${choice,,}" == "y" ] || (\
		echo "Aborting"; \
		exit 1; \
	)
	@read -p "Would you like to backup the current EEPROM content? [Y/n] " choice; \
	[ "$${choice,,}" == "n" ] || (\
		outfile="4chordmidi-$$(/bin/date +%Y-%m-%d-%H%M%S).eep"; \
		echo -e "\nReading back EEPROM content to\n    $$outfile"; \
		$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:r:$$outfile:i || exit 1; \
	)
	@echo -e "\nWriting new EEPROM data\n"
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U eeprom:w:$(EEPROM_FILE)
	@echo ""
	@echo "  +-----------------------------------------------------+"
	@echo "  |                                                     |"
	@echo "  |  All done.                                          |"
	@echo "  |                                                     |"
	@echo "  |  To ensure proper operation, make sure your EEPROM  |"
	@echo "  |  data is always in sync with the firmware that is   |"
	@echo "  |  accessing and validating the data itself.          |"
	@echo "  |                                                     |"
	@echo "  +-----------------------------------------------------+"
	@echo ""

distclean::
	@echo "[RM]  $(PROGRAM).bin"
	@rm -f $(PROGRAM).bin
	@echo "[RM]  $(EEPROM_FILE) $(EEPROM_FILE).bin"
	@rm -f $(EEPROM_FILE) $(EEPROM_FILE).bin

.PHONY: program eeprom program-eeprom distclean

