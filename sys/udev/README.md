## 4chord MIDI udev rule example

Just some proof of concept to have a script run whenever a 4chord MIDI device is plugged in and unplugged. In this case, it will automatically setup the MIDI connection to fluidsynth when the device is plugged in. If fluidsynth isn't running, it will start it. When the device is unplugged, everything is nicely cleaned up again.

There are three files to get this done:

* `4chordMIDI-fluidsynth.sh` the actual script taking care of the whole fluidsynth and MIDI connection setup handling
* `4chordMIDI-fluidsynth.rules` the udev rule template that will actually handle the script execution
* `install.sh` installation script to set it all up

The udev rule will executed by root while the whole fluidsynth and MIDI setup needs to be run by a regular user, so the script needs to be executed using `su - <user>` to make it work properly. The user name needs to be hard coded to the udev rule file, so to avoid doing this manually, I added the `install.sh` script. It also lets you choose the udev rule file prefix.

