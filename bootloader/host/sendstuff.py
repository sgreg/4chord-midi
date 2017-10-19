#!/usr/bin/python
#
# 4chord MIDI bootloader - Host side flash tool
#
# Copyright (C) 2017 Sven Gregori <sven@craplab.fi>
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

#
# TODO: cleanup .. very much cleanup.
#
#   - add constants for commands and paramter
#   - get the output neater
#   - add better checks to discover device, such as device class
#

import os
import sys
import math
import struct
import usb.core
import usb.util

if sys.argv < 2:
    print "Usage: " + sys.argv[0] + " /path/to/firmware.bin"
    sys.exit(1)

binfile = sys.argv[1]

# find device
dev = usb.core.find(idVendor=0x1209, idProduct=0xb00b)

if dev is None:
    raise ValueError('Cannot find 4chord MIDI device')

try:
    size = os.stat(binfile).st_size
    f = open(binfile, 'rb')
except (IOError, OSError) as e:
    print "Nope, no such file " + binfile
    sys.exit(1)

CHUNKSIZE = 128
print size
pages = int(math.ceil(size / float(CHUNKSIZE)))
print "pages: " + str(pages)

ret = dev.ctrl_transfer(0xc0, 0x55, 0x4d71, 0x6921, CHUNKSIZE)
print ret.tostring()
ret = dev.ctrl_transfer(0x40, 0x10, pages, 0)

page = 0
while True:
    chunk = f.read(CHUNKSIZE)
    if len(chunk) == 0:
        break;

    page += 1
    size = len(chunk)

    header = struct.pack('BB', page, size);
    data = header + chunk

    retry = True

    while retry:
        # send page
        print "sending page %d with %3d bytes: %s..." % (page, size, "".join("{:02x}".format(ord(c)) for c in chunk[:10]))
        dev.ctrl_transfer(0x40, 0x20, 0, 0, data, 1000)

        # read back last page
        ret = dev.ctrl_transfer(0xc0, 0x30, 0, 0, CHUNKSIZE)

        # verify byte by byte
        all_good = True
        for i in range(len(chunk)):
            if ord(chunk[i]) != ret[i]:
                print "mismatch at index " + str(i) + " " + str(ord(chunk[i])) + " vs " + str(ret[i])
                all_good = False
                break
        retry = not all_good

dev.ctrl_transfer(0x40, 0xaa, 0, 0)
print "done"

