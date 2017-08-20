#!/bin/bash
#
# 4chord MIDI - automatic fluidsynth setup script
#
# Copyright (C) 2017 Sven Gregori <sven@craplab.fi>
#
# This script is supposed to be called by udev when plugging in and
# out a 4chord MIDI device. It will check if fluidsynth is running,
# start it if not, and trying to automatically connect 4chord MIDI
# to it. When unplugging, it will run the script again and kill the
# fluidsynth if it has been started by udev before.
#
# Yes, all the output is rather useless since it won't show anywhere
# when called from udev, but it's quite handy during development.
#


# Path setup for all required executables and sound font files.
# Obviously, you need them installed to make this work, and depending
# on yout Linux distribution, the paths might need some adjustments.

# /path/to/aconnect
ACONNECT="/usr/bin/aconnect"

# /path/to/fluidsynth
FLUIDSYNTH="/usr/bin/fluidsynth"

# /path/to/soundfonts
SOUNDFONTS="/usr/share/sounds/sf2/FluidR3_GM.sf2"

# Path to PID file of fluidsynth process started from within this script.
# Ideally this would reside in /var/run but since this script is executed
# as normal user, it would need some additional permission setup before,
# but to keep things simple for now, /tmp is used.
PIDFILE="/tmp/fooscript.pid"


# start - 4chord MIDI device was plugged in
if [ "$1" == "start" ] ; then
    # make sure all required executables and other files exist
    if [ ! -f $ACONNECT ] ; then
        echo "cannot find aconnect executable"
        exit 1;
    fi

    if [ ! -f $FLUIDSYNTH ] ; then
        echo "cannot find fluidsynth executable"
        exit 1
    fi

    if [ ! -f $SOUNDFONTS ] ; then
        echo "cannot find sound fonts file"
        exit 1
    fi

    # check if there's already fluidsynth available, start it otherwise
    $ACONNECT -o |grep -q "FLUID Synth"
    if [ $? -ne 0 ] ; then
        echo fluidsynth not running, starting it
        $FLUIDSYNTH -a alsa -m alsa_seq -si -g 1.0 $SOUNDFONTS >/dev/null 2>&1 &
        if [ $? -ne 0 ] ; then
            echo "failed"
            exit 1;
        fi
        # write fluidsynth PID to file
        echo $! > $PIDFILE
        echo "written PID $(cat $PIDFILE) to $PIDFILE"
    fi

    # wait a little bit
    sleep 1

    # get 4chord MIDI and fluidsynth port numbers
    in=$($ACONNECT -i |sed -n "s/^client \([0-9]*\): '4chord MIDI\(.*\)/\1/p")
    out=$($ACONNECT -o |sed -n "s/^client \([0-9]*\): 'FLUID Synth\(.*\)/\1/p")

    # make sure both ports exists
    if [ -z "$in" ] || [ -z "$out" ] ; then
        echo "something went wrong"
        exit 1
    fi

    # connect the ports and be happily done
    $ACONNECT $in $out

    exit $?

# stop - 4chord MIDI was unplugged
elif [ "$1" == "stop" ] ; then
    # check if there's a PID file from previous start call
    if [ ! -e $PIDFILE ] ; then
        echo "no pid file, never mind then"
        exit 0
    fi

    # get PID from PID file
    pid=$(cat $PIDFILE)
    echo "found PID $pid"
    # check it's really still a fluidsynth process, and kill it if it is
    if [ -d /proc/$pid ] && [ "$(readlink -f /proc/$pid/exe)" == "$FLUIDSYNTH" ] ; then
        echo "killing fluidsynth"
        kill -9 $pid
    else
        echo "whoops, found PID isn't a fluidsynth process"
    fi

    # clean up the PID file
    rm -f $PIDFILE

    exit 0

# neither start nor stop, show usage then
else
    echo "usage: $0 start"
    echo "       $0 stop"
    exit 1
fi
