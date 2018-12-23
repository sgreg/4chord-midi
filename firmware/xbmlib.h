/*
 * xbmlib Nokia 5110 LCD handling
 *
 * Copyright (C) 2018 Sven Gregori <sven@craplab.fi>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/
 *
 */
#ifndef XBMLIB_H
#define XBMLIB_H

#include <string.h>
#include <stdint.h>

/** struct xbmlib_frame full frame graphic type */
#define TYPE_FULL 0
/** struct xbmlib_frame diff frame graphic type */
#define TYPE_DIFF 1
/** struct xbmlib_frame key diff frame graphic type */
#define TYPE_KEY_DIFF 2

struct xbmlib_diff {
    uint8_t addr;
    uint8_t data;
};

struct xbmlib_diff_frame {
    uint8_t diffcnt;
    struct xbmlib_diff diffs[];
};

struct xbmlib_key_diff_frame {
    uint8_t base_value;
    uint8_t diffcnt;
    struct xbmlib_diff diffs[];
};

struct xbmlib_frame {
    uint8_t type;
    const void *data;
};

#endif /* XBMLIB_H */

