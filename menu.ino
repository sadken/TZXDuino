/**************************************************************
 *                  TZXDuino Menu Code:
 *  Menu Button (was motor controll button) opens menu
 *  up/down move through menu, play select, stop back
 *  Menu Options:
 *  TSX Baud Rate:
 *    1200
 *    2400
 *    3600
 *  
 *  Turbo Boost:      Speeds up ZX80/81 TZX/O/P files and Electron UEF files TURN OFF IF NOT USING THESE FILES
 *    On
 *    Off
 *  
 *  Pause @ Start
 *    On
 *    Off
 *    
 *  Gremlin Loader
 *    On
 *    Off
 *    
 *  Save settings to eeprom on exit. 
 */

#ifdef HAVE_EEPROM
#include <EEPROM.h>
#endif

 const char str_on[] PROGMEM = "On";
 const char str_off[] PROGMEM = "Off";
 const char str_on_star[] PROGMEM = "On  *";
 const char str_off_star[] PROGMEM = "Off *";

 const char str_tsx_baud_rate[] PROGMEM = "TSX Baud Rate";
 const char str_turbo_boost[] PROGMEM = "Turbo Boost";
 const char str_pause_at_start[] PROGMEM = "Pause @ Start";
 const char str_gremlin_loader[] PROGMEM = "Gremlin Loader";

void doOnOffSubmenu(const char * const submenu_heading, bool &flag)
{
  // submenu code optimised for an 'On/Off' submenu, which has only two
  // states (on or off)
  //lcd_clearline(0);
  printtextF(submenu_heading,0);
  bool subItemOnOff=true; // true -> showing the 'On' value, false -> showing the 'Off' value
  bool updateScreen=true;
  bool lastbtn=true;
  while(!button_stop() || lastbtn) {
    if(updateScreen) {
      //lcd_clearline(1);
      if (subItemOnOff) {
        if (flag)
          printtextF(str_on_star,1);
        else
          printtextF(str_on,1);
      } else {
        if (flag)
          printtextF(str_off,1);
        else
          printtextF(str_off_star,1);
      }
      updateScreen=false;
    }

    if (!lastbtn) {
      if(button_down()){
        subItemOnOff=false;
        lastbtn=true;
        updateScreen=true;
      }
      if(button_up()) {
        subItemOnOff=true;
        lastbtn=true;
        updateScreen=true;
      }
      if(button_play()) {
        flag = subItemOnOff;
        lastbtn=true;
        updateScreen=true;
      }
    }
    if(!button_down() && !button_up() && !button_play() && !button_stop()) lastbtn=false;
  }
}

void menuMode()
{
  bool lastbtn=true;
  byte menuItem=0;
  byte subItem=0;
  bool updateScreen=true;
  
  while(!button_stop() || lastbtn)
  {
    if(updateScreen) {
      ////lcd_clearline(0);
      printtextF(PSTR("Menu Screen"),0);
      ////lcd_clearline(1);
      switch(menuItem) {
        
        case 0:
        printtextF(str_tsx_baud_rate,1);
        break;
        
        case 1:
        printtextF(str_turbo_boost,1);
        break;
        
        case 2:
        printtextF(str_pause_at_start,1);
        break;

        case 3:
        printtextF(str_gremlin_loader,1);
        break;
        
      }
      updateScreen=false;
    }
    
    if(button_down() && !lastbtn){
      if(menuItem<3) menuItem+=1;
      lastbtn=true;
      updateScreen=true;
    }
    if(button_up() && !lastbtn) {
      if(menuItem>0) menuItem+=-1;
      lastbtn=true;
      updateScreen=true;
    }

    
    if(button_play() && !lastbtn) {
      switch(menuItem){
        
        case 0:
          subItem=0;
          updateScreen=true;
          lastbtn=true;
          while(!button_stop() || lastbtn) {
            if(updateScreen) {
              ////lcd_clearline(0);
              printtextF(str_tsx_baud_rate,0);
              ////lcd_clearline(1);
              switch(subItem) {
                case 0:                                  
                printtextF(PSTR("1200 Baud"),1);
                if(BAUDRATE==1200) printtextF(PSTR("1200 Baud *     "),1);
                break;
                case 1:        
                  printtextF(PSTR("2400 Baud"),1);
                  if(BAUDRATE==2400) printtextF(PSTR("2400 Baud *     "),1);
                break;
                case 2:                  
                printtextF(PSTR("3600 Baud"),1);
                if(BAUDRATE==3600) printtextF(PSTR("3600 Baud *     "),1);
                break;                
              }
              updateScreen=false;
            }

            if(!lastbtn) {
              if(button_down()){
                if(subItem<2) subItem+=1;
                lastbtn=true;
                updateScreen=true;
              }
              if(button_up()) {
                if(subItem>0) subItem+=-1;
                lastbtn=true;
                updateScreen=true;
              }
              if(button_play()) {
                switch(subItem) {
                  case 0:
                    BAUDRATE=1200;
                  break;
                  case 1:
                    BAUDRATE=2400;
                  break;
                  case 2:
                    BAUDRATE=3600;
                  break;
                }
                updateScreen=true;
                lastbtn=true;
              }
            }
            if(!button_down() && !button_up() && !button_play() && !button_stop()) lastbtn=false;
          }
          lastbtn=true;
          updateScreen=true;
        break;
        
        case 1:
          doOnOffSubmenu(str_turbo_boost, uefTurboMode);
          lastbtn=true;
          updateScreen=true;
        break;

        case 2:
          doOnOffSubmenu(str_pause_at_start, PauseAtStart);
          lastbtn=true;
          updateScreen=true;
        break;

        case 3:
          doOnOffSubmenu(str_gremlin_loader, FlipPolarity);
          lastbtn=true;
          updateScreen=true;
        break;
        
      }
    }
    if(!button_down() && !button_up()&& !button_play() && !button_stop()) lastbtn=false;
  }
  updateEEPROM();
}

void updateEEPROM()
{
#ifdef HAVE_EEPROM
  /* Setting Byte: 
   *  bit 0: 1200
   *  bit 1: 2400
   *  bit 2: 3600
   *  bit 3: n/a
   *  bit 4: n/a
   *  bit 5: Pause @ Start
   *  bit 6: Gremlin Loader
   *  bit 7: UEFTurbo
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
  }
  if(PauseAtStart) settings |=32;
  if(FlipPolarity) settings |=64;
  if(uefTurboMode) settings |=128;
  EEPROM.put(0,settings);
#endif
}

void loadEEPROM()
{
#ifdef HAVE_EEPROM
  byte settings=0;
  EEPROM.get(0,settings);
  if(!settings) return;
  
  if(bitRead(settings,7)) {
    uefTurboMode=true;
  } else {
    uefTurboMode=false;
  }

  if(bitRead(settings,6)) {
    FlipPolarity=true;
  } else {
    FlipPolarity=false;
  }

  if(bitRead(settings,5)) {
    PauseAtStart=true;
  } else {
    PauseAtStart=false;
  }
  
  if(bitRead(settings,0)) {
    BAUDRATE=1200;
  }
  if(bitRead(settings,1)) {
    BAUDRATE=2400;
  }
  if(bitRead(settings,2)) {
    BAUDRATE=3600;
  }
  //setBaud();
#endif
}
