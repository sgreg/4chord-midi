/*
 * AVR ATmega328P fuse bit dump tool
 *
 * Copyright (C) 2020 Sven Gregori <sven@craplab.fi>
 *
 *
 * Outputs human readable representation of AVR ATmega328P fuse bit settings.
 *
 * Usage: ./atmega328p_fuse_dump <low> <high> <extended>
 *
 * Pass the three fuse bytes low, high and extended as hexadecimal value, e.g.
 *        ./atmega328p_fuse_dump 0xf7 0xd9 0xff
 * It then parses the fuse bit valus and outputs all its settings to stdout.
 *
 *
 * This tool only outputs the fuse byte data. To set up the values, have a
 * look at http://www.engbedded.com/fusecalc/
 *
 *
 * Note: Only ATmega328p devices are supported! Using this tool to verify
 *       fuse settings for other microcontrollers is not recommended.
 *
 * Note: This tool was written to run on x86_64 platform. If you're planning
 *       to run it on a different platform, make sure the bitfield endianess
 *       is correct.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Fuse byte representation.
 * Each of the three fuse bytes are accessible in three ways:
 *      "bits" struct:   access single bits
 *      "fields" struct: access combined multi-bit fields
 *      "byte" integer:  direct integer value of the byte
 */
union fuse_byte_low {
    struct {
        uint8_t CKSEL0:1;
        uint8_t CKSEL1:1;
        uint8_t CKSEL2:1;
        uint8_t CKSEL3:1;
        uint8_t SUT0:1;
        uint8_t SUT1:1;
        uint8_t CKOUT:1;
        uint8_t CKDIV8:1;
    } bits;
    struct {
        uint8_t CKSEL:4;
        uint8_t SUT:2;
        uint8_t CKOUT:1;
        uint8_t CKDIV8:1;
    } fields;
    uint8_t byte;
};

union fuse_byte_high {
    struct {
        uint8_t BOOTRST:1;
        uint8_t BOOTSZ0:1;
        uint8_t BOOTSZ1:1;
        uint8_t EESAVE:1;
        uint8_t WDTON:1;
        uint8_t SPIEN:1;
        uint8_t DWEN:1;
        uint8_t RSTDISBL:1;
    } bits;
    struct {
        uint8_t BOOTRST:1;
        uint8_t BOOTSZ:2;
        uint8_t EESAVE:1;
        uint8_t WDTON:1;
        uint8_t SPIEN:1;
        uint8_t DWEN:1;
        uint8_t RSTDISBL:1;
    } fields;
    uint8_t byte;
};

union fuse_byte_extended {
    struct {
        uint8_t BODLEVEL0:1;
        uint8_t BODLEVEL1:1;
        uint8_t BODLEVEL2:1;
        uint8_t _reserved0:1;
        uint8_t _reserved1:1;
        uint8_t _reserved2:1;
        uint8_t _reserved3:1;
        uint8_t _reserved4:1;
    } bits;
    struct {
        uint8_t BODLEVEL:3;
        uint8_t _reserved:5;
    } fields;
    uint8_t byte;
};


/*
 * Functions to print the fuse byte data
 */
