/*
 *                                    TZXduino
 *                             Written and tested by
 *                          Andrew Beer, Duncan Edwards
 *                          www.facebook.com/Arduitape/
 *                          
 *              Designed for TZX files for Spectrum (and more later)
 *              Load TZX files onto an SD card, and play them directly 
 *              without converting to WAV first!
 *              
 *              Directory system allows multiple layers,  to return to root 
 *              directory ensure a file titles ROOT (no extension) or by 
 *              pressing the Menu Select Button.
 *              
 *              Written using info from worldofspectrum.org
 *              and TZX2WAV code by Francisco Javier Crespo
 *              
 *              ***************************************************************
 *              Menu System:
 *                TODO: add ORIC and ATARI tap support, clean up code, sleep                 
 *                
 *              V1.0
 *                Motor Control Added.
 *                High compatibility with Spectrum TZX, and Tap files
 *                and CPC CDT and TZX files.
 *                
 *                V1.32 Added direct loading support of AY files using the SpecAY loader 
 *                to play Z80 coded files for the AY chip on any 128K or 48K with AY 
 *                expansion without the need to convert AY to TAP using FILE2TAP.EXE. 
 *                Download the AY loader from http://www.specay.co.uk/download 
 *                and load the LOADER.TAP AY file loader on your spectrum first then
 *                simply select any AY file and just hit play to load it. A complete
 *                set of extracted and DEMO AY files can be downloaded from
 *                http://www.worldofspectrum.org/projectay/index.htm
 *                Happy listening!
 *                
 *                V1.8.1 TSX support for MSX added by Natalia Pujol
 *                
 *                V1.8.2 Percentage counter and timer added by Rafael Molina Chesserot along with a reworking of the OLED1306 library. 
 *                Many memory usage improvements as well as a menu for TSX Baud Rates and a refined directory controls.
 *                
 *                V1.8.3 PCD8544 library changed to use less memory. Bitmaps added and Menu system reduced to a more basic level. 
 *                Bug fixes of the Percentage counter and timer when using motor control/
 *                
 *                V1.11 Added unzipped UEF playback and turbo UEF to the Menu thatks to the kind work by kernal@kernalcrash.com
 *                Supports Gunzipped UEFs only.
 *                
 *                v1.13 HQ.UEF support added by Rafael Molina Chesserot of Team MAXDuino 
 *                v1.13.1 Removed digitalWrite in favour of a macro suggested by Ken Forster
 *                Some MSX games will now work at higher Baudrates than before.
 *                v1.13.2 Added a fix to some Gremlin Loaders which reverses the polarity of the block.
 *                New menu Item added. "Gremlin Loader" turned on will allow Samurai Trilogy and Footballer of the Year II 
 *                CDTs to load properly.
 *                
 *                1.14 ID15 code adapted from MAXDuino by Rafael Molina Chasserot.
 *                Not working 100% with CPC Music Loaders but will work with other ID15 files.
 *                
 *                1.14.2 Added an ID15 switch to Menu as ID15 being enabled was stopping some files loading properly.
 *                1.14.3 Removed the switch in favour of an automatic system of switching ID15 routine on and off.
 *                
 *                1.15 Added support for the Surenoo RGB Backlight LCD using an adapted version of the Grove RGBLCD library.
 *                     Second counter not currently working. Also some memory saving tweaks.
 *                     
 *                1.15.3 Adapted the MAXDuino ID19 code and TurboMode for ZX80/81
 *                       Also added UEF Chunk 117 which allows for differing baudrates in BBC UEFs.
 *                       Added a Spectrum Font for OLED 1306 users converted by Brendan Alford
 *                       Added File scrolling by holding up or down buttons. By Brendan Alford.
 *                       
 *                1.16 Fixed a bug that was stopping Head Over Heels (and probably others)loading on +2 Spectrum. Seems to have made 
 *                     ZX80/81 playback alot more stable too.
 *                     
 *                1.17 Added ORIC TAP file playback from the Maxduino team.
 *               
 *                1.18 Added a delay to IDCHUNKEOF before the stopFile(); as it was cutting off the last few milliseconds of playback on some UEF files.
 *                     Jungle Journey for Acorn Electron is now working.
 *                       
 *                
  */

