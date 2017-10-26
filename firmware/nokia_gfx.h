/* Automatically created by xbm2nokia.sh */
#ifndef _NOKIA_GFX_H_
#define _NOKIA_GFX_H_

#define NOKIA_GFX_ANIMATION

struct nokia_gfx_diff {
    uint16_t addr;
    uint8_t data;
};

struct nokia_gfx_frame {
    uint16_t delay;
    uint16_t diffcnt;
    struct nokia_gfx_diff diffs[];
};

#define NOKIA_GFX_FRAME_COUNT 12

extern const uint8_t nokia_gfx_keyframe[];
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_01_intro_02;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_02_intro_03;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_03_intro_04;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_04_intro_05;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_05_intro_06;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_06_intro_07;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_07_intro_08;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_08_intro_09;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_09_intro_10;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_10_intro_11;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_11_intro_12;
extern const struct nokia_gfx_frame nokia_gfx_trans_intro_12_intro_01;

#endif
