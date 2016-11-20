void clearBuffer()
{
  
  for(int i=0;i<=buffsize;i++)
  {
    wbuffer[i][0]=0;
    wbuffer[i][1]=0;
  } 
}

word TickToUs(word ticks) {
  return (word) ((((float) ticks)/3.5)+0.5);
}

void TZXPlay(char *filename) {
  Timer1.stop();                              //Stop timer interrupt
  if(!entry.open(filename,O_READ)) {          //open file and check for errors
    printtext("Error Opening File",0);
  }
  bytesRead=0;                                //start of file
  currentTask=GETFILEHEADER;                  //First task: search for header
  if(checkForTap(filename)) {                 //Check for Tap File.  As these have no header we can skip straight to playing data
    currentTask=PROCESSID;
    currentID=TAP;
    printtext("TAP Playing",0);
  }
  currentBlockTask = READPARAM;               //First block task is to read in parameters
  clearBuffer();
  isStopped=false;
  pinState=LOW;                               //Always Start on a LOW output for simplicity
  count = 255;                                //End of file buffer flush
  EndOfFile=false;
  digitalWrite(outputPin, pinState);
  Timer1.setPeriod(1000);                     //set 1ms wait at start of a file.
}

bool checkForTap(char *filename) {
  //Check for TAP file extensions as these have no header
  byte len = strlen(filename);
  if(strstr(strlwr(filename + (len-4)), ".tap")) {
    return true;
  }
  return false;
}

void TZXStop() {
  Timer1.stop();                              //Stop timer
  isStopped=true;
  entry.close();                              //Close file
  bytesRead=0;
}

void TZXPause() {
  isStopped=pauseOn;
}

void TZXLoop() {
    noInterrupts();                           //Pause interrupts to prevent var reads and copy values out
    copybuff = morebuff;
    morebuff = LOW;
    isStopped = pauseOn;
    interrupts();
    if(copybuff==HIGH) {
      btemppos=0;                             //Buffer has swapped, start from the beginning of the new page
      copybuff=LOW;
    }

    if(btemppos<=buffsize) 
    {
      TZXProcess();                           //generate the next period to add to the buffer
      if(currentPeriod>0) {
        noInterrupts();                       //Pause interrupts while we add a period to the buffer
        wbuffer[btemppos][workingBuffer ^ 1] = currentPeriod;   //add period to the buffer
        interrupts();
        btemppos+=1;
      }
    }
}

void TZXSetup() {
    pinMode(outputPin, OUTPUT);               //Set output pin
    digitalWrite(outputPin, LOW);             //Start output LOW
    isStopped=true;
    pinState = LOW;
    Timer1.initialize(100000);                //100ms pause prevents anything bad happening before we're ready
    Timer1.attachInterrupt(wave);
    Timer1.stop();                            //Stop the timer until we're ready
}