#include "SdCardSetup.h"
#include "Timers.h"
#include "TZXDuino.h"
#include "userconfig.h"
#include "buttons.h"
#include "version.h"

#ifdef P8544
  const size_t screen_width_chars = 14;
  const char space[] PROGMEM = "              ";
#else
  const size_t screen_width_chars = 16;
  const char space[] PROGMEM = "                ";
#endif

#ifdef LCDSCREEN16x2
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(LCD_I2C_ADDR,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
#endif

#ifdef RGBLCD
  #include <Wire.h>
  #include "rgb_lcd.h"
  const int colorR = 200;
  const int colorG = 000;
  const int colorB = 000;

  rgb_lcd lcd; 
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
#endif

#ifdef OLED1306 
  #include <Wire.h> 
#endif

#ifdef P8544
  #include <pcd8544.h>
  #define ADMAX 1023
  #define ADPIN 0
  #include <avr/pgmspace.h>
  byte dc_pin = 5;
  byte reset_pin = 3;
  byte cs_pin = 4;
  pcd8544 lcd(dc_pin, reset_pin, cs_pin);
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
  #define backlight_pin 2
  
  const byte Play [] PROGMEM = {
    0x00, 0x7f, 0x3e, 0x1c, 0x08, 0x00, 0x00
  };

  const byte Paused [] PROGMEM = {
    0x00, 0x7f, 0x7f, 0x00, 0x7f, 0x7f, 0x00
  };

  const byte Stop [] PROGMEM = {
    0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e
  };
#endif

SDTYPE sd;                          // Initialise SD card 
FILETYPE entry, dir, tmpdir;        // SD card current file (=entry) and current directory (=dir) objects, plus one temporary

char fileName[maxFilenameLength + 1];     //Current filename
unsigned int fileNameLen;
uint16_t fileIndex;                    //Index of current file, relative to current directory
uint16_t prevSubDirIndex[nMaxPrevSubDirs];  //History when stepping into subdirectories (sequence of indexes of directory entries, relative to root)
byte subdir = 0;
unsigned long filesize;             // filesize used for dimensioning AY files

#define scrollSpeed   250           //text scroll delay
#define scrollWait    3000          //Delay before scrolling starts

byte scrollPos = 0;                 //Stores scrolling text position
unsigned long scrollTime = millis() + scrollWait;

#ifdef HAVE_MOTOR
bool motorState = true;                //Current motor control state
bool oldMotorState = true;             //Last motor control state
#endif

bool start = false;                     //Currently playing flag
bool pauseOn = false;                   //Pause state
uint16_t lastIndex = 0;             //Index of last file in current directory
bool isDir = false;                     //Is the current file a directory
unsigned long timeDiff = 0;         //button debounce
int browseDelay = 500;              // Delay between up/down navigation

char PlayBytes[17];


void setup() {
  pinMode(chipSelect, OUTPUT);      //Setup SD card chipselect pin
  while (!sd.begin(chipSelect,SPI_SPEED)) {  
    //Start SD card and check it's working
    printtextF(PSTR("No SD Card"),0);
    delay(1000);
  } 

  #ifdef USB_STORAGE_ENABLED
  // need to do this as early as possible, to ensure mass storage gets enumerated
  // see https://github.com/adafruit/Adafruit_TinyUSB_ArduinoCore/issues/4
  setup_usb_storage();
  #endif

  #ifdef LCDSCREEN16x2
    lcd.init();                     //Initialise LCD (16x2 type)
    lcd.backlight();
    lcd.clear();
    lcd.createChar(0, SpecialChar);
  #endif

  #ifdef RGBLCD
    lcd.begin(16,2);
    lcd.setRGB(colorR, colorG, colorB);
    lcd.clear();
    lcd.createChar(0, SpecialChar);
  #endif
  
  #ifdef SERIALSCREEN
    Serial.begin(115200);
  #endif
  
  #ifdef OLED1306 
    delay(1000);  //Needed!
    Wire.begin();
    init_OLED();
    delay(1500);              // Show logo
    reset_display();           // Clear logo and load saved mode
  #endif

  #ifdef P8544
    lcd.begin ();
    analogWrite (backlight_pin, 20);
    P8544_splash(); 
  #endif

  printtextF(PSTR("Starting.."),0);
  
  dir.open("/");                    //set SD to root directory
  TZXSetup();                       //Setup TZX specific options
  
  setup_buttons();
 
  getMaxFile();                     //get the total number of files in the directory
  fileIndex=0;                      //move to the first file in the directory
  loadEEPROM();

  delay(500);

  #ifdef LCDSCREEN16x2
    lcd.clear();
  #endif

  #ifdef RGBLCD
    lcd.clear();
  #endif

  #ifdef P8544
    lcd.clear();
  #endif

  seekFile();                       
}

void loop(void) {
  if(start)
  {
    //TZXLoop only runs if a file is playing, and keeps the buffer full.
    TZXLoop();
  } else {
    digitalWrite(outputPin, LOW);    //Keep output LOW while no file is playing.
  }
  
  if((millis()>=scrollTime) && !start && fileNameLen > screen_width_chars-1) {
    //Filename scrolling only runs if no file is playing to prevent I2C writes 
    //conflicting with the playback Interrupt
    scrollTime = millis()+scrollSpeed;
    scrollText(fileName);
  }

  #ifdef HAVE_MOTOR
  motorState = button_motor();
  #endif
  
  if (millis() - timeDiff > 50) {   // check switch every 50ms 
     timeDiff = millis();           // get current millisecond count
      
      if(button_play()) {
        //Handle Play/Pause button
        if(!start) {
          //If no file is play, start playback
          playFile();
          delay(200);
        } else {
          //If a file is playing, pause or unpause the file                  
          pauseOn = !pauseOn;
          if (pauseOn) {
            printtextF(PSTR("Paused "),0);
            Counter1();             
            #ifdef P8544 
              lcd.gotoRc(3,38);
              lcd.bitmap(Paused, 1, 6);
            #endif
          } else {
           printtextF(PSTR("Playing"),0);
           Counter1();
           
            #ifdef P8544
              lcd.gotoRc(3,38);
              lcd.bitmap(Play, 1, 6);               
            #endif
          }
       }
       button_wait(button_play);
     }

     if(button_root() && !start){
      
       menuMode();
       printtextF(PSTR(VERSION),0);
       //lcd_clearline(1);
       printtextF(space,1);
       scrollPos=0;
       scrollText(fileName);
       
       button_wait(button_root);
     }

     if(button_stop() && start) {
       stopFile();
       button_wait(button_stop);
     }

     if(button_stop() && !start && subdir >0) {  
       subdir--;
       uint16_t this_directory=prevSubDirIndex[subdir];
    
       // get to parent folder via root (slow-ish but requires very little memory to keep a deep stack of parents)
       dir.close();
       dir.open("/");
       for(int i=0; i<subdir; i++)
       {
         tmpdir.open(&dir, prevSubDirIndex[i], O_RDONLY);
         dir = tmpdir;
         tmpdir.close();
       }
     
       getMaxFile();
       fileIndex = this_directory;
       seekFile();
       button_wait(button_stop);
     }     

     if (!start)
     {
        if(button_down()) 
        {
          //Move down a file in the directory
          scrollTime=millis()+scrollWait;
          scrollPos=0;
          downFile();
          if (button_wait_timeout(button_down, browseDelay)) {
            reduceBrowseDelay();
          }
          else
          {
            resetBrowseDelay();    
          }
        }

       else if(button_up()) 
       {
         //Move up a file in the directory
         scrollTime=millis()+scrollWait;
         scrollPos=0;
         upFile();       
         if (button_wait_timeout(button_up, browseDelay)) {
           reduceBrowseDelay();
         }
         else
         {
           resetBrowseDelay();    
         }
       }
       else
       {
        resetBrowseDelay();
       }
     }

     #ifdef HAVE_MOTOR
     if(start && (oldMotorState!=motorState)) {  
       //if file is playing and motor control is on then handle current motor state
       //Motor control works by pulling the btnMotor pin to ground to play, and NC to stop
       if(motorState && !pauseOn) {
        printtextF(PSTR("Paused "),0);
        Counter1();
        
            #ifdef P8544 
              lcd.gotoRc(3,38);
              lcd.bitmap(Paused, 1, 6);
            #endif
         pauseOn = true;
       } 
       if(!motorState && pauseOn) {
         printtextF(PSTR("Playing"),0);
         Counter1();
         
            #ifdef P8544
              lcd.gotoRc(3,38);
              lcd.bitmap(Play, 1, 6);              
            #endif
            
         pauseOn = false;
       }
       oldMotorState=motorState;
     }
     #endif
  }
}

void reduceBrowseDelay() {
  if (browseDelay >= 100) {
    browseDelay -= 100;
  }
}

void resetBrowseDelay() {
  browseDelay = 500;
}

void upFile() {    
  // move up a file in the directory.
  // find prior index, using fileIndex.
  while(fileIndex!=0)
  {
    fileIndex--;
    // fileindex might not point to a valid entry (since not all values are used)
    // and we might need to go back a bit further
    // Do this here, so that we only need this logic in one place
    // and so we can make seekFile dumber
    entry.close();
    if (entry.open(&dir, fileIndex, O_RDONLY))
    {
      entry.close();
      break;
    }
  }

  if(fileIndex==0)
  {
    // seek up wrapped - should now be reset to point to the last file
    fileIndex = lastIndex;
  }
  seekFile();
}

void downFile() {    
  //move down a file in the directory
  if (fileIndex>=lastIndex)
  {
    // seek down wrapped - now 0
    fileIndex=0;
  }
  else
  {
    fileIndex++;
  }    
  seekFile();
}

void seekFile() {
  //move to a set position in the directory, store the filename, and display the name on screen.
  entry.close(); // precautionary, and seems harmelss if entry is already closed
  if (!entry.open(&dir, fileIndex, O_RDONLY))
  {
    // if entry.open fails, when given an index, it seems to automatically adjust curPosition to the next good entry
    // this means that we can just retry calling open, with the new index
    fileIndex = dir.curPosition()/32-1;
    entry.close();
    entry.open(&dir, fileIndex, O_RDONLY);
  }
  
  entry.getName(fileName, maxFilenameLength);
  fileNameLen = strlen(fileName);
  filesize = entry.fileSize();
  ayblklen = filesize + 3;  // add 3 file header, data byte and chksum byte to file length
  isDir = (entry.isDir() || 0==strcmp(fileName, "ROOT"));
  entry.close();

  if (isDir==1) {
    printtext(PSTR(VERSION),0);
    #ifdef P8544
      printtext("                 ",3);
    #endif
  } else {
    printtext(PSTR("Select File.."),0);
    #ifdef P8544
      printtext("                 ",3);
    #endif
  }

  scrollPos=0;
  scrollText(fileName);
  scrollTime=millis()+scrollWait;
}

void stopFile() {
  TZXStop();
  if(start){
    printtextF(PSTR("Stopped"),0);
    //lcd_clearline(0);
    //lcd.print(F("Stopped"));
    #ifdef P8544
      lcd.gotoRc(3,38);
      lcd.bitmap(Stop, 1, 6);
    #endif
    start=false;
  }
}

void playFile() {
  
  if(isDir)
  {
    //If selected file is a directory move into directory
    changeDir();
  }
  else if (fileName[0] == '\0') 
  {
    printtextF(PSTR("No File Selected"),1);
    //lcd_clearline(1);
    //lcd.print(F("No File Selected"));
  }
  else
  {
    printtextF(PSTR("Playing         "),0);    
    scrollPos=0;
    if (!PauseAtStart) pauseOn = false;
    scrollText(fileName);
    currpct=100;
    lcdsegs=0;      
    TZXPlay(); 
      #ifdef P8544
        lcd.gotoRc(3,38);
        lcd.bitmap(Play, 1, 6);
      #endif
    start=true; 
    if (PauseAtStart) {
      printtextF(PSTR("Paused "),0);
      #ifdef P8544
        lcd.gotoRc(3,38);
        lcd.bitmap(Play, 1, 6);
      #endif
      pauseOn = true;
      TZXPause();
    }
  }
}

void getMaxFile() {    
  // gets the total files in the current directory and stores the number in maxFile
  // and also gets the file index of the last file found in this directory
  dir.rewind();
  while(entry.openNext(&dir, O_RDONLY)) {
    entry.close();
    lastIndex = dir.curPosition()/32-1;
  }

  dir.rewind(); // precautionary but I think might be unnecessary since we're using fileIndex everywhere else now
}

void changeDir() {    
  //change directory, if fileName="ROOT" then return to the root directory
  //SDFat has no easy way to move up a directory, so returning to root is the easiest way. 
  //each directory (except the root) must have a file called ROOT (no extension)
  if(!strcmp(fileName, "ROOT"))
  {
    subdir=0;    
    dir.close();
    dir.open("/");
  }
  else
  {
    if (subdir < nMaxPrevSubDirs)
    {
      // stash subdirectory index (much quicker than copying the entire name)
      prevSubDirIndex[subdir] = fileIndex;
      subdir++;
    }
    tmpdir.open(&dir, fileIndex, O_RDONLY);
    dir = tmpdir;
    tmpdir.close();
  }
  getMaxFile();
  fileIndex=0;
  seekFile();
}

void scrollText(char* text)
{
  if(scrollPos==0) {
    scrollTime = millis()+scrollWait;
  }

  // do nothing if no lcd defined or only serial display defined
#if defined(LCDSCREEN16x2) || defined(RGBLCD) || defined(OLED1306) || defined(P8544)

  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  // or 14 chars on P8544
  char outtext[screen_width_chars + 1];
  char * outtext_p = outtext;
  char * textp = text + scrollPos;
  if(isDir) {
    *outtext_p++ = '>';
  }
  while(outtext_p < outtext+screen_width_chars && *textp != '\0') {
    *outtext_p++ = *textp++;
  }
  *outtext_p='\0';
  printtext(outtext,1);

  // new scroll routine below
  // when we reached the end of the string, instead of scrolling it away to nothingness, we'll pause at the end
  // for longer than usual, and reset the pos to 0 for next time
  if (*textp == '\0') {
    // reached the end of the string, during the above loop.
    scrollPos = 0;
    scrollTime=millis()+scrollWait;
  }
  else
  {
    scrollPos++;
  }

#endif
}



void printtextF(const char* text, int l) {  //Print text to screen. 
  
  #ifdef SERIALSCREEN
  Serial.println(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif
  
  #ifdef LCDSCREEN16x2
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif

  #ifdef RGBLCD
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif

  #ifdef OLED1306
    sendStrLine(text, l);
  #endif

  #ifdef P8544
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif 
   
}

void printtext(char* text, int l) {  //Print text to screen. 
  
  #ifdef SERIALSCREEN
  Serial.println(text);
  #endif
  
  #ifdef LCDSCREEN16x2
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif

  #ifdef RGBLCD
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif

  #ifdef OLED1306
    sendStrLine(text, l);
  #endif

  #ifdef P8544
    lcd.setCursor(0,l);
    lcd.print(space);
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif   
   
}
