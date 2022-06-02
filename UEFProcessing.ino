
void ReadUEFHeader() {
  //Read and check first 12 bytes for a UEF header
  char uefHeader[9];
  byte i=0;
  
  if(entry.seekSet(0)) {
    i = entry.read(uefHeader,9);
    if(memcmp_P(uefHeader,UEFFile,9)!=0) {
      printtextF(PSTR("Not UEF File"),1);
      TZXStop();
    }
  } else {
    printtextF(PSTR("Error Reading File"),0);
  }
  bytesRead =12;
}

void UEFCarrierToneBlock() {
  //Pure Tone Block - Long string of pulses with the same length
  currentPeriod = pilotLength;
  pilotPulses += -1;
  if(pilotPulses==0) {
    currentTask = GETCHUNKID;
  }
}

void writeUEFData() {

  //Convert byte from file into string of pulses.  One pulse per pass
  byte r;
  if(currentBit==0) {                         //Check for byte end/first byte


    if(r=ReadByte(bytesRead)==1) {            //Read in a byte
      currentByte = outByte;
          //itoa(currentByte,PlayBytes,16); printtext(PlayBytes,lineaxy);
      bytesToRead += -1;
      bitChecksum = 0;

      //blkchksum = blkchksum ^ currentByte;    // keep calculating checksum
      if(bytesToRead == 0) {                  //Check for end of data block
        lastByte = 1;
        //Serial.println(F("  Rewind bytesRead"));
        //bytesRead += -1;                      //rewind a byte if we've reached the end
        if(pauseLength==0) {                  //Search for next ID if there is no pause
          currentTask = PROCESSCHUNKID;
        } else {
          currentBlockTask = PAUSE;           //Otherwise start the pause
        }
        //return;                               // exit
      }
    } else if(r==0) {                         // If we reached the EOF
      currentTask = GETCHUNKID;
    }

    currentBit = 11;
    pass=0;
  }
  if ((currentBit == 2) && (parity == 0))currentBit = 1; // parity N
  if (currentBit == 11) {
    currentPeriod = zeroPulse;
  } else if (currentBit == 2) {
    //itoa(bitChecksum,PlayBytes,16);printtext(PlayBytes,lineaxy);
    currentPeriod = (bitChecksum ^ (parity & 0x01)) ? onePulse : zeroPulse;
    //currentPeriod =  bitChecksum ? onePulse : zeroPulse;
  } else if (currentBit == 1) {
      currentPeriod = onePulse;
  } else {
    if(currentByte&0x01) {                       //Set next period depending on value of bit 0
      currentPeriod = onePulse;
    } else {
      currentPeriod = zeroPulse;
    }

  }
  pass+=1;      //Data is played as 2 x pulses for a zero, and 4 pulses for a one when speed is 1200

  if (currentPeriod == zeroPulse) {
    if(pass==passforZero) {
       if ((currentBit>1) && (currentBit<11)) {
          currentByte >>= 1;                        //Shift along to the next bit
       }
       currentBit += -1;
       pass=0;
       if ((lastByte) && (currentBit==0)) {
         currentTask = GETCHUNKID;
       }
    }
  } else {
    // must be a one pulse
    if(pass==passforOne) {
      if ((currentBit>1) && (currentBit<11)) {
        bitChecksum ^= 1;
        currentByte >>= 1;                        //Shift along to the next bit
      }

      currentBit += -1;
      pass=0;
      if ((lastByte) && (currentBit==0)) {
        currentTask = GETCHUNKID;
      }
    }
  }


}