void TZXProcess() {
    byte r = 0;
    currentPeriod = 0;
    if(currentTask == GETFILEHEADER) {
      //grab 7 byte string
      ReadTZXHeader();
      //set current task to GETID
      currentTask = GETID;
    }
    if(currentTask == GETID) {
      //grab 1 byte ID
      if(ReadByte(bytesRead)==1) {
        currentID = outByte;
      } else {
        currentID = EOF;
      }
      //reset data block values
      currentBit=0;
      pass=0;
      //set current task to PROCESSID
      currentTask = PROCESSID;
      currentBlockTask = READPARAM;
    }
    if(currentTask == PROCESSID) {
      //ID Processing
      switch(currentID) {
        case ID10:
          //Process ID10 - Standard Block
          switch (currentBlockTask) {
            case READPARAM:
              if(r=ReadWord(bytesRead)==2) {
                pauseLength = outWord;
              }
              if(r=ReadWord(bytesRead)==2) {
                bytesToRead = outWord +1;
              }
              if(r=ReadByte(bytesRead)==1) {
                if(outByte == 0) {
                  pilotPulses = PILOTNUMBERL;
                } else {
                  pilotPulses = PILOTNUMBERH;
                }
                bytesRead += -1;
              }
              pilotLength = PILOTLENGTH;
              sync1Length = SYNCFIRST;
              sync2Length = SYNCSECOND;
              zeroPulse = ZEROPULSE;
              onePulse = ONEPULSE;
              currentBlockTask = PILOT;
              usedBitsInLastByte=8;
          break;
          
          default:
            StandardBlock();
          break;
          }

        break;
        
        case ID11:
          //Process ID11 - Turb Tape Block
          switch (currentBlockTask) {
            case READPARAM:
              if(r=ReadWord(bytesRead)==2) {
                pilotLength = TickToUs(outWord);
              }
              if(r=ReadWord(bytesRead)==2) {
                sync1Length = TickToUs(outWord);
              }
              if(r=ReadWord(bytesRead)==2) {
                sync2Length = TickToUs(outWord);
              }
              if(r=ReadWord(bytesRead)==2) {
                zeroPulse = TickToUs(outWord);
              }
              if(r=ReadWord(bytesRead)==2) {
                onePulse = TickToUs(outWord);
              }              
              if(r=ReadWord(bytesRead)==2) {
                pilotPulses = outWord;
              }
              if(r=ReadByte(bytesRead)==1) {
                usedBitsInLastByte = outByte;
              }
              if(r=ReadWord(bytesRead)==2) {
                pauseLength = outWord;
              }
              if(r=ReadLong(bytesRead)==3) {
                bytesToRead = outLong +1;
              }
              currentBlockTask = PILOT;
          break;
          
          default:
            StandardBlock();
          break;
          }

        break;
        case ID12:
        //Process ID12 - Pure Tone Block
          if(currentBlockTask==READPARAM){
            if(r=ReadWord(bytesRead)==2) {
               pilotLength = TickToUs(outWord);
            }
            if(r=ReadWord(bytesRead)==2) {
              pilotPulses = outWord;
              //DebugBlock("Pilot Pulses", pilotPulses);
            }
            currentBlockTask = PILOT;
          } else {
            PureToneBlock();
          }
        break;

        case ID13:
        //Process ID13 - Sequence of Pulses          
          if(currentBlockTask==READPARAM) {  
            if(r=ReadByte(bytesRead)==1) {
              seqPulses = outByte;
            }
            currentBlockTask = DATA;
          } else {
            PulseSequenceBlock();
          }
        break;

        case ID14:
          //process ID14 - Pure Data Block
          if(currentBlockTask==READPARAM) {
            if(r=ReadWord(bytesRead)==2) {
              zeroPulse = TickToUs(outWord); 
            }
            if(r=ReadWord(bytesRead)==2) {
              onePulse = TickToUs(outWord); 
            }
            if(r=ReadByte(bytesRead)==1) {
              usedBitsInLastByte = outByte;
            }
            if(r=ReadWord(bytesRead)==2) {
              pauseLength = outWord; 
            }
            if(r=ReadLong(bytesRead)==3) {
              bytesToRead = outLong+1;
            }
            currentBlockTask=DATA;
          } else {
            PureDataBlock();
          }
        break;

        case ID20:
          //process ID20 - Pause Block
          if(r=ReadWord(bytesRead)==2) {
            if(outWord>0) {
              currentPeriod = pauseLength;
              bitSet(currentPeriod, 15);
            }
            currentTask=GETID;
          }
        break;

        case ID21:
          //Process ID21 - Group Start
          if(r=ReadByte(bytesRead)==1) {
            bytesRead += outByte;
          }
          currentTask = GETID;
        break;

        case ID22:
          //Process ID22 - Group End
          currentTask = GETID;
        break;

        case ID24:
          //Process ID24 - Loop Start
          if(r=ReadWord(bytesRead)==2) {
            loopCount = outWord;
            loopStart = bytesRead;
          }
          currentTask = GETID;
        break;

        case ID25:
          //Process ID25 - Loop End
          loopCount += -1;
          if(loopCount!=0) {
            bytesRead = loopStart;
          } 
          currentTask = GETID;
        break;
        
        case ID30:
          //Process ID30 - Text Description
          if(r=ReadByte(bytesRead)==1) {
            //Show info on screen - removed until bigger screen used
            //byte j = outByte;
            //for(byte i=0; i<j; i++) {
            //  if(ReadByte(bytesRead)==1) {
            //    lcd.print(char(outByte));
            //  }
            //}
            bytesRead += outByte;
          }
          currentTask = GETID;
        break;

        case ID32:
          //Process ID32 - Archive Info
          //Block Skipped until larger screen used
          if(ReadWord(bytesRead)==2) {
            bytesRead += outWord;
          }
          currentTask = GETID;
        break;

        case ID33:
          //Process ID32 - Archive Info
          //Block Skipped until larger screen used
          if(ReadByte(bytesRead)==1) {
            bytesRead += (long(outByte) * 3);
          }
          currentTask = GETID;
        break;       

        case ID35:
          //Process ID35 - Custom Info Block
          //Block Skipped
          bytesRead += 10;
          if(r=ReadDword(bytesRead)==4) {
            bytesRead += outLong;
          }
        break;
        
        case TAP:
          //Pure Tap file block
          switch(currentBlockTask) {
            case READPARAM:
              pauseLength = PAUSELENGTH;
              if(r=ReadWord(bytesRead)==2) {
                    bytesToRead = outWord+1;
              }
              if(r=ReadByte(bytesRead)==1) {
                if(outByte == 0) {
                  pilotPulses = PILOTNUMBERL + 1;
                } else {
                  pilotPulses = PILOTNUMBERH + 1;
                }
                bytesRead += -1;
              }
              pilotLength = PILOTLENGTH;
              sync1Length = SYNCFIRST;
              sync2Length = SYNCSECOND;
              zeroPulse = ZEROPULSE;
              onePulse = ONEPULSE;
              currentBlockTask = PILOT;
              usedBitsInLastByte=8;
            break;

            default:
              StandardBlock();
            break;
          }
          
        break;

        case EOF:
          //Handle end of file
          if(!count==0) {
            currentPeriod = 32769;
            count += -1;
          } else {
            stopFile();
            return;
          }
        break; 
        
        default:
          //stopFile();
          //ID Not Recognised - Fall back if non TZX file or unrecognised ID occurs
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("ID? ");
          lcd.setCursor(4,0);
          lcd.print(String(currentID, HEX));
          lcd.setCursor(0,1);
          lcd.print(String(bytesRead,HEX) + " - L: " + String(loopCount, DEC));
          delay(5000);
          stopFile();
        break;
      }
    }
}

