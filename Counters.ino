void Counter1() {
  #ifdef LCDSCREEN16x2            
                                         
              itoa(newpct,PlayBytes,10);
              strcat_P(PlayBytes,PSTR("%"));
              lcd.setCursor(8,0);
              lcd.print(PlayBytes);
              //sprintf(PlayBytes,"%03d",lcdsegs%1000);lcd.setCursor(13,0);lcd.print(PlayBytes);
              strcpy(PlayBytes,"000");
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
                                
              lcd.setCursor(13,0);
              lcd.print(PlayBytes); 
              
            #endif

            #ifdef RGBLCD            
           
                                            
              itoa(newpct,PlayBytes,10);
              strcat_P(PlayBytes,PSTR("%"));
              lcd.setCursor(8,0);
              lcd.print(PlayBytes);
              /*
               //sprintf(PlayBytes,"%03d",lcdsegs%1000);lcd.setCursor(13,0);lcd.print(PlayBytes);
              strcpy(PlayBytes,"000");
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
                                
              lcd.setCursor(13,0);
              lcd.print(PlayBytes);  */
              
            #endif
            
            #ifdef OLED1306
                                           
              itoa(newpct,PlayBytes,10);strcat_P(PlayBytes,PSTR("%"));sendStrXY(PlayBytes,8,0);
              
              strcpy(PlayBytes,"000");
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
                                
              sendStrXY(PlayBytes,13,0);
            #endif

            #ifdef P8544            
                                         
              itoa(newpct,PlayBytes,10);
              strcat_P(PlayBytes,PSTR("%"));
              lcd.setCursor(0,3);
              lcd.print(PlayBytes);
              //sprintf(PlayBytes,"%03d",lcdsegs%1000);lcd.setCursor(13,0);lcd.print(PlayBytes);strcpy(PlayBytes,"000");
              if ((lcdsegs %1000) <10) itoa(lcdsegs%10,PlayBytes+2,10);
              else 
                 if ((lcdsegs %1000) <100)itoa(lcdsegs%1000,PlayBytes+1,10);
                 else 
                    itoa(lcdsegs%1000,PlayBytes,10);
                                
              
             #endif
  
}


void Counter2()    {

  #ifdef LCDSCREEN16x2  
              lcd.setCursor(8,0);
              lcd.print(newpct);
              lcd.print("%");  
             
            #endif

            #ifdef RGBLCD  
              lcd.setCursor(8,0);
              lcd.print(newpct);
              lcd.print("%");  
             
            #endif
                         
            #ifdef OLED1306
              if (newpct <10) {setXY(8,0);sendChar(48+newpct%10);}
              else
                if (newpct <100){setXY(8,0);sendChar(48+newpct/10); sendChar(48+newpct%10);}
                else {setXY(8,0);sendChar('1');sendChar('0');sendChar('0');}
                
              sendChar('%');              
            #endif

            #ifdef P8544
              lcd.setCursor(0,3);
              lcd.print(newpct);
              lcd.print("%");  
             
            #endif
}
