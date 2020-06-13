## 4chord MIDI udev rule example

Just some proof of concept to have a script run whenever a 4chord MIDI device is plugged in and unplugged. In this case, it will automatically setup the MIDI connection to fluidsynth when the device is plugged in. If fluidsynth isn't running, it will start it. When the device is unplugged, everything is nicely cleaned up again.

In addition, another rule will set up the permission for the device so any user can flash it via the bootloader.

There are a few files to get this done:

* `4chordMIDI-bootflash.rules` the udev rule file to set up the permission
* `4chordMIDI-fluidsynth.sh` the actual script taking care of the whole fluidsynth and MIDI connection setup handling
* `4chordMIDI-fluidsynth.rules` the udev rule template that will actually handle the script execution
* `install.sh` installation script to set it all up

The udev rule will executed by root while the whole fluidsynth and MIDI setup needs to be run by a regular user, so the script needs to be executed using `su - <user>` to make it work properly. The user name needs to be hard coded to the udev rule file, so to avoid doing this manually, I added the `install.sh` script. It also lets you choose the udev rule file prefix.

### Known Issues

The udev rule doesn't seem to work too well with (at least) Arch Linux. While the `4chordMIDI-fluidsynth.sh` script gets executed, and fluidsynth is trying to start, it doesn't succeed due to some underlything ALSA issues. Possibly just some configuration issue but haven't gotten around to finding a solution yet.
 - **Workaround 1**: have the udev rule still in place, but start fluidsynth manually *before* plugging in the 4chord MIDI device
 - **Workaround 2**: ditch the whole udev part and run `4chordMIDI-fluidsynth.sh` manually *after* plugging in the device
