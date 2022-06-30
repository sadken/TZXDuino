## TZXDuino

(Stripwax)
https://github.com/stripwax/TZXDuino is a fork of the original github repo (see https://github.com/sadken/TZXDuino), containing the latest source from https://www.facebook.com/Arduitape and my own changes.

Versions and releases are done using tags, unlike the original versions where file names change for each release.  With this model, file names won't change, merging and diffing is easier, etc.

I aim to improve this over time, adding more features and improvements, and to keep it up-to-date with any other official changes, and that includes requesting these being pulled into the original github repo too.

I'll try to keep this up-to-date and would like to encourage other developers to fork/contribute .
I'll also maintain other branches occasionally, for experiments or divergent ideas.

For my SAMD (Seeeduino Xiao M0) support specifically, please see [README_seeeduino_xiao_m0.md](README_seeeduino_xiao_m0.md)

## Recent feature additions
* improved directory navigation performance (many more directories are supported, up/down latency is better)
* flicker-free text updates on OLED1306
* USB Mass Storage (your files from your SD card are browseable when device is plugged into USB)
* reduce firmware size of Menu system
* improved "scrollText" routine for scrolling long filenames that don't fit on the screen
* support for ADC-style button interface
* fixed annoying buzzing at end-of-file
* SAMD XIAO M0 fully supported (custom timer code, pause handling)

## Credits
* Original Code and Testing: Andrew Beer & Duncan Edwards
* Additional support, ideas, and testing: Duncan Edwards
