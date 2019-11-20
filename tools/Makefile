all: atmega328p_fuse_dump

CC = gcc
CFLAGS = -Wall -Wextra

atmega328p_fuse_dump: atmega328p_fuse_dump.c
	$(CC) $(CFLAGS) $^ -o $@

graphics:
	./xbmtool.sh -f -n gfx -o ../bootloader/device ../bootloader/device/*.xbm
	./xbmtool.sh -f -n gfx -o ../firmware ../graphics/gfx/*.xbm
	./xbmtool.sh -a -n intro -o ../firmware ../graphics/intro/*.xbm

clean:
	rm -f atmega328p_fuse_dump

clean-graphics:
	rm -f ../bootloader/device/gfx.c
	rm -f ../bootloader/device/gfx.h
	rm -f ../firmware/gfx.c
	rm -f ../firmware/gfx.h
	rm -f ../firmware/intro.c
	rm -f ../firmware/intro.h

.PHONY : all clean clean-graphics

