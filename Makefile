#
# 4chord MIDI - Global Makefile
#
# Copyright (C) 2020 Sven Gregori <sven@craplab.fi>
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

all: tools firmware bootloader

tools:
	make -C tools/

graphics:
	make -C tools/ clean-graphics
	make -C tools/ graphics

firmware:
	make -C firmware/

bootloader:
	make -C bootloader/device/

check-programmer:
	make -C firmware/ check-programmer

fuses:
	make -C firmware/ fuses

program:
	make -C bootloader/device/ program-all

clean:
	make -C tools/ clean
	make -C firmware/ clean
	make -C bootloader/device/ clean

distclean:
	make -C firmware/ distclean
	make -C bootloader/device/ distclean

.PHONY: all tools graphics firmware bootloader check-programmer fuses program clean distclean

