## 4chord MIDI schematic and PCB layout files

**Note:** All content was designed using KiCad 4. Some files might not work with older versions of KiCad (actually, I haven't tried that). Schematics are additionally provided in PDF and PCB layouts in Gerber format.

### Subdirectory content:

- Atmega328-TQFP32:
    - SMD version
    - 32pin TQFP ATmega328
    - Nokia 3310 LCD


#### Notes on the Nokia 3310 LCD
There are tons of sources for the 3310 LCD, I don't know how compatible they are with each other.
I purchased mine from [Olimex](https://www.olimex.com/Products/Components/LCD/LCD%20DISPLAY%20NOKIA3310/), along with the matching [connector](https://www.olimex.com/Products/Components/Connectors/FPA-WZA201-08-LF/)
and [backlight](https://www.olimex.com/Products/Components/LCD/LCD%20DISPLAY%20NOKIA3310-BACKLIGHT/).
Note, they also offer a [breakout board](https://www.olimex.com/Products/Modules/LCD/MOD-LCD3310/open-source-hardware) which comes in handy during prototyping.

It seems KiCad doesn't have a footprint for the connector out of the box, so I created one myself, which you'll need to import to KiCad if you want to modify the hardware.
Check my [kicad-libs repo](https://github.com/sgreg/kicad-libs) for more information.

#### Notes on PCB artwork
As with the LCD connector footprints, the logos used on the PCB can be found in my [kicad-libs repo](https://github.com/sgreg/kicad-libs).
