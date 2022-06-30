// Counter1 updates the file percentage completion and also the time elapsed
// Counter2 only displays the file percentage (should be called when the file percentage changes i.e. during processing handler)
// displayLcdTime (in Display.ino) shows the time elapsed

void Counter1() {

  // display newpct
  Counter2();
  // display time
  displayLcdTime();
}

void Counter2()
{
  #if defined(LCDSCREEN16x2) || defined(RGBLCD)
    // TODO which is better, this
      lcd.setCursor(8,0);
      lcd.print(newpct);
      lcd.print("%");  
    // or this (from old Counter1 code)
    /*
        itoa(newpct,PlayBytes,10);
        strcat_P(PlayBytes,PSTR("%"));
        lcd.setCursor(8,0);
        lcd.print(PlayBytes);
    */
  #endif
                         
  #ifdef OLED1306
    itoa(newpct,PlayBytes,10);
    sendStrXY(PlayBytes,8,0);
    sendChar('%');
  #endif

  #ifdef P8544
    // TODO which is better, this
    lcd.setCursor(0,3);
    lcd.print(newpct);
    lcd.print("%");  
    // or this (from old Counter1 code)
    /*
    itoa(newpct,PlayBytes,10);
    strcat_P(PlayBytes,PSTR("%"));
    lcd.setCursor(0,3);
    lcd.print(PlayBytes);
    */
  #endif
}
