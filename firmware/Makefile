#
# 4chord midi - Makefile
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

MCU = atmega328p
F_CPU = 12000000

OBJS  = main.o graphics.o spi.o uart.o lcd3310.o buttons.o gui.o menu.o playback.o usb.o timer.o cli.o
OBJS += usbdrv/usbdrv.o usbdrv/usbdrvasm.o
OBJS += playback_mode_chord.o playback_mode_chord_arpeggio.o playback_mode_chord_arpeggio_octave.o playback_mode_arpeggio.o playback_mode_arpeggio_octave.o


# Fuses
# If you're looking for a way to fuck up your controller, here you go.
#
# To be a bit on the safer side, run `make fuses-dump` using the
# ../tools/atmega328p_fuse_dump tool and verify its output are the
# correct fuse settings.
#
FUSE_LOW = 0xf7
FUSE_HIGH = 0xd9
FUSE_EXTENDED = 0xff
FUSE_DUMP_TOOL = ../tools/atmega328p_fuse_dump


CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

CFLAGS = -g -Os -std=gnu99 -I. \
-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
-Wall -Wextra -Wstrict-prototypes \
-DF_CPU=$(F_CPU) -mmcu=$(MCU) 

ASFLAGS_C = -Wa,-adhlms=$(<:.c=.lst),-gstabs
ASFLAGS_ASM = -Wa,-gstabs 

LDFLAGS = -Wl,-Map=$(PROGRAM).map,--cref

AVRDUDE_FLAGS = -p $(MCU) -c usbasp

.PRECIOUS : %.elf %.o

all: $(PROGRAM).hex

$(PROGRAM).hex: $(PROGRAM).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@$(SIZE) $^

$(PROGRAM).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cli.o: CFLAGS += -DBUILD_DATE_STRING="\"$(shell /bin/date +%d%m%y-%H%M%S)\""

%.o: %.c
	$(CC) -c $(CFLAGS) $(ASFLAGS_C) $< -o $@

%.o: %.S
	$(CC) -c $(CFLAGS) -x assembler-with-cpp $(ASFLAGS_ASM) $< -o $@

# Fuses settings: CLK 8MHz internal oscillator, no clock divider, 6ck/14ck + 65ms
fuses-dump:
	@if [ -x $(FUSE_DUMP_TOOL) ] ; then \
		$(FUSE_DUMP_TOOL) $(FUSE_LOW) $(FUSE_HIGH) $(FUSE_EXTENDED) ; \
		echo "" ; \
	else \
		echo "Fuse Dump Tool not available" ; \
	fi

fuses: fuses-dump
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXTENDED):m


program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:4chordmidi.hex

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(OBJS:.o=.lst)
	rm -f $(PROGRAM).map
	rm -f $(PROGRAM).elf
	rm -f $(PROGRAM).hex

.PHONY : all fuses-dump fuses program clean distclean

