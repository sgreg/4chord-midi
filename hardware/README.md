# 4chord MIDI schematic and PCB layout files

**Note:** All content was designed using KiCad 4. This should be updated to KiCad 5 eventually. Schematics are additionally provided in PDF and PCB layouts in Gerber format.

## Subdirectory content:

- Atmega328-TQFP32:
    - SMD version
    - 32pin TQFP ATmega328
    - Nokia 5110 clip-in LCD


## Notes on the Nokia 5110 LCD
There are several options to source the LCD with its clip-in frame, the most common one being the breakout board version of it. In some cases you might find the standalone LCD without breakout board.

KiCad didn't have footprints for it, so I created one that is available in my [kicad-libs repo](https://github.com/sgreg/kicad-libs), with more information available [on its dedicated hackady.io project page](https://hackaday.io/project/25666-yet-another-nokia-5110-lcd-breakout-board).


## Notes on PCB artwork
As with the LCD connector footprints, the logos used on the PCB can be found in my [kicad-libs repo](https://github.com/sgreg/kicad-libs).


## License
The 4chord MIDI documentation is released as Open Hardware under the CERN OHL v.1.2.

You may redistribute and modify this documentation under the terms of the [CERN OHL v.1.2](http://ohwr.org/cernohl).
This documentation is distributed WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A PARTICULAR PURPOSE. Please see the CERN OHL v.1.2 for applicable conditions
