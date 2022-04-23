Inspired by JamHamster's TZX Cassette  https://github.com/jamhamster/TZX-Cassette-Mod (specifically the 0.2 variant here: https://github.com/jamhamster/TZX-Cassette-Mod/blob/master/TZXCassette%20Mod%20v0.2.pdf ), I thought I'd try out something more ambitious.  To do that, I realised I would need a much smaller device platform than the Arduino Nano.
I did a laughably small amount of research and decided that Seeeduino Xiao M0 would be ideal because

1. It is very small
2. It has 3v3 operation which I figured could enable use of smaller simpler peripherals (SD reader, for example)
3. It has a higher clock speed
4. It is 'Arduino Compatible'

This is my first Arduino project - I know people probably usually pick an easy one like a blinking LED but where's the fun in that.

My eventual goal is to have something that

1. Fits in a cassette tape
2. Interfaces with ZX Spectrum +2 tape player via a playback head
3. Detects the motion of the spindles for play, stop, also fastforward and rewind (to skip TZX blocks)
4. (stretch goal) Supports recording direct to SD device
5. (stretch goal) Hosts a ZX Spectrum binary that can be downloaded to the spectrum to run a native file chooser menu on the ZX Spectrum for 'hands free' usage.

Not all of those goals are necessarily sensible or even useful, so for now I'm just focusing on the first two with a part of a plan for items 3 and 4.  It's clear that I would need a large number of GPIOs to achieve this.

Three main things stand out for this device, now that I've spent more time with it.

1. It has really very few GPIOs (only 11 GPIO pins - it only has 14 pins in total)
2. You can't disable the onboard DC-DC convertor for 5v to 3v3 which makes interfacing with a battery a bit fiddly
3. The library support is not quite as good (I started using SAMD_TimerInterrupt but it misbehaves - I'm working with the maintainer of that library to help resolve but for now my fixes are in my TZXDuino branch)

Since it has very few GPIOs, one "quick win" is to put all the buttons onto a single GPIO using ADC with analogue input.  A resistor network is setup as a potential divider such that pressing each button presents a different voltage ratio to the input pin.  This voltage can then easily be read in software to determine which button is pressed.
Each button is implemented as Vcc -> RESISTOR -> BUTTON -> ADC PIN , and the ADC_PIN is also connected to ground via 10k resistor.  The analogue input is pulled down to ground, or pulled up to a ratio of Vcc when a corresponding button is pressed.
I've mapped this to PIN A3, and I've chosen my resistors to be 0Ohm, 1K, 2.4K, 10K, 20K (divided from 10K) , meaning the ratios are
* Play button: Vcc
* Stop button: 90% Vcc
* Root (Menu) button: 80% Vcc
* Down button: 50% Vcc
* Up button: 33% Vcc

