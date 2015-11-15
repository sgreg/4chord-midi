#
# 4chord midi - Makefile
#

MCU = atmega88
F_CPU = 8000000

OBJS = main.o graphics.o spi.o uart.o lcd3310.o buttons.o gui.o chords.o

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

CFLAGS = -g -Os -std=gnu99 -I. \
-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums \
-Wall -Wextra -Wstrict-prototypes \
-DF_CPU=$(F_CPU) -mmcu=$(MCU) 

ASFLAGS = -Wa,-adhlms=$(<:.c=.lst),-gstabs 
ASFLAGS_ASM = -Wa,-gstabs 
LDFLAGS = -Wl,-Map=$(<:.o=.map),--cref

AVRDUDE = avrdude
AVRDUDE_FLAGS = -p $(MCU) -c usbasp


.PRECIOUS : %.elf %.o

all: 4chordmidi.hex

4chordmidi.hex: 4chordmidi.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@$(SIZE) $^

4chordmidi.elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(ASFLAGS) $(BUILD_DATE_DEFINE) $< -o $@

%.o: %.S
	$(CC) -c $(CFLAGS) -x assembler-with-cpp $(ASFLAGS_ASM) $< -o $@

# Fuses settings: CLK 8MHz internal oscillator, no clock divider, 6ck/14ck + 65ms
fuses:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m


program:
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:4chordmidi.hex

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f *.elf
	rm -f *.hex
	rm -f *.lst
	rm -f *.map

# Listing of phony targets.
.PHONY : all fuses program clean distclean

