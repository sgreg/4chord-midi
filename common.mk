#
# 4chord MIDI - Common Makefile definitions
#
# Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
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

# Do some sanity check
# Two variables need to be set to make this work:
#   PROGRAM - the name of the output executable, hex and map file
#   OBJS    - list of object files to include into the compilation
#
ifndef PROGRAM
$(error 'PROGRAM' variable not set)
endif
ifndef OBJS
$(error 'OBJS' variable not set)
endif

# Set base path from including directory to directory this file is in
BASE_PATH := $(dir $(lastword $(MAKEFILE_LIST)))

# MCU type and clock speed in Hz
MCU = atmega328p
F_CPU = 12000000


# Fuses
# If you're looking for a way to fuck up your controller, here you go.
#
# To be a bit on the safer side, run `make fuses-dump` using the
# ./tools/atmega328p_fuse_dump tool and verify its output are the
# correct fuse settings.
#
FUSE_LOW = 0xf7
FUSE_HIGH = 0xd0
FUSE_EXTENDED = 0xff
FUSE_DUMP_TOOL = $(BASE_PATH)/tools/atmega328p_fuse_dump

# Set executable names
CC = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude

# Set compile flags, parameters etc.
CFLAGS += -g -Os -std=gnu99 -I. \
-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
-Wall -Wextra -Wstrict-prototypes \
-DF_CPU=$(F_CPU) -mmcu=$(MCU) 

ASFLAGS_C += -Wa,-adhlms=$(<:.c=.lst),-gstabs
ASFLAGS_ASM += -Wa,-gstabs 

LDFLAGS += -Wl,-Map=$(PROGRAM).map,--cref

SIZE_FLAGS += -C --mcu=$(MCU)
AVRDUDE_FLAGS += -p $(MCU) -c usbasp


.PRECIOUS : %.elf %.o

all: default size

size:
	@echo ""
	@$(SIZE) $(SIZE_FLAGS) $(PROGRAM).elf

$(PROGRAM).hex: $(PROGRAM).elf
	@echo "[CP]  $@"
	@$(OBJCOPY) -O ihex -R .eeprom $< $@

$(PROGRAM).elf: $(OBJS)
	@echo "[LD]  $@"
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	@echo "[CC]  $@"
	@$(CC) -c $(CFLAGS) $(ASFLAGS_C) $< -o $@

%.o: %.S
	@echo "[AS]  $@"
	@$(CC) -c $(CFLAGS) -x assembler-with-cpp $(ASFLAGS_ASM) $< -o $@


fuses-dump:
	@if [ -x $(FUSE_DUMP_TOOL) ] ; then \
		$(FUSE_DUMP_TOOL) $(FUSE_LOW) $(FUSE_HIGH) $(FUSE_EXTENDED) ; \
		echo "" ; \
	else \
		echo "Fuse Dump Tool not available" ; \
	fi

fuses: fuses-dump
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:$(FUSE_LOW):m -U hfuse:w:$(FUSE_HIGH):m -U efuse:w:$(FUSE_EXTENDED):m

clean:
	@echo "[RM]  $(OBJS)"
	@rm -f $(OBJS)

distclean:: clean
	@echo "[RM]  $(OBJS:.o=.lst)"
	@rm -f $(OBJS:.o=.lst)
	@echo "[RM]  $(PROGRAM).map"
	@rm -f $(PROGRAM).map
	@echo "[RM]  $(PROGRAM).elf"
	@rm -f $(PROGRAM).elf
	@echo "[RM]  $(PROGRAM).hex"
	@rm -f $(PROGRAM).hex

.PHONY: all size fuses-dump fuses clean distclean

