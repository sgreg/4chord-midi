#!/bin/bash
#
# 4chord MIDI - udev rule installation script
#
# Copyright (C) 2019 Sven Gregori <sven@craplab.fi>
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
    read -p "Overwrite it? [n/Y] " choice
    if [ -n "$choice" ] && [[ ${choice,,} != "y" ]] ; then
        echo "Check the SCRIPT_DIR and SCRIPT_FILE variables to change the path"
        echo ""
        echo "Aborting"
        exit 1
    else
        echo ""
        echo "Overwriting script file."
    fi
fi

if [ -e $udev_rule ] ; then
    echo ""
    echo "udev rule file $udev_rule exists."
    read -p "Overwrite it? [n/Y] " choice
    if [ -n "$choice" ] && [[ ${choice,,} != "y" ]] ; then
        echo "Check the UDEV_DIR and UDEV_FILE variables to change the path"
        echo ""
        echo "Aborting"
        exit 1
    else
        echo ""
        echo "Overwriting udev rule."
    fi
fi

similar_rules=$(ls -1 $UDEV_DIR/*-${UDEV_FILE} 2>/dev/null | grep -v $udev_rule)
if [ -n "$similar_rules" ] ; then
    echo ""
    echo "Found possible duplicate rule files:"
    for rule in $similar_rules ; do
        echo "    - $rule"
    done
    echo ""
    echo "This might prevent the udev rules from properly functioning or"
    echo "they might interfere with each other.  They could be leftovers"
    echo "from running this script before,  in which case it is probably"
    echo "safe to delete them.  It's recommended you have a look at them"
    echo "and delete or modify them as needed."
    echo ""
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
echo "All done"
echo ""

