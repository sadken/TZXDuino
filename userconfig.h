/////////////////////////////////////////////////////////////////
/*                                                             */
/*                                                             */
/*          Configure your screen settings here                */
/*    Delete the // of the lines appropriate to your device    */
/*                                                             */
/*                                                             */
/////////////////////////////////////////////////////////////////


// which device/platform are you building.

#if defined(__AVR__)
#define outputPin           9               // Audio Output PIN - Set accordingly to your hardware.
#define LowWrite() {PORTB&=(~(1<<PORTB1));}
#define HighWrite() {PORTB|=(1<<PORTB1);}
#define chipSelect 10                       //Sd card chip select pin
#elif defined(__SAMD21__)
#define outputPin           7               // Audio Output PIN - Set accordingly to your hardware.
#define LowWrite() {digitalWrite(outputPin, LOW);}
#define HighWrite() {digitalWrite(outputPin, HIGH);}
#define chipSelect 2                        //Sd card chip select pin
#endif

//#define BUTTONS_ADC
//#define BUTTONS_ADC_PIN      A3 // buttons on single ADC input pin
// set ADC ranges here (used if BUTTONS_ADC is defined). Set your resistors so that the voltage divider falls in the middle of the bands.
// each #define is the 'lower bound', the upper bound is the next level after it.
// no action triggered when ADC reading between 0 and the level for UP
#define BUTTONS_ADC_UP_LOW   200 // UP action triggered when ADC reading anywhere between this level and the next level (DOWN)
#define BUTTONS_ADC_DOWN_LOW 400 // DOWN action triggered when ADC reading anywhere between this level and the next level (ROOT)
#define BUTTONS_ADC_ROOT_LOW 700 // ROOT action triggered when ADC reading anywhere between this level and the next level (STOP)
#define BUTTONS_ADC_STOP_LOW 900 // STOP action triggered when ADC reading anywhere between this level and the next level (PLAY)
#define BUTTONS_ADC_PLAY_LOW 980 // PLAY action triggered when ADC reading anywhere between this level and 1023 (= upper limit)

// set button GPIO pins (used if BUTTONS_ADC is NOT defined)
#define btnPlay       17            //Play Button
#define btnStop       16            //Stop Button
#define btnUp         15            //Up button
#define btnDown       14            //Down button
#define btnRoot       7             //Return to SD card root

// define HAVE_MOTOR if you want to add support for motor control (requires an extra GPIO)
#define HAVE_MOTOR
#define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)

// define HAVE_EEPROM if your device has an eeprom compatible with <EEPROM.h>
#define HAVE_EEPROM


//#define SERIALSCREEN  1           /*For Testing purposes only */

#define LCDSCREEN16x2   1         /*Set if you are using a 1602 LCD screen */
#define LCD_I2C_ADDR    0x27      /*Set the i2c address of your 1602LCD usually 0x27 or 0x3f*/

//#define RGBLCD         1

//#define OLED1306       1          /*Defines if you are using a 128x64 or 128x32 OLED screen */
//#define OLED1306_0.91  1          /*Defines if you are using a 0.91" 128x32 screen.*/
//#define OLED1306_1.3   1          /* Use this line as well if you have a 1.3" OLED screen */
//#define SPECFONT       1          /* Use this line if you want to use the Spectrum Font by Brendan Alford

//#define P8544   1                 /*Set if you are using an 84x48 PCD8544 screen */

//#define TZXDuino_Logo  1

//#define LCD_USE_1602_LCD_MODULE  1 /*Not used at the moment */

// filesystem options
#define maxFilenameLength   100       //Maximum length for long filename support (ideally as large as possible to support very long filenames)
#define nMaxPrevSubDirs  20         //Number of parent directories to hold i.e. maximum depth of being able to navigate 'back up' through parent folders (ideally as large as possible to support deep file trees)

// Test with reduced SPI speed for breadboards.  SD_SCK_MHZ(4) will select
// the highest speed supported by the board that is not over 4 MHz.
// Change SPI_SPEED to SPI_FULL_SPEED or SD_SCK_MHZ(50) for best performance.
//#define SPI_SPEED SD_SCK_MHZ(4)
#define SPI_SPEED SPI_FULL_SPEED

// support exFat?
// SD_INCLUDE_EXFAT_SUPPORT
// set to 1 to include exfat, 0 to exclude exfat (fat16/fast32 only), or undefine to just automatically choose for you - based on available ram
#define SD_INCLUDE_EXFAT_SUPPORT 0 // 0 or 1 .  0 means 'do not include exfat support', which will make the smallest firmware
// SD_FAT_TYPE
// Using the SD_INCLUDE_EXFAT_SUPPORT is preferred but if you want to use the SdFatConfig.h approach (as documented in sdfat docs)
// or you want to hardcode the support type yourself (e.g. if you want ONLY exfat and no fat32 support) you can #define SD_FAT_TYPE directly
//#define SD_FAT_TYPE 1 // 0 or 1 or 2 or 3 , see sdfat docs