void
print_fuse_low(union fuse_byte_low *fuse)
{
    /* Clock source strings, fuse->fields.CLSEL is used as index */
    char *clksources[] = {
        /* 0000 */ "External Clock",
        /* 0001 */ "Undefined value!",
        /* 0010 */ "Internal 8MHz RC Oscillator",
        /* 0011 */ "Internal 128kHz RC Oscillator",
        /* 0100 */ "External Low Frequency Crystal Oscillator",
        /* 0101 */ "External Low Frequency Crystal Oscillator",
        /* 0110 */ "External Full Swing Crystal Oscillator",
        /* 0111 */ "External Full Swing Crystal Oscillator",
        /* 1000 */ "External Low Power Crystal Oscillator",
        /* 1001 */ "External Low Power Crystal Oscillator",
        /* 1010 */ "External Low Power Crystal Oscillator",
        /* 1011 */ "External Low Power Crystal Oscillator",
        /* 1100 */ "External Low Power Crystal Oscillator",
        /* 1101 */ "External Low Power Crystal Oscillator",
        /* 1110 */ "External Low Power Crystal Oscillator",
        /* 1111 */ "External Low Power Crystal Oscillator"
    };

    /* 
     * Clock startup time strings for different clock sources.
     * Outer index is mapped by the starttime_map array (see below)
     * Inner index is defined by fuse->fields.SUT
     */
    char *starttimes[][4] = {
        { /* 0: CKSEL = 00xx external clock or internal osciallator */
            "6K CK / 14 CK + 0ms",
            "6K CK / 14 CK + 4.1ms",
            "6K CK / 14 CK + 65ms",
            "Undefined value!"
        },
        { /* 1: CKSEL = 0100 low freq crystal */
            "1K CK / 4 CK + 0ms",
            "1K CK / 4 CK + 4.1ms",
            "1K CK / 4 CK + 65ms",
            "Undefined value!"
        },
        { /* 2: CKSEL = 0101 low freq crystal */
            "32K CK / 4 CK + 0ms",
            "32K CK / 4 CK + 4.1ms",
            "32K CK / 4 CK + 65ms",
            "Undefined value!"
        },
        { /* 3: CKSEL = 1xx0 low power crystal or
                CKSEL = 0110 full swing crystal */
            "258 CK / 14 CK + 4.1ms",
            "258 CK / 14 CK + 65ms",
            "1K CK / 14 CK + 0ms",
            "1K CK / 14 CK + 4.1ms",
        },
        { /* 4: CKSEL = 1xx1 low power crystal or
                CKSEL = 0111 full swing crystal */
            "1K CK / 14 CK + 65ms",
            "16K CK / 14 CK + 0ms",
            "16K CK / 14 CK + 4.1ms",
            "16K CK / 14 CK + 65ms"
        }
    };

    /*
     * Map clock start time options to clock sources.
     * The index is defined by fuse->fields.CKSEL (same as clksources[] above)
     * and the value references the index in the inner starttimes[][] array.
     * e.g. the internal 8MHz RC osciallator (clksource index 2) has clock
     * start time options as defined in index 0 of starttimes.
     */
    int starttime_map[] = {
        0, 0, 0, 0,
        1, 2, 3, 4,
        3, 4, 3, 4,
        3, 4, 3, 4
    };

    /* Print low byte bits and fields */
    printf("Clock source:                         %s\n",
            clksources[fuse->fields.CKSEL]);
    printf("Clock startup time:                   %s\n",
            starttimes[starttime_map[fuse->fields.CKSEL]][fuse->fields.SUT]);
    printf("Clock output:                         %s\n",
            (!fuse->bits.CKOUT) ? "enabled" : "disabled");
    printf("Clock speed divided by 8:             %s\n",
            (!fuse->bits.CKDIV8) ? "yes" : "no");
}

void
print_fuse_high(union fuse_byte_high *fuse)
{
    /* Boot flash sizes in words */
    int bootsizes[] = {2048, 1024, 512, 256};

    /* Print high byte bits and fields */
    printf("Reset to bootloader:                  %s\n",
            (!fuse->bits.BOOTRST) ? "yes (WARNING)" : "no");
    printf("Bootloader size:                      %d words\n",
            bootsizes[fuse->fields.BOOTSZ]);
    printf("Preserve EEPROM memory during erase:  %s\n",
            (!fuse->bits.EESAVE) ? "yes" : "no");
    printf("Watchdog always on:                   %s\n",
            (!fuse->bits.WDTON) ? "yes" : "no");
    printf("Serial programming and data download: %s\n",
            (!fuse->bits.SPIEN) ? "enabled" : "disabled (WARNING!)");
    printf("debugWIRE:                            %s\n",
            (!fuse->bits.WDTON) ? "enabled" : "disabled");
    printf("Reset Pin function:                   %s\n",
            (!fuse->bits.RSTDISBL) ? "PC6 I/O" : "External Reset");
}

void
print_fuse_extended(union fuse_byte_extended *fuse)
{
    /*
     * Brown-out detection (BOD) trigger level values.
     * Note, fuse->fields.BODLEVEL is 3 bits wide, but the MSB is always
     * one in all available option, so only the 2 LSBs are needed
     */
    char *bodlevels[] = {"4.3V", "2.7V", "1.8V", "disabled"};

    /* Print extended byte BOD level field */
    printf("Brown-out Detection trigger level:    %s\n",
            bodlevels[fuse->fields.BODLEVEL & 0x3]);
}

int
main(int argc, char **argv)
{
    union fuse_byte_low low;
    union fuse_byte_high high;
    union fuse_byte_extended extended;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <low> <high> <extended>\n", argv[0]);
        return 1;
    }

    /* parse parameters to fuse byte unions */
    low.byte      = strtol(argv[1], NULL, 16);
    high.byte     = strtol(argv[2], NULL, 16);
    extended.byte = strtol(argv[3], NULL, 16);

    print_fuse_low(&low);
    print_fuse_high(&high);
    print_fuse_extended(&extended);
    
    return 0;
}

