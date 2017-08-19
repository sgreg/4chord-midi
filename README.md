## 4chord MIDI - The Four Chord USB MIDI Keyboard

As we know latest since [The Axis of Awesome](https://en.wikipedia.org/wiki/The_Axis_of_Awesome), all it takes to make a hit song is [4 chords](https://www.youtube.com/watch?v=5pidokakU4I).
This project implements a USB MIDI device exactly for that purpose.

4chord MIDI can be used as a regular USB MIDI controller, just like any other MIDI keyboard, except it has just four buttons. It implements the I-V-vi-IV chord progression in any selectable key.
But since simply playing chords is a bit limiting, it also features different playback modes such as triad arpeggios and chords mixed with arpeggios. 
Additionally, the tempo of each note progression is selectable from 30bpm to 240bpm.

4chord MIDI is fully open. The hardware is licensed under the [CERN Open Hardware License v.1.2](http://www.ohwr.org/projects/cernohl/wiki) and the software is released under the [GNU General Public License version 2](http://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html).

See also [the 4chord MIDI project website](http://sgreg.fi/projects/4chord-midi) and [its hackaday.io project page](https://hackaday.io/project/26078-4chord-midi) for more details, prpgress information, pictures and everything else there is.


### Status

4chord MIDI includes custom hardware and firmware built around an [ATmega328P](http://www.atmel.com/devices/atmega328p.aspx) microcontroller and the [V-USB](https://www.obdev.at/products/vusb/index.html) library.
The first hardware revision A was created back in December 2015 as a simple, credit card sized PCB. In June 2017 the PCB was fully re-designed and shaped into a Piano, with Revision B officially finalized in August 2017 ([Release](https://github.com/sgreg/4chord-midi/releases/tag/v1.1-B)) and the PCB design [shared on OSH Park](https://oshpark.com/projects/GbBggrq3).


### Features

##### Hardware Revision B
* Piano shaped PCB layout
* ATmega328P TQFP32 microcontroller running at 12MHz
* Nokia 5110 clip-in LCD with PWM controllable back light
* USB powered with 3.3V main operating voltage
* Three button menu interface
* Four button chord playback
* UART interface for debug and testing with selectable supply voltage
* 6-pin ISP interface


##### Firmware Version 1.1
* MIDI message handling for note on/off commands for each playback key
* Hardware timer based tempo handling for arpeggio and mixed mode playback
* Graphical user interface to display current playback key, mode and tempo
* UART command line interface to emulate button press (38400 8N1)
* USB and MIDI message handling via V-USB
* Dedicated USB VID/PID pair thanks to [pid.codes](http://pid.codes/)

##### Tools
* XBM images to C char arrays conversion tool
* ATmega328P fuse dump tool to output human readable fuse settings


### Usage

First off, all I can tell about here refers to Linux. But since 4chord MIDI implements standard USB and MIDI specifications, other operating systems should treat it also as a normal MIDI device, so any general MIDI setup instructions should work just fine.

I usually use [jack](http://www.jackaudio.org/) for audio routing and [Ardour](http://ardour.org/) for recording in general, and [a2jmidid](http://manual.ardour.org/setting-up-your-system/setting-up-midi/midi-on-linux/), [Calf plugins](http://calf-studio-gear.org/) and [Fluidsynth](http://www.fluidsynth.org/) sound fonts for MIDI in particular. I never used [Rosegarden](http://www.rosegardenmusic.com/), [MusE](http://muse-sequencer.org/) or anything alike, but then again, 4chord MIDI is a regular USB MIDI device, so it should work as any other MIDI controller.

##### Plug it in

Connect 4chord MIDI to your computer and check if it got recognized. Since it uses the V-USB library for USB handling, `dmesg` should show something like this:
```
[3038097.883853] usb 1-1.1.1: new low-speed USB device number 112 using ehci-pci
[3038099.140623] usb 1-1.1.1: New USB device found, idVendor=1209, idProduct=deaf
[3038099.140626] usb 1-1.1.1: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[3038099.140627] usb 1-1.1.1: Product: 4chord MIDI
[3038099.140628] usb 1-1.1.1: Manufacturer: CrapLab
```
and `lsusb` should show something like this:
```
Bus 001 Device 112: ID 1209:deaf InterBiometrics CrapLab 4chord MIDI
```
or, more likely, simply
```
Bus 001 Device 112: ID 1209:deaf InterBiometrics CrapLab 4chord MIDI
```
Of course, the bus and device number will most certainly differ, but as long as the VID/PID pair (`1209:deaf`) is the same, it's all good.

##### Simple playback

Before going too deep into home recording in Linux etc, simple playback can be achieved with ALSA as well.
For Ubuntu, the required packages are `alsa-tools fluidsynth fluidsynth-soundfont-gm`, for any other distribution ..well, I guess they should have similar names?

First, start fluidsynth as server in one terminal:

```
$ fluidsynth -a alsa -m alsa_seq -s -g 1.0 /usr/share/sounds/sf2/FluidR3_GM.sf2
```
If it complains about the soundfont path, check where they are installed to and adjust the path.

Then, in a second terminal, check for available MIDI input and output ports using `aconnect`, which should output something like this:

```
$ aconnect -i
client 20: '4chord MIDI' [type=kernel]
    0 '4chord MIDI MIDI 1'

$ aconnect -o
client 129: 'FLUID Synth (32590)' [type=user]
    0 'Synth input port (32590:0)'
```

This tells us MIDI input is port `20:0` and MIDI output is port `129:0`. Now we connect them.

```
$ aconnect 20:0 129:0
```

That's it. You should hear now a piano sound when pressing any of the four chord buttons.


**Note:** while testing this, I did encounter some noise (and once what sounded more like a dead robot than a piano). It's possible some ALSA tweaking can take care of that, but anyway, I highly recommend to have a look into [jack](http://www.jackaudio.org/) if you're serious about making music in Linux.

