#!/usr/bin/env python3
#
# 4chord MIDI bootloader - Host side flash tool
#
# Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
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

# USB device information
USB_VID = 0x1209
USB_PID = 0xdeaf
USB_DEVICE_CLASS = 0xff

# USB request types
USB_SEND = 0x40
USB_RECV = 0xC0

# Bootloader commands
CMD_HELLO               = 0x01
CMD_FWUPDATE_INIT       = 0x10
CMD_FWUPDATE_MEMPAGE    = 0x11
CMD_FWUPDATE_VERIFY     = 0x12
CMD_FWUPDATE_FINALIZE   = 0x13
CMD_BYE                 = 0xf0

#
#             (\.   \      ,/)
#              \(   |\     )/
#    Here      //\  | \   /\\
#     be      (/ /\_#oo#_/\ \)
#   wizards    \/\  ####  /\/
#                   `##'
#
# Magic numbers expected as CMD_HELLO value and index
# parameter to get things going in the bootloader.
# (ASCII for "Moi!" which simply translates to "Hi!")
HELLO_VALUE = 0x4d71
HELLO_INDEX = 0x6921

# Page size in bytes
PAGESIZE = 128

verbose = False


# check command line arguments
if len(sys.argv) == 2:
    binfile = sys.argv[1]
elif len(sys.argv) == 3 and sys.argv[1] == '-v':
    verbose = True
    binfile = sys.argv[2]
else:
    print('''bootflash - flash the 4chord MIDI firmware via its bootloader.
Usage: {0} [-v] /path/to/firmware.bin

Make sure the bootloader is active, and the .bin firmware file is used.
To activate the bootloader in the first place, press the "Select" button
and keep it pressed while plugging in the USB cable. The .bin firmware
file is built automatically as part of the firmware build itself.

Options:
    -v      add some verbose output
'''.format(sys.argv[0]))
    sys.exit(1)

# open firmware file
try:
    size = os.stat(binfile).st_size
    f = open(binfile, 'rb')
except (IOError, OSError) as e:
    print('Cannot open firmware file: {0}'.format(e))
    sys.exit(1)

# find device
dev = usb.core.find(idVendor=USB_VID, idProduct=USB_PID)

if dev is None:
    print('''Error: No device found

Please make sure that you have a 4chord MIDI device connected and
that its bootloader is active. To activate the bootloader, keep
the "Select" button pressed when plugging in the USB cable.
''')
    sys.exit(1)

if dev.bDeviceClass != USB_DEVICE_CLASS:
    print('''Error: Found 4chord MIDI device, but no bootloader

To activate the bootloader:
    - unplug the USB cable
    - press the "Select" button
    - while keeping the button pressed, plug the USB cable back in

You should now see the "4chord MIDI Bootloader" message on the display,
and you are ready to give it another try.
''')
    sys.exit(1)

# alright, looks like we're all set up and everything is as it should be

total_pages = int(math.ceil(size / float(PAGESIZE)))

# send HELLO and receive bootloader versions tring
hello = dev.ctrl_transfer(USB_RECV, CMD_HELLO, HELLO_VALUE, HELLO_INDEX, PAGESIZE)

#
# XXX: if needed later, here is a good place to add version checks and adjust
#      behaviour accordingly. For example, if later on the bootloader firmware
#      gets additional functionality, here could be checked if that additional
#      functionality is supported, and fall back to old capabilities otherwise
#

print("""

  d8b                                 ,d8888b d8b                    d8b
  ?88                         d8P     88P'    88P                    ?88
   88b                     d888CRAPLAB8888P  d88                      88b
   888888b  d8888b  d8888b   ?88'    ?88'    888   d888b8b   .d888b,  888888b
   88P `?8bd8P' ?88d8P' ?88  88P     88P     ?88  d8P' ?88   ?8b,     88P `?8b
  d88,  d8888b  d8888b  d88  88b    d88       88b 88b  ,88b    `?8b  d88   88P
 d88'`?88P'`?8888P'`?8888P'  `?8b  d88'        88b`?88P'`88b`?888P' d88'   88b


   .:.:...:.:.:...:.:...:.:.:..  4chord MIDI  ..:.:.:...:.:...:.:.:...:.:.

            Firmware....: {binfile:s}
            File size...: {size:d} bytes
            Page size...: {pagesize:d} bytes
            Pages.......: {pages:d}
            Bootloader..: {device:s}""".format(
                binfile=binfile, size=size,
                pagesize=PAGESIZE, pages=total_pages,
                device=hello.tostring().decode('UTF-8')))

# send INFO with number of pages to write
dev.ctrl_transfer(USB_SEND, CMD_FWUPDATE_INIT, total_pages, 0)

total_retries = 0
max_retries = 0
max_retry_page = 0

page_number = 0

if verbose:
    print('            Writing.....: verbose mode')
while True:
    page_data = f.read(PAGESIZE)
    size = len(page_data)

    if size == 0:
        break;

    page_number += 1

    page_header = struct.pack('BB', page_number, size);
    data = page_header + page_data

    retry = True
    retry_count = 0

    while retry:
        retry_count += 1
        # send page
        if verbose:
            sys.stdout.write('\r                          page {:3d} #{:d}: {:3d} bytes [{:s}...]'
                    .format(page_number, retry_count, size, "".join("{:02x}".format(c) for c in page_data[:10])))
        else:
            sys.stdout.write('\r            Writing.....: {:d}/{:d}'.format(page_number, total_pages))

        sys.stdout.flush()
        dev.ctrl_transfer(USB_SEND, CMD_FWUPDATE_MEMPAGE, 0, 0, data)

        # read back last page
        ret = dev.ctrl_transfer(USB_RECV, CMD_FWUPDATE_VERIFY, 0, 0, PAGESIZE)

        # verify byte by byte
        all_good = True
        for i in range(len(page_data)):
            if page_data[i] != ret[i]:
                all_good = False
                break

        retry = not all_good

        if verbose:
            if retry:
                total_retries += 1
            else:
                if retry_count > max_retries:
                    max_retries = retry_count
                    max_retry_page = page_number
                print('')

# finalize firmware update
dev.ctrl_transfer(USB_SEND, CMD_FWUPDATE_FINALIZE, 0, 0)

# gracefully end with a BYE
dev.ctrl_transfer(USB_SEND, CMD_BYE, 0, 0)

if verbose:
    print('            Retries.....: {0:d}'.format(total_retries))
    print('            Max retries.: {0:d} (#{1:d})'.format(max_retries, max_retry_page))
else:
    print('')
print('''
            All Done \o/
''')

