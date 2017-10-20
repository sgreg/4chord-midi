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

import os
import sys
import math
import struct
import usb.core
import usb.util

# USB device information
USB_VID = 0x1209
USB_PID = 0xdeaf
USB_DEVICE_CLASS = 0xff

# USB request types
USB_SEND = 0x40
USB_RECV = 0xC0

# Bootloader commands
CMD_HELLO   = 0x55
CMD_INFO    = 0x10
CMD_MEMPAGE = 0x20
CMD_VERIFY  = 0x30
CMD_BYE     = 0xaa

# parameters for CMD_HELLO (it's just ASCII for the Finnish greeting "Moi!")
HELLO_VALUE = 0x4d71
HELLO_INDEX = 0x6921

PAGESIZE = 128

verbose = False

if len(sys.argv) == 2:
    binfile = sys.argv[1]
elif len(sys.argv) == 3 and sys.argv[1] == '-v':
    verbose = True
    binfile = sys.argv[2]
else:
    print 'Usage: ' + sys.argv[0] + ' /path/to/firmware.bin'
    sys.exit(1)

if verbose:
    print 'Using file ' + binfile

try:
    size = os.stat(binfile).st_size
    f = open(binfile, 'rb')
except (IOError, OSError) as e:
    print 'Cannot open firmware file: %s' % (e,)
    sys.exit(1)

# find device
dev = usb.core.find(idVendor=USB_VID, idProduct=USB_PID)

if dev is None:
    print 'Cannot find any 4chord MIDI device'
    sys.exit(1)

if dev.bDeviceClass != USB_DEVICE_CLASS:
    print 'Found 4chord MIDI device, but not as bootloader.'
    print ''
    print 'Please make sure that you have the bootloader running by pressing'
    print 'the "Select" menu button when powering up the device. You should'
    print 'see "4chord MIDI Bootloader" displayed on the LCD in that case.'
    print ''
    sys.exit(1)

# alright, looks like we're all set up and everything is as it should be

total_pages = int(math.ceil(size / float(PAGESIZE)))

print '4chord MIDI bootloader flashing tool'
print ''
print 'File size: %d bytes' % (size, )
print 'Page size: %d bytes' % (PAGESIZE,)
print '-> %d memory pages to write' % (total_pages,)
print ''

if verbose:
    print '<- [HELLO]'
ret = dev.ctrl_transfer(USB_RECV, CMD_HELLO, HELLO_VALUE, HELLO_INDEX, PAGESIZE)

if verbose:
    print '-> [HELLO]'

print 'Device: ' + ret.tostring()

#
# XXX: if needed later, here is a good place to add version checks and adjust
#      behaviour accordingly. For example, if later on the bootloader formware
#      gets additional functionality, here could be checked if that additional
#      functionality is supported, and fall back to old capabilities otherwise
#

if verbose:
    print '<- [INFO]'
else:
    print ''
dev.ctrl_transfer(USB_SEND, CMD_INFO, total_pages, 0)

total_retries = 0
max_retries = 0
max_retry_page = 0

page_number = 0
while True:
    page_data = f.read(PAGESIZE)
    if len(page_data) == 0:
        break;

    page_number += 1
    size = len(page_data)

    page_header = struct.pack('BB', page_number, size);
    data = page_header + page_data

    retry = True
    retry_count = 0

    while retry:
        retry_count += 1
        # send page
        if verbose:
            print '<- [MEMPAGE] page %3d, %3d bytes, attempt %d, %s...' \
                    % (page_number, size, retry_count, \
                    "".join("{:02x}".format(ord(c)) for c in page_data[:10]))
        else:
            sys.stdout.write('\rSending page %d/%d' % (page_number, total_pages))
            sys.stdout.flush()
        dev.ctrl_transfer(USB_SEND, CMD_MEMPAGE, 0, 0, data)

        # read back last page
        ret = dev.ctrl_transfer(USB_RECV, CMD_VERIFY, 0, 0, PAGESIZE)

        # verify byte by byte
        all_good = True
        for i in range(len(page_data)):
            if ord(page_data[i]) != ret[i]:
                all_good = False
                break
        retry = not all_good

        if verbose:
            if retry:
                total_retries += 1
                print '-> [VERIFY]  mismatch at index ' + str(i)
            else:
                if retry_count > max_retries:
                    max_retries = retry_count
                    max_retry_page = page_number
                print '-> [VERIFY]  all good'

if verbose:
    print '<- [BYE]'
dev.ctrl_transfer(USB_SEND, CMD_BYE, 0, 0)

print ''
print 'Firmware updated!'
if verbose:
    print 'Total number of retries: ' + str(total_retries)
    print 'Maximum number of retries: %d (#%d)' % (max_retries, max_retry_page)
print ''
print 'All done.'

