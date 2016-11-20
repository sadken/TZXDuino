#TZXDuino

Arduino based TZX and TAP (currently only ZX spectrum and Amstrad CPC flavour) file player.

## Parts Used
1. Arduino (I used a Nano, but this should work on any NANO compatible)
2. SD card module
3. 16x2 I2C LCD
4. Small audio amp, I used an LM386 module, but any headphone type amp should work
5. 5 x Buttons (micro switches are best, but any should work)

## Installation
Not All Libraries Are Equal!

With so many variations on the SD, LCD I2C, and TimerOne libraries i thought I'd include them here.  So to prevent any hair pulling moments use these versions of the libraries. 

## Wiring

All buttons and the motor control pull the pin to GND.

Arduino Pins:
       06 -> MOTOR CONTROL (Connected to one side of the 2.5mm socket Only used on some CPC models)
       07 -> BUTTON (ROOT)
       09 -> Output (To Amp)
       10 -> SDCS (SD Card chip select)
       11 -> MOSI (SD Card MOSI PIN)
       12 -> MISO (SD Card MI PIN)
       13 -> SCK (SD Card SCK PIN)
A0     14 -> BUTTON (DOWN)
A1     15 -> BUTTON (UP)
A2     16 -> BUTTON (STOP)
A3     17 -> BUTTON (PLAT)
A4 SDA 18 -> LCD SDA
A5 SCL 19 -> LCD SCL


LCD PINS:
SDA ->  Arduino A4
SCL ->  Arduino A5
VCC ->  5v
GND ->  GND

SD CARD PINS:
 GND -> GND
3.3V -> NC
  5V -> 5V
SDCS -> 10 ARDUINO
MOSI -> 11 ARDUINO
 SCK -> 13 ARDUINO
MISO -> 12 ARDUINO
 GND -> GND
 
## Usage
Wire up as above, and program the Arduino using the IDE.

Drop some TZX/TAP/CDT files onto a FAT32 formatted SD card, plug it into the TZXDuino, and power on. 

Up/Down buttons move through the files, Play starts playback.

To get files to load first time you'll need to adjust the volume until you get a strong signal on the Spectrum / CPC.
Once you've got a strong signal you should able to load most files as if you were using a normal tape. 

## History
Starting with the Arduitape WAV playing project, i've moved to playing TZX and TAP files directly without needing to convert them to a WAV file first. 

## Credits
Code: Andrew Beer

Testing: Duncan Edwards
