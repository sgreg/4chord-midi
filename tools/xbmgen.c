/*
 * xbmgen Nokia 5110 LCD data generator
 *
 * Copyright (C) 2018 Sven Gregori <sven@craplab.fi>
 * Released under GPLv2
 *
 * Generates raw data used for direct use with a Nokia 5110 display from
 * an XBM image file. Since an XBM file is essentially just C code, the
 * XBM file itself is included in the xbmgen.h header that was generated
 * by the xbmtool.sh script.
 *
 * Depending on the defines in the xbmgen.h header file and other extra
 * defines passed as flags to gcc from xbmtool.sh, this code will create
 * either a full frame uint8_t array, a key diff frame struct, or a diff
 * frame struct, prefilled with the raw data that can be written as-is to
 * the LCD. Either way, the XBM input file is rotated 90 degree counter
 * clockwise, flipped vertically, and then transformed to match the LCD
 * controller's memory arrangements. Header file definitions of the
 * generated data are written to stderr, the data itself to stdout.
 *
 * For operation modes and how the defines relate to them, check the
 * comments with the main() function at the very end of the file.
 *
 * Note, this file is currently hardcoded to generate C code for 8-bit
 * AVR microcontrollers with avr-gcc as compiler. Data itself is defined
 * to end up in program memory using the PROGMEM attribute. Eventually,
 * this could/should be adjusted to support other architectures as well.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xbmgen.h"

#define DIFF_REALLOC 16
#define BYTES_PER_DIFF 2

struct diff {
    uint16_t addr;
    uint8_t data;
};

struct frame {
    uint16_t diffcnt;
    struct diff *diffs;
};

struct key_diff_frame {
    uint8_t base_value;
    struct frame frame;
};

#define TYPE_FULL 0
#define TYPE_DIFF 1
#define TYPE_KEY_DIFF 2
#define TYPE_NONE 3


/**
 * Return the number of bytes required to store a given number.
 * This is basically performing a special case ceil() operation.
 *
 * @param value number to get byte count for
 * @return number of bytes required to store the given number
 */
int
bytes(int value)
{
    /*
     * Yes, there are lots of more leet ways to do this (shift, modulo,
     * bitwise ANDing, using ternary operator), but gcc will either way
     * transform it in identical code, so I went for readability instead.
     */
    int bytes = value / 8;

    if (bytes * 8 == value) {
        return bytes;
    }
    return bytes + 1;
}

/**
 * Returns the size in bytes of the XBM image by multiplying the frame
 * width with the number of bytes required for the frame height.
 *
 * @return Size in bytes of the XBM image data
 */
size_t
frame_size(void)
{
    return bytes(xbmgen_frame_height) * xbmgen_frame_width;
}

/**
 * Returns the maximum number of diffs a transition or key diff graphic
 * can have to make the approach more space efficient than a full frame
 * char array.
 *
 * @return Maximum number of diffs to have a benefit in the diff approach
 */
int
max_diff_benefit(void)
{
    return frame_size() / BYTES_PER_DIFF;
}

/**
 * Allocate memory for diff structs inside a given frame.
 *
 * Whenever needed, a new chunk of memory for DIFF_REALLOC diff struct
 * elements is allocated for the given frame struct.
 *
 * @param frame frame struct to allocate diff struct memory for
 */
void
frame_alloc(struct frame *frame)
{
    if (frame->diffcnt % DIFF_REALLOC == 0) {
        frame->diffs = (struct diff *) realloc(
                frame->diffs,
                (frame->diffcnt + DIFF_REALLOC) * sizeof(struct diff));
    }
}

/**
 * Rotate and flip input buffer into given output buffer.
 *
 * @param data_in input buffer
 * @param out output buffer with rotated and flipped data
 */
void
rotate_flip(uint8_t *data_in, uint8_t *out)
{
    int width_off  = bytes(xbmgen_frame_width);
    int height_off = bytes(xbmgen_frame_height);

    int width, height;
    int width_byte, width_bit;
    int height_byte, height_bit;

    int data_byte;
    int data_value;

    uint8_t out_byte;
    for (width = 0; width < xbmgen_frame_width; width++) {
        out_byte = 0;
        for (height = 0; height < xbmgen_frame_height; height++) {
            width_byte = width / 8; // byte offset inside width
            width_bit  = width - (width_byte * 8); // bit offset inside byte offsetted byte ..eh

            data_byte  = data_in[height * width_off + width_byte]; // current byte data (width)
            data_value = (data_byte >> width_bit) & 0x01; // width data bit value

            height_byte = height / 8; // byte offset in height
            height_bit  = height - (height_byte * 8); // bit offset in byte offset

            out_byte |= data_value << height_bit;

            if (height_bit == 7 && height < xbmgen_frame_height - 1) {
                out[width * height_off + height_byte] = out_byte;
                out_byte = 0;
            }
        }

        out[width * height_off + height_byte] = out_byte;
    }
}

