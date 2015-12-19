## 4chord MIDI graphics sources for Nokia 3310 LCD
The 4chord MIDI GUI images are licensed under a [Creative Commons Attributions 4.0 International License](http://creativecommons.org/licenses/by/4.0/).

In case you want to create your own graphics, make sure of the following steps:

* use the same size for each category if you don't want to adjust the code (otherwise have a look at [firmware/lcd3310.c](https://github.com/sgreg/4chord-midi/blob/master/firmware/lcd3310.c)
* due to the way the LCD memory mapping works, you have to 
    1. rotate the image 90 degree counter clockwise
    2. flip the image vertically afterwards
* make sure the XBM file prefix is the same than the basename of the image file


### Picture category sizes

* Menu: 8x84 px
* Key: 32x32 px
* Key modifier: 16x16px
* Tempo digit: 16x8px
* Mode: 16x32px


### Fonts used in the original images

* Menu: Latin Modern Sans Oblique 8
* Key: Latin Modern Sans Quotation Oblique 36
* Key modifier sharp: Latin Modern Sans Quotation Oblique 16
* Key modifier flat: Latin Modern Sans Quotation Oblique 19
* Tempo: Ubuntu Mono Bold 20