void StandardBlock() {
  //Standard Block Playback
  switch (currentBlockTask) {
    case PILOT:
        //Start with Pilot Pulses
        currentPeriod = pilotLength;
        pilotPulses += -1;
        if(pilotPulses == 0) {
          currentBlockTask = SYNC1;
        }
    break;
    
    case SYNC1:
      //First Sync Pulse
      currentPeriod = sync1Length;
      currentBlockTask = SYNC2;
    break;
    
    case SYNC2:
      //Second Sync Pulse
      currentPeriod = sync2Length;
      currentBlockTask = DATA;
    break;
    
    case DATA:  
      //Data Playback    
      writeData();  
    break;
    
    case PAUSE:
      //Close block with a pause      
      currentPeriod = pauseLength;
      bitSet(currentPeriod, 15);
      if(currentID!=TAP) {
        currentTask = GETID;
      } else {
        currentBlockTask = READPARAM;
      }
      if(EndOfFile==true) currentID=EOF;
    break;
  }
}

void PureToneBlock() {
  //Pure Tone Block - Long string of pulses with the same length
  currentPeriod = pilotLength;
  pilotPulses += -1;
  if(pilotPulses==0) {
    currentTask = GETID;
  }
}

void PulseSequenceBlock() {
  //Pulse Sequence Block - String of pulses each with a different length
  //Mainly used in speedload blocks
  byte r=0;
  if(r=ReadWord(bytesRead)==2) {
    currentPeriod = TickToUs(outWord);    
  }
  seqPulses += -1;
  if(seqPulses==0) {
    currentTask = GETID;
  }
}

void PureDataBlock() {
  //Pure Data Block - Data & pause only, no header, sync
  switch(currentBlockTask) {
    case DATA:
      writeData();          
    break;
    
    case PAUSE:
      currentPeriod = pauseLength;
      bitSet(currentPeriod, 15);
      currentTask=GETID;
      if(EndOfFile==true) currentID=EOF;
    break;
  }
  
}

void writeData() {
  //Convert byte from file into string of pulses.  One pulse per pass
  byte r;
  if(currentBit==0) {                         //Check for byte end/first byte
    if(r=ReadByte(bytesRead)==1) {            //Read in a byte
      currentByte = outByte;
      bytesToRead += -1;
      if(bytesToRead == 0) {                  //Check for end of data block
        bytesRead += -1;                      //rewind a byte if we've reached the end
        if(pauseLength==0) {                  //Search for next ID if there is no pause
          currentTask = GETID;
        } else {
          currentBlockTask = PAUSE;           //Otherwise start the pause
        }
        return;
      }
    } else if(r==0) {
      EndOfFile=true;
      if(pauseLength==0) {
        currentTask = GETID;
      } else {
        currentBlockTask = PAUSE;
      }
      return;
    }
    if(bytesToRead!=1) {                      //If we're not reading the last byte play all 8 bits
      currentBit=8;
    } else {
      currentBit=usedBitsInLastByte;          //Otherwise only play back the bits needed
    }
    pass=0;
  } 
 if(currentByte&0x80) {                       //Set next period depending on value of bit 0
    currentPeriod = onePulse;
  } else {
    currentPeriod = zeroPulse;
  }
  pass+=1;                                    //Data is played as 2 x pulses
  if(pass==2) {
    currentByte <<= 1;                        //Shift along to the next bit
    currentBit += -1;
    pass=0;  
  }    
}

