#!/bin/bash
#
# 4chord MIDI - udev rule installation script
#
# Copyright (C) 2017 Sven Gregori <sven@craplab.fi>
#

UDEV_FILE="4chordMIDI-fluidsynth.rules"
UDEV_DIR="/etc/udev/rules.d"
SCRIPT_FILE="4chordMIDI-fluidsynth.sh"
SCRIPT_DIR="/usr/local/bin"


if [ $(whoami) != "root" ] || [ -z $SUDO_USER ] ; then
    echo "This needs to be run with sudo"
    exit 1
fi

cat << EOF
Welcome to the 4chord MIDI udev rules installation script

To set this up properly, a few additional information is needed.
If you don't know what to answer here or understand what is asked
for, just press the return key, it's probably going to be all fine.

Which user will be using 4chord MIDI?
EOF

read -p "Main user: [$SUDO_USER] " user
[ -z "$user" ] && user=$SUDO_USER

id -u $user >/dev/null 2>&1
if [ $? -ne 0 ] ; then
    echo "Sorry, user $user doesn't exist here."
    echo "Goodbye."
    exit 1
fi

cat << EOF
$user it is.

Next, udev rule files are usually prefixed with a number to have some
control over in which order different files are executed - basically
just in lexical order. Anything from 00 to 99 should do.
EOF

read -p "udev rule prefix number 00-99: [50] " prefix
[ -z "$prefix" ] && prefix="50"

if ! [[ "$prefix" =~ ^[0-9]{2}$ ]] ; then
    if [[ "$prefix" =~ ^[0-9]{1}$ ]] ; then
        prefix="0${prefix}"
    else
        echo "Invalid choice."
        echo "Goodbye."
        exit 1
    fi
fi

echo "Taking $prefix, okay."

script=$SCRIPT_DIR/$SCRIPT_FILE
udev_rule=$UDEV_DIR/${prefix}-${UDEV_FILE}

if [ -e $script ] ; then
    echo ""
    echo "Script file $script exists."
    read -p "Overwrite it? [n|Y] " choice
    [[ ${choice,,} == "y" ]] && rm -f $script
fi

if [ -e $udev_rule ] ; then
    echo ""
    echo "udev rule file $udev_rule exists."
    read -p "Overwrite it? [n|Y] " choice
    [[ ${choice,,} == "y" ]] && rm -f $udev_rule
fi

echo ""
echo "Copying script file"
cp $SCRIPT_FILE $SCRIPT_DIR
chmod +x $SCRIPT_DIR/$SCRIPT_FILE
echo "Creating udev rule file"
sed "s/@USER@/$user/g" $UDEV_FILE | sed "s|@SCRIPT@|$script|g" > $udev_rule
echo "Reloading udev rules"
udevadm control --reload-rules

echo ""
echo "All done!"
