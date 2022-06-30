Inspired by JamHamster's TZX Cassette  https://github.com/jamhamster/TZX-Cassette-Mod (specifically the 0.2 variant here: https://github.com/jamhamster/TZX-Cassette-Mod/blob/master/TZXCassette%20Mod%20v0.2.pdf ), I thought I'd try out something more ambitious.  To do that, I realised I would need a much smaller device platform than the Arduino Nano.

I did a laughably small amount of research and decided that Seeeduino Xiao M0 would be ideal because:

1. It is very small
2. It has 3v3 operation which I figured could enable use of smaller simpler peripherals (SD reader, for example)
3. It has a higher clock speed
4. It is 'Arduino Compatible'
5. It is cheap. Incredibly cheap.

This is my first Arduino project - I know people probably usually pick an easy one like a blinking LED but where's the fun in that.

My eventual goal is to have something that:

1. Fits in a cassette tape
2. Interfaces with ZX Spectrum +2 tape player via a playback head
3. Detects the motion of the spindles for play, stop, also fastforward and rewind (to skip TZX blocks)
4. (stretch goal) Support USB mass storage so files can be copied/etc without removing SD Card.  DONE!
5. (stretch goal) Supports recording direct to SD device
6. (stretch goal) Hosts a ZX Spectrum binary that can be downloaded to the spectrum to run a native file chooser menu on the ZX Spectrum for 'hands free' usage.

Not all of those goals are necessarily sensible or even useful, so for now I'm just focusing on the first two with a part of a plan for items 3 and 4.  It's clear that I would need a large number of GPIOs to achieve this.

Three main things stand out for this device, now that I've spent more time with it:

1. It has really very few GPIOs (only 11 GPIO pins - it only has 14 pins in total)
2. You can't disable the onboard DC-DC convertor for 5v to 3v3 which makes interfacing with a battery a bit fiddly
3. The library support is not quite as good (I started using SAMD_TimerInterrupt but it misbehaves - I'm working with the maintainer of that library to help resolve but for now my fixes are in my TZXDuino branch)

Since it has very few GPIOs, one "quick win" is to put all the buttons onto a single GPIO using ADC with analogue input.  A resistor network is setup as a potential divider such that pressing each button presents a different voltage ratio to the input pin.  This voltage can then easily be read in software to determine which button is pressed.

Each button is implemented as Vcc -> RESISTOR -> BUTTON -> ADC PIN , and the ADC_PIN is also connected to ground via 10k resistor.  The analogue input is pulled down to ground, or pulled up to a ratio of Vcc when a corresponding button is pressed.

I've mapped this to PIN A3, and I've chosen my resistors to be 0Ohm, 1K, 2.4K, 10K, 20K (divided from 10K) , meaning the ratios are:

* Play button: Vcc
* Stop button: 90% Vcc
* Root (Menu) button: 80% Vcc
* Down button: 50% Vcc
* Up button: 33% Vcc


# Peripherals and connections

Subject to change, but trying to squeeze everything into the available GPIOs:

0.  Audio Output (hardwire to cassette head or jack) (might also be used for audio input)
1.  RESERVED for play/ffwd/rwd detecting enhancemenet (reflectometer 1 strobe out (play/ffwd spindle)) 
2.  RESERVED for play/ffwd/rwd detecting enhancemenet (reflectometer 2 strobe out (rwd spindle))
3.  ADC for buttons
4.  RESERVED for reflectometer 1/2 return
5.  SCK I2C for OLED
6.  SDA I2C for OLED
7.  SPARE (but might be required for audio input circuitry)
8.  CLK for SD CARD (SPI)
9.  MISO for SD CARD (SPI)
10.  MOSI for SD CARD (SPI)
11.  3v3 (in from external LiPo DC stepdown; out to peripherals)
12.  GND
13.  5v (present when USB connected; out to LiPo charger TP4056)

Previously I had tied one of the GPIOs to the SD CARD's CS (chip select) line, but it seems to work fine without this, so long as (1) I tie the SD CARD CS to Ground, and (2) I initialise the sdfat library with a 'spare' gpio pin.  Well, there aren't really any spare GPIO pins, but you can use one of the LED GPIOs - and this has the excellent advantage of doubling as an SD activity status light!  :+1:

## SD CARD READER
I'm using one that natively supports 3v3 and picked the smallest I could find.  https://www.adafruit.com/product/4682

I intend to cut it down even further with a saw (because I am not using SDIO, and that chip on the right is just some resistors that I could add externally)

Noting that, now that USB Mass Storage support is added, I *could* use a nonremoval SPI flash storage device, instead of an SD CARD READER.  Unclear if that would be beneficial.  Adafruit sells a nonreplacable-SD-card, which could be nice but has a fairly low capacity (https://learn.adafruit.com/adafruit-spi-flash-sd-card), so if you need capacity, SD CARD seems the way to go.

## OLED
I'm using one of the many 'generic' OLED1306 modules, this one being 128x64 pixels, which allows for 16x8 characters on a 8x8 grid.
By default, TZXDuino will use just four rows (16x4), at a pseudo-expanded ('twice height') font  -  which I think is actually a quirk rather than necessarily by design.  It would be possible to change to 16x8 characters but no immediate benefits since nothing in TZXDuino uses the extra space.  MAXDuino does something similar here, but has easy ways to change the configuration to 16x8, and it also uses the additional space.

## LiPo CHARGING / POWER CIRCUIT
I'll write this up separately, probably in a Seeeduino forum.  I'm using a 'generic' TP4056 module connected to the Seeeduino 5v out (so it can charge the battery when the Xiao is plugged into a USB power source); the LiPo is connected to the TP4056 in the usual way but across the same terminals I have a tiny 3v3 step down "buck" device - the output of which is connected to the Seeeduino Xiao 3v3 line.

I experimented quite a bit, and it seems to work, although I don't know if this is a good idea in an electrical sense.  3v3 is provided by both the Xiao's onboard stepdown (from 5v), and from the external 3v3 step down (from the battery), and those are tied together.  I think it's ok - the external step down doesn't seem to be generating any reverse current, so the only current flowing into the battery while charging should be coming from the TP4056.  If it blows up though, I'll have learned a lesson.

The external step down is also the smallest/cheapest I could find: https://www.adafruit.com/product/4711 .  And I could even cut a bit off that as well (the rounded end with mounting hole)

The TP4056 is one that looks like this https://www.amazon.co.uk/AZDelivery-TP4056-Battery-Charger-Module/dp/B089QJZ3QP?th=1 but guess what, I don't need the USB socket at all (since I'm taking V_IN from the Xiao 5v line), so I can chop all that off and get the board about half the size.

What I would *really like* to do is find a spare GPIO and monitor the battery voltage / charging voltage, but with no spare GPIOs I think I am out of luck unless I get very creative.
