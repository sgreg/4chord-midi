all: atmega328p_fuse_dump graphics

CC = gcc
CFLAGS = -Wall -Wextra

atmega328p_fuse_dump: atmega328p_fuse_dump.c
	$(CC) $(CFLAGS) $^ -o $@

graphics:
	./xbmtool.sh -f -n gfx -o ../firmware ../graphics/gfx/*.xbm
	./xbmtool.sh -a -n intro -o ../firmware ../graphics/intro/*.xbm

clean:
	rm -f atmega328p_fuse_dump

.PHONY : all clean

