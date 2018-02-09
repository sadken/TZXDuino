/*
void menuMode()
 {
      //Return to root of the SD card.
       sd.chdir(true);
       getMaxFile();
       currentFile=1;
       seekFile(currentFile);  
       while(digitalRead(btnMselect)==LOW) {
         //prevent button repeats by waiting until the button is released.
         delay(50);
       }
}
*/
/**************************************************************
 *                  Casduino Menu Code:
 *  Menu Button (was motor controll button) opens menu
 *  up/down move through menu, play select, stop back
 *  Menu Options:
 *  Baud:
 *    1200
 *    2400
 *    2700
 *    3600
 *  
 *  MotorControl:
 *    On
 *    Off
 *  
 *  Save settings to eeprom on exit. 
 */

 
 void menuMode()
 { 
  byte lastbtn=true;
  byte menuItem=0;
  
  byte updateScreen=true;
  
  while(digitalRead(btnStop)==HIGH || lastbtn)
  {
    if(updateScreen) {
      printtextF(PSTR("TSX Baud Rate "),0);
      switch(menuItem) {
        case 0:                                  
                  printtextF(PSTR("1200 Baud       "),1);
                  if(BAUDRATE==1200) printtextF(PSTR("1200 Baud  * "),1);
                break;
                case 1:        
                  printtextF(PSTR("2400 Baud       "),1);
                  if(BAUDRATE==2400) printtextF(PSTR("2400 Baud  * "),1);
                break;
                                
                case 2:                  
                  printtextF(PSTR("3600 Baud       "),1);
                  if(BAUDRATE==3600) printtextF(PSTR("3600 Baud  * "),1);
                break;
                case 3:                  
                  printtextF(PSTR("3675 Baud       "),1);
                  if(BAUDRATE==3675) printtextF(PSTR("3675 Baud  * "),1);
                break;
                             
              }
              updateScreen=false;
            }
                    
            if(digitalRead(btnDown)==LOW && !lastbtn){
              if(menuItem<4) menuItem+=1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnUp)==LOW && !lastbtn) {
              if(menuItem>0) menuItem+=-1;
              lastbtn=true;
              updateScreen=true;
            }
            if(digitalRead(btnPlay)==LOW && !lastbtn) {
              switch(menuItem) {
                case 0:
                  BAUDRATE=1200;
                break;
                case 1:
                  BAUDRATE=2400;
                break;
                                
                case 2:
                  BAUDRATE=3600;
                break;
                case 3:
                  BAUDRATE=3675;                  
                
              
        break; 
      }
      updateScreen=true;
    }
    
    
      
    
    if(digitalRead(btnDown) && digitalRead(btnUp) && digitalRead(btnPlay) && digitalRead(btnStop)) lastbtn=false;
  }
  updateEEPROM();
 }

 void updateEEPROM()
 {
  /* Setting Byte: 
   *  bit 0: 1200
   *  bit 1: 2400
   *  bit 2: 3600
   *  bit 3: 3675
   */
  byte settings=0;

  switch(BAUDRATE) {
    case 1200:
    settings |=1;
    break;
    case 2400:
    settings |=2;
    break;
         
    case 3600:
    settings |=4;
    break;     
    case 3675:
    settings |=8;     
    break;
  }
  
  EEPROM.put(1,settings);
 }

 void loadEEPROM()
 {
  byte settings=0;
  EEPROM.get(1,settings);
  if(!settings) return;
  
  if(bitRead(settings,0)) {
    BAUDRATE=1200;
  }
  if(bitRead(settings,1)) {
    BAUDRATE=2400;
  }
  
  if(bitRead(settings,2)) {
    BAUDRATE=3600;  
  }
  if(bitRead(settings,3)) {
    BAUDRATE=3675;  
  }
  //setBaud();  
  TZXSetup();
 
 }

