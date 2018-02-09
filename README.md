# TZXDuino
Arduino based TZX and TAP (currently only ZX spectrum and Amstrad CPC flavour) file player.<br/>
(Excuse the badly written code, I'm working on that ;) )<br/>

## Parts Used
1. Arduino (I used a Nano, but this should work on any Nano compatible.)<br/>
2. SD Card module<br/>
3. 16x2 I2C LCD<br/>
4. Small audio amplifier. (I used an LM386 module, but any headphone type amplifier should work.)<br/>
5. 5 x Buttons (Micro switches are the best, but any should work.)<br/>

## Installation
**Not All Libraries Are Equal!**<br/>
<br/>
With so many variations on the SD, LCD I2C and TimerOne libraries I thought I'd include them here.  So to prevent any hair pulling moments use these versions of the libraries. <br/>

## Wiring
All buttons and the motor control pull the pin to GND.<br/>
<br/>
Arduino pins:<br/>
&nbsp;&nbsp;D6 -> MOTOR CONTROL (Connected to one side of the 2.5mm socket. Only used on some CPC models.)<br/>
&nbsp;&nbsp;D7 -> BUTTON (ROOT)<br/>
&nbsp;&nbsp;D9 -> Output (To amplifier)<br/>
D10 -> SDCS (SD Card chip select)<br/>
D11 -> MOSI (SD Card MOSI pin)<br/>
D12 -> MISO (SD Card MI/MISO pin)<br/>
D13 -> SCK (SD Card SCK pin)<br/>
&nbsp;&nbsp;A0 -> BUTTON (DOWN)<br/>
&nbsp;&nbsp;A1 -> BUTTON (UP)<br/>
&nbsp;&nbsp;A2 -> BUTTON (STOP)<br/>
&nbsp;&nbsp;A3 -> BUTTON (PLAY)<br/>
&nbsp;&nbsp;A4 -> LCD SDA<br/>
&nbsp;&nbsp;A5 -> LCD SCL<br/>
<br/>
LCD pins:<br/>
&nbsp;SDA -> Arduino A4<br/>
&nbsp;&nbsp;SCL -> Arduino A5<br/>
&nbsp;VCC -> 5V<br/>
GND -> GND<br/>
<br/>
SD Card pins:<br/>
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;5V -> 5V<br/>
&nbsp;GND -> GND<br/>
SDCS -> D10 Arduino<br/>
MOSI -> D11 Arduino<br/>
MISO -> D12 Arduino<br/>
&nbsp;&nbsp;&nbsp;SCK -> D13 Arduino<br/>

## Usage
Wire up as above, and program the Arduino using the IDE.<br/>
<br/>
Copy some TZX/TAP/CDT files onto a FAT32 formatted SD Card, plug it into the TZXDuino and power it on.<br/>
<br/>
I suggest to format SD Card with this tool to ensure maximum compatibility.<br/>
[SD Memory Card Formatter 5.0 for SD/SDHC/SDXC](https://www.sdcard.org/downloads/formatter_4/)<br/>
**CAUTION: Formatting will cause complete data loss on SD Card. Make sure to backup the SD Card before formatting**<br/>
<br/>
**Button functions:**<br/>
- UP/DOWN - No playback: move through the files<br/>
            During playback: Works as PREVIOUS/NEXT file.<br/>
- PLAY -  On file: starts playback.<br/>
          On directory: open directory.<br/>
- STOP -  During playback: stops playback.<br/>
          No playback: up a directory.<br/>
- ROOT -  During playback: restart playback.<br/>
          No playback: set TSX Baud Rate<br/>
<br/>
To get files to load first time you'll need to adjust the volume until you get a strong signal on the Spectrum / CPC.<br/>
Once you've got a strong signal you should able to load most files as if you were using a normal tape.<br/>

## History
Starting with the Arduitape WAV playing project. I've moved to playing TZX and TAP files directly without needing to convert them to a WAV file first.<br/>

## Credits
Code: Andrew Beer<br/>
<br/>
Testing: Duncan Edwards<br/>
<br/>
Code update: [Jujan](https://github.com/Jujan)
