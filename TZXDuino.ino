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

#include <SdFat.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "TZXDuino.h"
#include "userconfig.h"

#define VERSION "TZXDuino 1.19"


#ifdef LCDSCREEN16x2
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(LCD_I2C_ADDR,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
#endif

#ifdef RGBLCD
  #include <Wire.h>
  #include "rgb_lcd.h"
  const int colorR = 200;
  const int colorG = 000;
  const int colorB = 000;

  rgb_lcd lcd; 
  char indicators[] = {'|', '/', '-',0};
  uint8_t SpecialChar [8]= { 0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00 };
#endif

#ifdef OLED1306 
  #include <Wire.h> 
  
  char line0[17];
  char line1[17];
  char lineclr[17];
  char indicators[] = {'|', '/', '-',92}; 
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
  char indicators[] = {'|', '/', '-',0};
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

SdFat sd;                           //Initialise Sd card 

SdFile entry;                       //SD card file

#define subdirLength     22         // hasta 62 no incrementa el consumo de RAM
#define filenameLength   4*subdirLength  //Maximum length for scrolling filename, hasta 255 no incrementa consumo RAM

//#define filenameLength    100       //Maximum length for scrolling filename

char fileName[filenameLength + 1];    //Current filename
char sfileName[13];                 //Short filename variable
//char prevSubDir[5][25];
char prevSubDir[4][subdirLength];
int DirFilePos[4];                   //File Positios in Directory to be restored
int subdir = 0;
unsigned long filesize;             // filesize used for dimensioning AY files
const int chipSelect = 10;          //Sd card chip select pin

#define btnPlay       17            //Play Button
#define btnStop       16            //Stop Button
#define btnUp         15            //Up button
#define btnDown       14            //Down button
#define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)
#define btnRoot       7             //Return to SD card root
#define scrollSpeed   250           //text scroll delay
#define scrollWait    3000          //Delay before scrolling starts

byte scrollPos = 0;                 //Stores scrolling text position
unsigned long scrollTime = millis() + scrollWait;

byte mselectState = 0;              //Motor control state 1=on 0=off
byte motorState = 1;                //Current motor control state
byte oldMotorState = 1;             //Last motor control state
byte start = 0;                     //Currently playing flag
byte pauseOn = 0;                   //Pause state
int currentFile = 1;                //Current position in directory
int maxFile = 0;                    //Total number of files in directory
byte isDir = 0;                     //Is the current file a directory
unsigned long timeDiff = 0;         //button debounce
int browseDelay = 500;              // Delay between up/down navigation

byte UP = 0;                      //Next File, down button pressed
//char PlayBytes[16];
char PlayBytes[subdirLength];


void setup() {
  
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

  
  
  pinMode(chipSelect, OUTPUT);      //Setup SD card chipselect pin
  if (!sd.begin(chipSelect,SPI_FULL_SPEED)) {  
    //Start SD card and check it's working
    printtextF(PSTR("No SD Card"),0);
    
    return;
  } 
  
  sd.chdir();                       //set SD to root directory
  TZXSetup();                       //Setup TZX specific options
  
  //General Pin settings
  //Setup buttons with internal pullup 
  pinMode(btnPlay,INPUT_PULLUP);
  digitalWrite(btnPlay,HIGH);
  pinMode(btnStop,INPUT_PULLUP);
  digitalWrite(btnStop,HIGH);
  pinMode(btnUp,INPUT_PULLUP);
  digitalWrite(btnUp,HIGH);
  pinMode(btnDown,INPUT_PULLUP);
  digitalWrite(btnDown,HIGH);
  pinMode(btnMotor, INPUT_PULLUP);
  digitalWrite(btnMotor,HIGH);
  pinMode(btnRoot, INPUT_PULLUP);
  digitalWrite(btnRoot, HIGH); 
   
 
  printtextF(PSTR("Starting.."),0);
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
       
  getMaxFile();                     //get the total number of files in the directory
  seekFile(currentFile);            //move to the first file in the directory
  loadEEPROM();
  
  

}

void loop(void) {
  
  if(start==1)
  {
    //TZXLoop only runs if a file is playing, and keeps the buffer full.
    TZXLoop();
  } else {
    digitalWrite(outputPin, LOW);    //Keep output LOW while no file is playing.
  }
  
  if((millis()>=scrollTime) && start==0 && (strlen(fileName)>15)) {
    //Filename scrolling only runs if no file is playing to prevent I2C writes 
    //conflicting with the playback Interrupt
    scrollTime = millis()+scrollSpeed;
    scrollText(fileName);
    scrollPos +=1;
    if(scrollPos>strlen(fileName)) {
      scrollPos=0;
      scrollTime=millis()+scrollWait;
      scrollText(fileName);
    }
  }
  motorState=digitalRead(btnMotor);
  if (millis() - timeDiff > 50) {   // check switch every 50ms 
     timeDiff = millis();           // get current millisecond count
      
      if(digitalRead(btnPlay) == LOW) {
        //Handle Play/Pause button
        if(start==0) {
          //If no file is play, start playback
          playFile();
          delay(200);
        } else {
          //If a file is playing, pause or unpause the file                  
          if (pauseOn == 0) {
            printtextF(PSTR("Paused "),0);
            Counter1();             
            #ifdef P8544 
              lcd.gotoRc(3,38);
              lcd.bitmap(Paused, 1, 6);
            #endif
            
            pauseOn = 1;
            
          } else {
           printtextF(PSTR("Playing"),0);
           Counter1();
           
            #ifdef P8544
              lcd.gotoRc(3,38);
              lcd.bitmap(Play, 1, 6);               
            #endif
            
            pauseOn = 0;
          }
       }
       while(digitalRead(btnPlay)==LOW) {
        //prevent button repeats by waiting until the button is released.
        delay(200);
       }
     }

    

     if(digitalRead(btnRoot)==LOW && start==0){
      
       menuMode();
       printtextF(PSTR(VERSION),0);
       //lcd_clearline(1);
       printtextF(PSTR("                "),1);
       scrollPos=0;
       scrollText(fileName);
       
                
       while(digitalRead(btnRoot)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(200);
       }
     }
     if(digitalRead(btnStop)==LOW && start==1) {
       stopFile();
       while(digitalRead(btnStop)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(200);
       }       
     }
     if(digitalRead(btnStop)==LOW && start==0 && subdir >0) {  
       /*fileName[0]='\0';
       prevSubDir[subdir-1][0]='\0';
       subdir--;
       switch(subdir){
        case 1:
           //sprintf(fileName,"%s%s",prevSubDir[0],prevSubDir[1]);
           sd.chdir(strcat(strcat(fileName,"/"),prevSubDir[0]),true);
           break;
        case 2:
           //sprintf(fileName,"%s%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2]);
           sd.chdir(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),true);
           break;
       case 3:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
          sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),true); 
           break;
        case 4:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
          sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),"/"),prevSubDir[3]),true); 
           break;
        case 5:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
          sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),"/"),prevSubDir[3]),true); 
           break;          
        default: 
           //sprintf(fileName,"%s",prevSubDir[0]);
           sd.chdir("/",true);
       }*/
       fileName[0]='\0';
       subdir--;
       prevSubDir[subdir][0]='\0';     
       switch(subdir){
        case 0:
           //sprintf(fileName,"%s",prevSubDir[0]);
           sd.chdir("/");
           break;
        case 1:
           //sprintf(fileName,"%s%s",prevSubDir[0],prevSubDir[1]);
           sd.chdir(strcat(strcat(fileName,"/"),prevSubDir[0]));
           break;
        case 2:
        //default:
           //sprintf(fileName,"%s%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2]);
           subdir = 2;
           sd.chdir(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]));
           break;
       case 3:
       default:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
           subdir = 3;
           sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),true); 
           break; 
       /*case 4:
       default:
           //sprintf(fileName,"%s%s/%s/%s",prevSubDir[0],prevSubDir[1],prevSubDir[2],prevSubDir[3]);
           subdir = 4;
           sd.chdir(strcat(strcat(strcat(strcat(strcat(strcat(strcat(strcat(fileName,"/"),prevSubDir[0]),"/"),prevSubDir[1]),"/"),prevSubDir[2]),"/"),prevSubDir[3]),true); 
           break; */        
       }
       
       //Return to prev Dir of the SD card.
       //sd.chdir(fileName,true);
       //sd.chdir("/CDT");       
       //printtext(prevDir,0); //debug back dir
       getMaxFile();
       currentFile=1;
       seekFile(currentFile);  
       while(digitalRead(btnStop)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(200);
       }
     }     

     if (start==0)
     {
        if(digitalRead(btnDown)==LOW) 
        {
          //Move down a file in the directory
          scrollTime=millis()+scrollWait;
          scrollPos=0;
          downFile();
          //while(digitalRead(btnDown)==LOW) {
            //prevent button repeats by waiting until the button is released.
            delay(browseDelay);
            reduceBrowseDelay();
          //}
        }

       else if(digitalRead(btnUp)==LOW) 
       {
         //Move up a file in the directory
         scrollTime=millis()+scrollWait;
         scrollPos=0;
         upFile();       
         //while(digitalRead(btnUp)==LOW) {
           //prevent button repeats by waiting until the button is released.
            delay(browseDelay);
            reduceBrowseDelay();
         //   }
       }
       else
       {
        resetBrowseDelay();
       }
     }

     if(digitalRead(btnUp)==LOW && start==1) {
 /*     
       while(digitalRead(btnUp)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50); 
       }
 */      
     }

     if(digitalRead(btnDown)==LOW && start==1) {
/*
       while(digitalRead(btnDown)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
*/
     }


     if(start==1 && (!oldMotorState==motorState)) {  
       //if file is playing and motor control is on then handle current motor state
       //Motor control works by pulling the btnMotor pin to ground to play, and NC to stop
       if(motorState==1 && pauseOn==0) {
        printtextF(PSTR("Paused "),0);
        Counter1();
        
            #ifdef P8544 
              lcd.gotoRc(3,38);
              lcd.bitmap(Paused, 1, 6);
            #endif
         pauseOn = 1;
       } 
       if(motorState==0 && pauseOn==1) {
         printtextF(PSTR("Playing"),0);
         Counter1();
         
            #ifdef P8544
              lcd.gotoRc(3,38);
              lcd.bitmap(Play, 1, 6);              
            #endif
            
         pauseOn = 0;
       }
       oldMotorState=motorState;
     }
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
  //move up a file in the directory
  currentFile--;
  if(currentFile<1) {
    getMaxFile();
    currentFile = maxFile;
  }
  UP=1;   
  seekFile(currentFile);
}

