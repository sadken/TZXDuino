// ---------------------------------------------------------------------------------
// DO NOT USE CLASS-10 CARDS on this project - they're too fast to operate using SPI
// ---------------------------------------------------------------------------------
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
 */

#include <SdFat.h>
#include <TimerOne.h>
#include "TZXDuino.h"

//Set defines for various types of screen, currently only 16x2 I2C LCD is supported
#define LCDSCREEN16x2   1

#ifdef LCDSCREEN16x2
  #include <Wire.h> 
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x3F,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display
#endif

SdFat sd;                           //Initialise Sd card 

SdFile entry;                       //SD card file

#define filenameLength    100       //Maximum length for scrolling filename

char fileName[filenameLength+1];    //Current filename
char sfileName[13];                 //Short filename variable

const int chipSelect = 10;          //Sd card chip select pin

#define btnPlay       17            //Play Button
#define btnStop       16            //Stop Button
#define btnUp         15            //Menu Up button
#define btnDown       14            //Menu Down button
#define btnMotor      6             //Motor Sense (connect pin to gnd to play, NC for pause)
#define btnRoot       7             //Return to SD card root
#define scrollSpeed   250           //text scroll delay
#define scrollWait    3000          //Delay before scrolling starts
byte scrollPos=0;                   //Stores scrolling text position
unsigned long scrollTime=millis()+scrollWait;

byte mselectState = 0;              //Motor control state 1=on 0=off
byte motorState = 1;                //Current motor control state
byte oldMotorState = 1;             //Last motor control state
byte start = 0;                     //Currently playing flag
byte pauseOn = 0;                   //Pause state
int currentFile = 1;                //Current position in directory
int maxFile = 0;                    //Total number of files in directory
byte isDir = 0;                     //Is the current file a directory
unsigned long timeDiff = 0;         //button debounce

void setup() {
  #ifdef LCDSCREEN16x2
    lcd.init();                     //Initialise LCD (16x2 type)
    lcd.backlight();
  #endif
  #ifdef SERIALSCREEN
    Serial.begin(115200);
  #endif
  pinMode(chipSelect, OUTPUT);      //Setup SD card chipselect pin
  if (!sd.begin(chipSelect,SPI_FULL_SPEED)) {  
    //Start SD card and check it's working
    printtext("No SD Card",0);
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
  getMaxFile();                     //get the total number of files in the directory
  seekFile(currentFile);            //move to the first file in the directory
  printtext("Ready v1.0",0);
}

void loop(void) {
  if(start==1)
  {
    //TZXLoop only runs if a file is playing, and keeps the buffer full.
    TZXLoop();
  } else digitalWrite(outputPin, LOW);    //Keep output LOW while no file is playing.
  
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
            printtext("Paused",0);
            pauseOn = 1;
          } else {
            printtext("Playing",0);
            pauseOn = 0;
          }
       }
       while(digitalRead(btnPlay)==LOW) {
        //prevent button repeats by waiting until the button is released.
        delay(50);
       }
     }
     if(digitalRead(btnRoot)==LOW && start==0){
       //Return to root of the SD card.
       sd.chdir(true);
       getMaxFile();
       currentFile=1;
       seekFile(currentFile);  
       while(digitalRead(btnRoot)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
     }
     if(digitalRead(btnStop)==LOW) {
       stopFile();
     }
     if(digitalRead(btnUp)==LOW && start==0) {
       //Move up a file in the directory
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       upFile();       
       while(digitalRead(btnUp)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50); 
       }
     }
     if(digitalRead(btnDown)==LOW && start==0) {
       //Move down a file in the directory
       scrollTime=millis()+scrollWait;
       scrollPos=0;
       downFile();
       while(digitalRead(btnDown)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
     }
     if(start==1 && (!oldMotorState==motorState)) {  
       //if file is playing and motor control is on then handle current motor state
       //Motor control works by pulling the btnMotor pin to ground to play, and NC to stop
       if(motorState==1 && pauseOn==0) {
         printtext("Paused",0);
         pauseOn = 1;
       } 
       if(motorState==0 && pauseOn==1) {
         printtext("Playing",0);
         pauseOn = 0;
       }
       oldMotorState=motorState;
     }
  }
}

void upFile() {    
  //move up a file in the directory
  currentFile--;
  if(currentFile<1) {
    getMaxFile();
    currentFile = maxFile;
  } 
  seekFile(currentFile);
}

void downFile() {    
  //move down a file in the directory
  currentFile++;
  if(currentFile>maxFile) { currentFile=1; }
  seekFile(currentFile);
}

void seekFile(int pos) {    
  //move to a set position in the directory, store the filename, and display the name on screen.
  entry.cwd()->rewind();
  for(int i=1;i<=currentFile;i++) {
    entry.openNext(entry.cwd(),O_READ);
    entry.getName(fileName,filenameLength);
    entry.getSFN(sfileName);
    if(entry.isDir() || !strcmp(sfileName, "ROOT")) { isDir=1; } else { isDir=0; }
    entry.close();
  }
  scrollPos=0;
  scrollText(fileName);
}

void stopFile() {
  TZXStop();
  if(start==1){
    printtext("Stopped",0);
    start=0;
  }
}

void playFile() {
  if(isDir==1) {
    //If selected file is a directory move into directory
    changeDir();
  } else {
    if(entry.cwd()->exists(sfileName)) {
      printtext("Playing",0);
      scrollPos=0;
      pauseOn = 0;
      scrollText(fileName);
      TZXPlay(sfileName);           //Load using the short filename
      start=1;       
    } else {
      printtext("No File Selected",1);
    }
  }
}

void getMaxFile() {    
  //gets the total files in the current directory and stores the number in maxFile
  
  entry.cwd()->rewind();
  maxFile=0;
  while(entry.openNext(entry.cwd(),O_READ)) {
    entry.getName(fileName,filenameLength);
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
    sd.chdir(true);
  } else {
    sd.chdir(fileName, true);
  }
  getMaxFile();
  currentFile=1;
  seekFile(currentFile);
}

void scrollText(char* text)
{
  //Text scrolling routine.  Setup for 16x2 screen so will only display 16 chars
  if(scrollPos<0) scrollPos=0;
  char outtext[16];
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
  printtext(outtext,1);
}

void printtext(char* text, int l) {
  //Print text to screen.  
  #ifdef LCDSCREEN16x2
    lcd.setCursor(0,l);
    lcd.print("                    ");
    lcd.setCursor(0,l);
    lcd.print(text);
  #endif

}
