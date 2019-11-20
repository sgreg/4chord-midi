#
# 4chord MIDI bootloader - Makefile
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

PROGRAM = bootloader

BOOTLOAD_ADDR = 0x7000

OBJS = main.o uart.o
OBJS += usbdrv/usbdrv.o usbdrv/usbdrvasm.o
ifneq ($(DEBUG),1)
OBJS += spi.o lcd.o gfx.o
endif

include ../../common.mk

LDFLAGS += -Wl,--section-start=.text=$(BOOTLOAD_ADDR)

ifeq ($(DEBUG),1)
CFLAGS += -DDEBUG
endif

APPLICATION = $(wildcard $(BASE_PATH)firmware/*.hex)

default: $(PROGRAM).hex

program-all:
ifeq ($(APPLICATION),)
	@echo "---------------------------------------------------"
	@echo " NOTE: firmware was not built yet, building it now"
	@echo "---------------------------------------------------\n"
	make -C $(BASE_PATH)firmware default
	@echo "\n-----------------------------------------------------------"
	@echo " Done. Please re-run make to actually program the firmware"
	@echo "-----------------------------------------------------------\n"
else
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(APPLICATION) -U flash:w:$(PROGRAM).hex
endif

program:
	@echo "\n  +---------------------------------------------------+"
	@echo "  |                      WARNING                      |"
	@echo "  | Flashing 4chord MIDI bootloader WITHOUT firmware! |"
	@echo "  +---------------------------------------------------+\n"
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(PROGRAM).hex

.PHONY: program-all program