void downFile() {    
  //move down a file in the directory
  currentFile++;
  if(currentFile>maxFile) { currentFile=1; }
  UP=0;  
  seekFile(currentFile);
}

void seekFile(int pos) {    
  //move to a set position in the directory, store the filename, and display the name on screen.
  if (UP==1) {  
    entry.cwd()->rewind();
    for(int i=1;i<=currentFile-1;i++) {
      entry.openNext(entry.cwd(),O_READ);
      entry.close();
    }
  }

  if (currentFile==1) {entry.cwd()->rewind();}
  entry.openNext(entry.cwd(),O_READ);
  entry.getName(fileName,filenameLength);
  entry.getSFN(sfileName);
  filesize = entry.fileSize();
  ayblklen = filesize + 3;  // add 3 file header, data byte and chksum byte to file length
  if(entry.isDir() || !strcmp(sfileName, "ROOT")) { isDir=1; } else { isDir=0; }
  entry.close();
  
  PlayBytes[0]='\0'; 
  if (isDir==1) {
    strcat_P(PlayBytes,PSTR(VERSION));
    #ifdef P8544
      printtext("                 ",3);
    #endif
  } else {
    //ltoa(filesize,PlayBytes,10);
    strcat_P(PlayBytes,PSTR("Select File.."));
    #ifdef P8544
      printtext("                 ",3);
    #endif
  }
  printtext(PlayBytes,0);

  scrollPos=0;
  scrollText(fileName);
}