void wave() {
  //ISR Output routine
  unsigned long fudgeTime = micros();         //fudgeTime is used to reduce length of the next period by the time taken to process the ISR
  word workingPeriod = wbuffer[pos][workingBuffer];
  byte pauseFlipBit = false;
  unsigned long newTime=1;
  intError = false;
  if(isStopped==0 && workingPeriod >= 1)
  {
      if(bitRead(workingPeriod, 15))          
      {
        //If bit 15 of the current period is set we're about to run a pause
        //Pauses start with a 1.5ms where the output is untouched after which the output is set LOW
        //Pause block periods are stored in milliseconds not microseconds
        isPauseBlock = true;
        bitClear(workingPeriod,15);         //Clear pause block flag
        pinState = !pinState;
        pauseFlipBit = true;
        wasPauseBlock = true;
      } else {
        if(workingPeriod >= 1 && wasPauseBlock==false) {
          pinState = !pinState;
        } else if (wasPauseBlock==true && isPauseBlock==false) {
          wasPauseBlock=false;
        }
      }
      digitalWrite(outputPin, pinState);
      if(pauseFlipBit==true) {
        newTime = 1500;                     //Set 1.5ms initial pause block
        pinState = LOW;                     //Set next pinstate LOW
        wbuffer[pos][workingBuffer] = workingPeriod - 1;  //reduce pause by 1ms as we've already pause for 1.5ms
        pauseFlipBit=false;
      } else {
        if(isPauseBlock==true) {
          newTime = long(workingPeriod)*1000; //Set pause length in microseconds
          isPauseBlock = false;
        } else {
          newTime = workingPeriod;          //After all that, if it's not a pause block set the pulse period 
        }
        pos += 1;
        if(pos > buffsize)                  //Swap buffer pages if we've reached the end
        {
          pos = 0;
          workingBuffer^=1;
          morebuff = HIGH;                  //Request more data to fill inactive page
        } 
     }
  } else if(workingPeriod <= 1 && isStopped==0) {
    newTime = 1000;                         //Just in case we have a 0 in the buffer
    pos += 1;
    if(pos > buffsize) {
      pos = 0;
      workingBuffer ^= 1;
      morebuff = HIGH;
    }
  } else {
    newTime = 1000;                         //Just in case we have a 0 in the buffer
  }
  newTime += 12;                            //extra fudging, for some reason the pulses on the nano i used were always 12us short
  fudgeTime = micros() - fudgeTime;         //Compensate for stupidly long ISR
  Timer1.setPeriod(newTime - fudgeTime);    //Finally set the next pulse length
}

int ReadByte(unsigned long pos) {
  //Read a byte from the file, and move file position on one if successful
  byte out[1];
  int i=0;
  if(entry.seekSet(pos)) {
    i = entry.read(out,1);
    if(i==1) bytesRead += 1;
  }
  outByte = out[0];
  return i;
}

int ReadWord(unsigned long pos) {
  //Read 2 bytes from the file, and move file position on two if successful
  byte out[2];
  int i=0;
  if(entry.seekSet(pos)) {
    i = entry.read(out,2);
    if(i==2) bytesRead += 2;
  }
  outWord = word(out[1],out[0]);
  return i;
}

int ReadLong(unsigned long pos) {
  //Read 3 bytes from the file, and move file position on three if successful
  byte out[3];
  int i=0;
  if(entry.seekSet(pos)) {
    i = entry.read(out,3);
    if(i==3) bytesRead += 3;
  }
  outLong = (word(out[2],out[1]) << 8) | out[0];
  return i;
}

int ReadDword(unsigned long pos) {
  //Read 4 bytes from the file, and move file position on four if successful  
  byte out[4];
  int i=0;
  if(entry.seekSet(pos)) {
    i = entry.read(out,4);
    if(i==4) bytesRead += 4;
  }
  outLong = (word(out[3],out[2]) << 16) | word(out[1],out[0]);
  return i;
}

void ReadTZXHeader() {
  //Read and check first 10 bytes for a TZX header
  char tzxHeader[11];
  int i=0;
  
  if(entry.seekSet(0)) {
    i = entry.read(tzxHeader,10);
    if(memcmp(tzxHeader,TZXTape,7)!=0) {
      printtext("Not TZXTape",1);
      TZXStop();
    }
  } else {
    printtext("Error Reading File",0);
  }
  bytesRead = 10;
}

/*  
void DebugBlock(String title, unsigned long number) {
  //Used to debug errors while coding updates - Simply displays text above a value on the lcd
  stopFile();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(title);
  lcd.setCursor(0,1);
  lcd.print(number,DEC);
  delay(500);
}*/