/**
 * Arrange buffer for LCD memory by transforming data in the way of:
 *
 *  x0_0, x0_1, x0_2, ..., x0_m      x0_0, x1_0, x2_0, ..., xn_0
 *  x1_0, x1_1, x1_2, ..., x1_m      x0_1, x1_1, x2_1, ..., xn_1
 *  x2_0, x2_1, x2_2, ..., x2_m  ->  x0_2, x1_2, x3_1, ..., xn_2
 *  ...                              ...
 *  xn_0, xn_1, xn_2, ..., xn_m      x0_m, x1_m, x3_m, ..., xn_m
 *
 * with m being the frame width, and n being bytes(frame height).
 * In other words, matrix transposition.
 *
 * @param in input buffer (rotated and flipped image)
 * @param out output buffer with re-arranged LCD memory data
 * @param buflen output buffer size
 */
void
arrange_mem(uint8_t *in, uint8_t *out, size_t buflen)
{
    size_t idx;
    int row;
    int rotate_row = 0;
    int out_index = 0;
    const int row_count = bytes(xbmgen_frame_height);

    // there is probably a more elegant way to do this..
    for (row = 0; row < row_count; row++) {
        for (idx = 0; idx < buflen; idx++) {
            if (rotate_row == row) {
                out[out_index++] = in[idx];
            }

            if (++rotate_row == row_count) {
                rotate_row = 0;
            }
        }
    }
}

/**
 * Print out keyframe C code.
 * Code for .c file is written to stdout, header file code to stderr.
 *
 * @param buffer keyframe data
 * @param buflen size of keyframe data
 */
void
print_full_frame(uint8_t *buffer, size_t buflen)
{
    size_t i;

#ifdef DIFF_FRAME
    fprintf(stderr, "/* full frame for %s */\n", xbm_file2);
    fprintf(stdout, "/* full frame for %s */\n", xbm_file2);
#else
    fprintf(stderr, "/* full frame for %s */\n", xbm_file);
    fprintf(stdout, "/* full frame for %s */\n", xbm_file);
#endif
    fprintf(stderr, "extern const uint8_t %s[];\n\n", framename);
    fprintf(stdout, "const uint8_t %s[] PROGMEM = {", framename);

    for (i = 0; i < buflen; i++) {
        if (i % 8 == 0) {
            fprintf(stdout, "\n        ");
        }
        fprintf(stdout, "0x%02x, ", buffer[i]);
    }

    fprintf(stdout, "\n};\n\n");
}

#ifdef DIFF_FRAME
/**
 * Print out frame transistion C code.
 * Code for .c file is written to stdout, header file code to stderr.
 *
 * @param frame diff frame transistion struct
 */
void
print_diff_frame(struct frame *frame)
{
    int i;
    struct diff *diff;

    fprintf(stderr, "/* diff frame for %s -> %s */\n", xbm_file1, xbm_file2);
    fprintf(stderr, "extern const struct xbmlib_diff_frame %s;\n\n", framename);

    fprintf(stdout, "/* diff frame for %s -> %s */\n", xbm_file1, xbm_file2);
    fprintf(stdout, "const struct xbmlib_diff_frame %s PROGMEM = {\n", framename);
    fprintf(stdout, "    .diffcnt = %d,\n", frame->diffcnt);
    fprintf(stdout, "    .diffs = {");

    for (i = 0; i < frame->diffcnt; i++) {
        if (i % 4 == 0) {
            if (i != 0) {
                fprintf(stdout, ",");
            }
            fprintf(stdout, "\n        ");
        } else {
            fprintf(stdout, ", ");
        }

        diff = &frame->diffs[i];
        fprintf(stdout, "{%3d, 0x%02x}", (diff->addr & 0xff), diff->data);
    }

    fprintf(stdout, "\n    }\n};\n\n");
}
#endif /* DIFF_FRAME */

/**
 * Print out key diff frame C code.
 * Code for .c file is written to stdout, header file code to stderr.
 *
 * @param key_diff_frame key diff frame struct
 */