void stopFile() {
  TZXStop();
  if(start==1){
    printtextF(PSTR("Stopped"),0);
    //lcd_clearline(0);
    //lcd.print(F("Stopped"));
    #ifdef P8544
      lcd.gotoRc(3,38);
      lcd.bitmap(Stop, 1, 6);
    #endif
    start=0;
  }
}

void playFile() {
  
  if(isDir==1) {
    //If selected file is a directory move into directory
    changeDir();
  } else {
    if(entry.cwd()->exists(sfileName)) {
      printtextF(PSTR("Playing         "),0);    
      scrollPos=0;
      if (PauseAtStart == false) pauseOn = 0;
      scrollText(fileName);
      currpct=100;
      lcdsegs=0;      
      TZXPlay(sfileName);           //Load using the short filename
        #ifdef P8544
          lcd.gotoRc(3,38);
          lcd.bitmap(Play, 1, 6);
        #endif
      start=1; 
      if (PauseAtStart == true) {
        printtextF(PSTR("Paused "),0);
      #ifdef P8544                                        
              lcd.gotoRc(3,38);
              lcd.bitmap(Paused, 1, 6);
      #endif
      pauseOn = 1;
      TZXPause();
      }       
    } else {
      printtextF(PSTR("No File Selected"),1);
      //lcd_clearline(1);
      //lcd.print(F("No File Selected"));
    }
  }
}

