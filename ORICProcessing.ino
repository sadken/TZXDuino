void OricDataBlock() {  
    //Convert byte from file into string of pulses.  One pulse per pass 
    byte r; 
    if(currentBit==0) {                         //Check for byte end/first byte 
        
      if(r=ReadByte(bytesRead)==1) {            //Read in a byte  
        currentByte = outByte;  
        bytesToRead += -1;  
        bitChecksum = 0;  
        if(bytesToRead == 0) {                  //Check for end of data block 
          lastByte = 1; 
          //if(pauseLength==0) {                  //Search for next ID if there is no pause 
            //currentTask = IDEOF;  
          //} else {  
            //currentBlockTask = PAUSE;           //Otherwise start the pause 
          //} 
          //return;                               // exit 
        } 
      } else if(r==0) {                         // If we reached the EOF  
        EndOfFile=true; 
        temppause = 0;  
        forcePause0=1;  
        count =255; 
        currentID = IDPAUSE;  
        //currentBlockTask = GAP; 
        //currentTask = IDEOF;  
          
        return; 
      } 
    
      currentBit = 11;  
      pass=0; 
    } 
    OricBitWrite(); 
    
  } 
    
  void OricBitWrite(){  
    if (currentBit == 11) { //Start Bit 
        //currentPeriod = ORICZEROPULSE;  
        if (pass==0) currentPeriod = ORICZEROLOWPULSE;  
        if (pass==1) currentPeriod = ORICZEROHIGHPULSE;           
    } else if (currentBit == 2) { // Paridad inversa i.e. Impar 
        //currentPeriod =  bitChecksum ? ORICONEPULSE : ORICZEROPULSE;  
        if (pass==0)  currentPeriod = bitChecksum ? ORICZEROLOWPULSE : ORICONEPULSE;  
        if (pass==1)  currentPeriod = bitChecksum ? ORICZEROHIGHPULSE : ORICONEPULSE;       
    } else if (currentBit == 1) { 
        currentPeriod = ORICONEPULSE;     
    } else {  
      if(currentByte&0x01) {                       //Set next period depending on value of bit 0  
        currentPeriod = ORICONEPULSE;           
      } else {  
        //currentPeriod = ORICZEROPULSE;  
        if (pass==0)  currentPeriod = ORICZEROLOWPULSE;   
        if (pass==1)  currentPeriod = ORICZEROHIGHPULSE;          
      } 
    }     
    
    pass+=1;      //Data is played as 2 x pulses for a zero, and 2 pulses for a one 
    
      if (currentPeriod == ORICONEPULSE) {  
      // must be a one pulse  
            
  /*    if(pass==2) { 
          
        if ((currentBit>2) && (currentBit<11)) {  
          bitChecksum ^= 1; 
          currentByte >>= 1;                        //Shift along to the next bit 
        } 
          
        currentBit += -1; 
        pass=0;   
        if ((lastByte) && (currentBit==0)) {  
          //currentTask = GETCHUNKID; 
          currentBlockTask = PAUSE; 
        }   
      } */  
        if ((currentBit>2) && (currentBit<11) && (pass==2)) { 
              bitChecksum ^= 1; 
              currentByte >>= 1;                        //Shift along to the next bit 
              currentBit += -1; 
              pass=0;                           
        } 
        if ((currentBit==1) && (pass==6)) { 
              currentBit += -1; 
              pass=0;           
        } 
        if (((currentBit==2) || (currentBit==11))  && (pass==2)) {  
              currentBit += -1; 
              pass=0;           
        } 
        if ((currentBit==0) && (lastByte) ) { 
          //currentTask = GETCHUNKID; 
          count = 255;  
          currentBlockTask = PAUSE; 
        }               
    } 
    else {  
      // must be a zero pulse 
      if(pass==2) { 
         if ((currentBit>2) && (currentBit<11)) { 
            currentByte >>= 1;                        //Shift along to the next bit 
         }  
         currentBit += -1;  
         pass=0;  
         if ((currentBit==0) && (lastByte)) { 
           //currentTask = GETCHUNKID;  
           count = 255; 
           currentBlockTask = PAUSE;  
         }  
      } 
      
    } 
      
  } 


  