void
print_key_diff_frame(struct key_diff_frame *key_diff_frame)
{
    int i;
    struct diff *diff;
    struct frame *frame = &key_diff_frame->frame;

#ifdef DIFF_FRAME
    fprintf(stderr, "/* key diff frame for %s */\n", xbm_file2);
    fprintf(stdout, "/* key diff frame for %s */\n", xbm_file2);
#else
    fprintf(stderr, "/* key diff frame for %s */\n", xbm_file);
    fprintf(stdout, "/* key diff frame for %s */\n", xbm_file);
#endif
    fprintf(stderr, "extern const struct xbmlib_key_diff_frame %s;\n\n", framename);
    fprintf(stdout, "const struct xbmlib_key_diff_frame %s PROGMEM = {\n", framename);
    fprintf(stdout, "    .base_value = 0x%02x,\n", key_diff_frame->base_value);
    fprintf(stdout, "    .diffcnt = %d,\n", frame->diffcnt);
    fprintf(stdout, "    .diffs = {");

    for (i = 0; i < frame->diffcnt; i++) {
        if (i % 4 == 0) {
            if (i != 0) {
                fprintf(stdout, ",");
            }
            fprintf(stdout, "\n        ");
        } else {
            fprintf(stdout, ", ");
        }

        diff = &frame->diffs[i];
        fprintf(stdout, "{%3d, 0x%02x}", (diff->addr & 0xff), diff->data);
    }

    fprintf(stdout, "\n    }\n};\n\n");
}

/**
 * Creates full frame char array from xbm image.
 */
void
process_full_frame(void)
{
    size_t buflen = frame_size();
    uint8_t *rotbuf = calloc(1, buflen);
    uint8_t *outbuf = calloc(1, buflen);

    rotate_flip(xbmgen_frame2_data, rotbuf);
    arrange_mem(rotbuf, outbuf, buflen);

    print_full_frame(outbuf, buflen);

    free(outbuf);
    free(rotbuf);
}

#ifdef DIFF_FRAME
/**
 * Creates diff frame information from xbm images and stores it in the
 * given frame struct.
 *
 * @param frame diff frame struct to store processed data in
 */
void
get_diff_frame(struct frame *frame)
{
    size_t i;
    size_t buflen = frame_size();
    uint8_t *rotbuf1 = calloc(1, buflen);
    uint8_t *rotbuf2 = calloc(1, buflen);
    uint8_t *outbuf1 = calloc(1, buflen);
    uint8_t *outbuf2 = calloc(1, buflen);
    struct diff *diff;

    memset(frame, 0, sizeof(struct frame));

    rotate_flip(xbmgen_frame1_data, rotbuf1);
    rotate_flip(xbmgen_frame2_data, rotbuf2);

    arrange_mem(rotbuf1, outbuf1, buflen);
    arrange_mem(rotbuf2, outbuf2, buflen);

    for (i = 0; i < buflen; i++) {
        if (outbuf1[i] != outbuf2[i]) {
            frame_alloc(frame);
            diff = &frame->diffs[frame->diffcnt++];

            if (frame->diffcnt == 1 && i > 0xff) {
                /*
                 * first diff is after address overflow, so add a dummy
                 * entry at 0xff to ensure proper offset calculation when
                 * displaying the data
                 */
                diff->addr = 0xff;
                diff->data = outbuf1[0xff];
                frame_alloc(frame);
                diff = &frame->diffs[frame->diffcnt++];
            }

            diff->addr = i;
            diff->data = outbuf2[i];
        }
    }

    free(outbuf1);
    free(outbuf2);
    free(rotbuf1);
    free(rotbuf2);
}
#endif /* DIFF_FRAME */

/**
 * Creates key diff frame information from xbm image and stores it in the
 * given key_diff_frame struct.
 *
 * @param frame diff frame struct to store processed data in
 */
void
get_key_diff_frame(struct key_diff_frame *key_diff_frame)
{
    size_t i;
    size_t buflen = frame_size();
    uint8_t *rotbuf = calloc(1, buflen);
    uint8_t *outbuf = calloc(1, buflen);

    uint8_t key_value = 0;
    int value_counts[256] = {0};
    int max_count = 0;

    struct frame *frame;
    struct diff *diff;

    memset(key_diff_frame, 0, sizeof(struct key_diff_frame));

    rotate_flip(xbmgen_frame2_data, rotbuf);
    arrange_mem(rotbuf, outbuf, buflen);

    /*
     * Get key value (i.e. value with most occurrences) first.
     */
    for (i = 0; i < buflen; i++) {
        if (++value_counts[outbuf[i]] > max_count) {
            max_count = value_counts[outbuf[i]];
            key_value = outbuf[i];
        }
    }

    if (max_count < max_diff_benefit()) {
        /*
         * Key value is not the fully dominant value of this frame, and
         * the amount of resulting diffs will be more than the beneficial
         * threshold. No point to even continue here.
         */
        key_diff_frame->frame.diffcnt = frame_size() - max_count;

    } else {
        /*
         * Go on then. Calculate the diffs for each frame value
         */
        key_diff_frame->base_value = key_value;
        key_diff_frame->frame.diffcnt = 0;

        for (i = 0; i < buflen; i++) {
            if (outbuf[i] != key_value) {
                frame = &key_diff_frame->frame;
                frame_alloc(frame);
                diff = &frame->diffs[frame->diffcnt++];

                if (frame->diffcnt == 1 && i > 0xff) {
                    /*
                     * Here as well, first diff after address overflow,
                     * add dummy diff at 0xff to get the addressing right.
                     * See also get_diff_frame() function.
                     */
                    diff->addr = 0xff;
                    diff->data = outbuf[0xff];
                    frame_alloc(frame);
                    diff = &frame->diffs[frame->diffcnt++];
                }

                diff->addr = i;
                diff->data = outbuf[i];
            }
        }
    }

    free(rotbuf);
    free(outbuf);
}

