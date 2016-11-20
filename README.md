#TZXDuino

Arduino based TZX and TAP (currently only ZX spectrum and Amstrad CPC flavour) file player.<br/>
(Excuse the badly written code, I'm working on that ;) )

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

Arduino Pins:<br/>
       06 -> MOTOR CONTROL (Connected to one side of the 2.5mm socket Only used on some CPC models)<br/> 
       07 -> BUTTON (ROOT)<br/>
       09 -> Output (To Amp)<br/>
       10 -> SDCS (SD Card chip select)<br/>
       11 -> MOSI (SD Card MOSI PIN)<br/>
       12 -> MISO (SD Card MI PIN)<br/>
       13 -> SCK (SD Card SCK PIN)<br/>
A0     14 -> BUTTON (DOWN)<br/>
A1     15 -> BUTTON (UP)<br/>
A2     16 -> BUTTON (STOP)<br/>
A3     17 -> BUTTON (PLAT)<br/>
A4 SDA 18 -> LCD SDA<br/>
A5 SCL 19 -> LCD SCL<br/>
<br/>
<br/>
LCD PINS:<br/>
SDA ->  Arduino A4<br/>
SCL ->  Arduino A5<br/>
VCC ->  5v<br/>
GND ->  GND<br/>
<br/>
SD CARD PINS:<br/>
 GND -> GND<br/>
3.3V -> NC<br/>
  5V -> 5V<br/>
SDCS -> 10 ARDUINO<br/>
MOSI -> 11 ARDUINO<br/>
 SCK -> 13 ARDUINO<br/>
MISO -> 12 ARDUINO<br/>
 GND -> GND<br/>
<br/>
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