void getMaxFile() {    
  //gets the total files in the current directory and stores the number in maxFile
  
  entry.cwd()->rewind();
  maxFile=0;
  while(entry.openNext(entry.cwd(),O_READ)) {
    //entry.getName(fileName,filenameLength);
    entry.close();
    maxFile++;
  }
  entry.cwd()->rewind();
}



void changeDir() {    
  //change directory, if fileName="ROOT" then return to the root directory
  //SDFat has no easy way to move up a directory, so returning to root is the easiest way. 
  //each directory (except the root) must have a file called ROOT (no extension)
                      
  if(!strcmp(fileName, "ROOT")) {
    subdir=0;    
    sd.chdir(true);
  } else {
     //if (subdir >0) entry.cwd()->getName(prevSubDir[subdir-1],filenameLength); // Antes de cambiar  
     DirFilePos[subdir] = currentFile;
     sd.chdir(fileName, true);
     if (strlen(fileName) > subdirLength) {
      //entry.getSFN(sfileName);
      strcpy(prevSubDir[subdir], sfileName);
     }
     else {
      strcpy(prevSubDir[subdir], fileName);
     }
     subdir++;      
  }
  getMaxFile();
  currentFile=1;
  seekFile(currentFile);
}

void scrollText(char* text)
{
  #ifdef LCDSCREEN16x2
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,1);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif

  #ifdef RGBLCD
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,1);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif

  #ifdef OLED1306
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[17];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<16;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<16;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[16]='\0';
  printtext(outtext,1);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif

  #ifdef P8544
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[15];
  if(isDir) { outtext[0]= 0x3E; 
    for(int i=1;i<14;i++)
    {
      int p=i+scrollPos-1;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  } else { 
    for(int i=0;i<14;i++)
    {
      int p=i+scrollPos;
      if(p<strlen(text)) 
      {
        outtext[i]=text[p];
      } else {
        outtext[i]='\0';
      }
    }
  }
  outtext[14]='\0';
  printtext(outtext,1);
  //lcd_clearline(1);
  //lcd.print(outtext);
  #endif
}



void printtextF(const char* text, int l) {  //Print text to screen. 
  
  #ifdef SERIALSCREEN
  Serial.println(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif
  
  #ifdef LCDSCREEN16x2
    lcd.setCursor(0,l);
    lcd.print(F("                "));
    lcd.setCursor(0,l);
    lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif

  #ifdef RGBLCD
    lcd.setCursor(0,l);
    lcd.print(F("                "));
    lcd.setCursor(0,l);
    lcd.print(reinterpret_cast <const __FlashStringHelper *> (text));
  #endif

 #ifdef OLED1306
       char* space = PSTR("                ");
       strncpy_P(lineclr, space, 16);
      if ( l == 0 ) {
        strncpy_P(line0, text, 16);
        sendStrXY(lineclr,0,0);
      } else {
        strncpy_P(line1, text, 16);
        sendStrXY(lineclr,0,1);
      }
      sendStrXY(line0,0,0);
      sendStrXY(line1,0,1);
  #endif

  #ifdef P8544
    lcd.setCursor(0,l);
    lcd.print(F("              "));
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
    lcd.print(F("                "));
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif

  #ifdef RGBLCD
    lcd.setCursor(0,l);
    lcd.print(F("                "));
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif

   #ifdef OLED1306
      setXY(0,l);
      sendStr("                ");
      setXY(0,l);
      sendStr(text);
  #endif

  #ifdef P8544
    lcd.setCursor(0,l);
    lcd.print(F("              "));
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif   
   
}