/**
 * Stitch it all together.
 *
 * #defines used to determine operation mode and their origin:
 *      DIFF_FRAME        defined in xbmgen.h generated by xbmtool.sh
 *      LAST_DIFF_FRAME   given as gcc flag by xbmtool.sh
 *      MIXED_GRAPHICS    given as gcc flag by xbmtool.sh
 *
 *
 * Execution order by operation mode:
 *
 *  -f full frame graphics set:
 *      all frames:
 *      DIFF_FRAME=0  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=0
 *          1. Create full frame char array from xbm file, period.
 *
 *  -g mixed frame graphics set:
 *      all frames:
 *      DIFF_FRAME=0  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=1
 *          1  Create key diff frame information
 *          2a Create key diff frame struct if approach is beneficial
 *          2b Create full frame char array otherwise
 *
 *  -a one-shot animation:
 *      first frame:
 *      DIFF_FRAME=0  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=1
 *          see mixed frame graphics set
 *
 *      all other frames:
 *      DIFF_FRAME=1  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=0
 *          1  Create key diff frame information
 *          2  Create transition diff frame information
 *          3a Create transition diff struct if approach is more benficial
 *             than key diff frame, and is within max_diff_benefit
 *          3b Create key diff frame if approach is within max_diff_benefit
 *          3c Create full frame char array otherwise
 *
 *  -l looping animation:
 *      first frame:
 *      DIFF_FRAME=0  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=1
 *          see mixed frame graphics set
 *
 *      second to second-last frame:
 *      DIFF_FRAME=1  LAST_DIFF_FRAME=0  MIXED_GRAPHICS=0
 *          see one-shot animation
 *
 *      last frame:
 *      DIFF_FRAME=1  LAST_DIFF_FRAME=1  MIXED_GRAPHICS=0
 *          1  Create key diff frame information
 *          2  Create transition diff frame information
 *          3a Create transition diff struct if approach is most benficial
 *          3b Create nothing at all otherwise
 */
int
main(void)
{
    int ret;
#if defined DIFF_FRAME || defined MIXED_GRAPHICS
    struct key_diff_frame key_diff_frame;
#ifdef DIFF_FRAME
    struct frame frame;
#endif /* DIFF_FRAME */

    get_key_diff_frame(&key_diff_frame);
#ifdef DIFF_FRAME
    get_diff_frame(&frame);

    /*
     * Check if regular diff is a better choice than key diff, and if
     * a diff based approach is the best in the first place.
     * If so, go ahead with it and be done.
     */
    if (frame.diffcnt < key_diff_frame.frame.diffcnt
            && frame.diffcnt < max_diff_benefit())
    {
        print_diff_frame(&frame);
        ret = TYPE_DIFF;
#ifdef LAST_DIFF_FRAME
    } else if (1) {
        /*
         * Last frame in an animation that won't be using diffs, we got no
         * business here anymore. The first frame (which would be transitioned
         * to here) is anyway drawn from scratch again, either using key diff
         * or as full frame.
         */
        ret = TYPE_NONE;
#endif /* LAST_DIFF_FRAME */
    } else
#endif /* DIFF_FRAME */
    if (key_diff_frame.frame.diffcnt < max_diff_benefit()) {
        /*
         * Check if a key diff approach is the better choice here.
         * If it is, take that road and we're done.
         */
        print_key_diff_frame(&key_diff_frame);
        ret = TYPE_KEY_DIFF;

    } else {
#endif /* DIFF_FRAME || MIXED_GRAPHICS */
        /*
         * All else failed, we're best off drawing the full frame.
         * (also the only option for full frame graphics)
         */
        process_full_frame();
        ret = TYPE_FULL;
#if defined DIFF_FRAME || defined MIXED_GRAPHICS
    }

    free(key_diff_frame.frame.diffs);
#ifdef DIFF_FRAME
    free(frame.diffs);
#endif /* DIFF_FRAME */
#endif /* DIFF_FRAME || MIXED_GRAPHICS */

    return ret;
}

