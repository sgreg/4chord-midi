all: atmega328p_fuse_dump graphics

CC = gcc
CFLAGS = -Wall -Wextra

atmega328p_fuse_dump: atmega328p_fuse_dump.c
	$(CC) $(CFLAGS) $^ -o $@

graphics:
	./create_graphics.sh ../graphics ../firmware

clean:
	rm -f atmega328p_fuse_dump

.PHONY : all clean

