#include "Timers.h"

#if defined(__AVR__)
  #include <TimerOne.h>

  void TimerClass::stop(void)
  {
    Timer1.stop();                              //Stop timer interrupt
  }

  void TimerClass::setPeriod(unsigned long period)
  {
    Timer1.setPeriod(period);
  }

  void TimerClass::initialize()
  {
    Timer1.initialize(100000);                //100ms pause prevents anything bad happening before we're ready
    Timer1.attachInterrupt(wave);
    Timer1.stop();                            //Stop the timer until we're ready
  }

#elif defined(__SAMD21__)

  #if defined(SAMD_USE_SAMDTimerInterrupt)
    #include "SAMDTimerInterrupt.h"
    SAMDTimer ITimer(TIMER_TC3);
  #else
  
  //
  // The following, including the MyTC3Timer class, is a modified version of portions of SAMDTimer (from SAMDTimerInterrupt library)
  // incorporating significant fixes to period accuracy and reducing instruction count of setInterval function
  // which are not yet available in the Arduino-hosted version of the library (at time of writing)
  // see https://github.com/khoih-prog/SAMD_TimerInterrupt/issues/18
  // and https://github.com/khoih-prog/SAMD_TimerInterrupt/issues/17
  //
  
  typedef void (*timerCallback)  ();
  timerCallback TC3_callback;
  
  void TC3_Handler()
  {
    // get timer struct
    TcCount16* TC = (TcCount16*) TC3;
    
    // If the compare register matching the timer count, trigger this interrupt
    if (TC->INTFLAG.bit.MC0 == 1) 
    {
      TC->INTFLAG.bit.MC0 = 1; // write 1 here, to clear the interrupt tr
      (*TC3_callback)();
    }
  }
  
  #define SAMD_TC3        ((TcCount16*) TC3)
  
  #include "Arduino.h"
  #define TIMER_HZ      48000000L

  class MyTC3Timer
  {
    private:
      int             _prescaler;
      int             _compareValue;
      bool initialized;
  
    public:
  
      MyTC3Timer() : initialized(false) {};
      ~MyTC3Timer() {};
     
      void attachInterruptInterval(const unsigned long & interval, timerCallback callback)
      {
        setInterval(interval, callback);
      }

      // commented out as unused by TZXDuino  
//      void detachInterrupt()
//      {
//        NVIC_DisableIRQ(TC3_IRQn); 
//      }
//  
      void disableTimer()
      {
        SAMD_TC3->CTRLA.reg &= ~TC_CTRLA_ENABLE;
        while (SAMD_TC3->STATUS.bit.SYNCBUSY);
      }
  
      // commented out as unused by TZXDuino  
//      void reattachInterrupt()
//      {
//        NVIC_EnableIRQ(TC3_IRQn); 
//      }
//  
      void enableTimer()
      {     
        SAMD_TC3->CTRLA.reg |= TC_CTRLA_ENABLE;
        while (SAMD_TC3->STATUS.bit.SYNCBUSY);
      }
  
      // Just stop clock source, clear the count
      void stopTimer()
      {
        // TODO, clear the count
        disableTimer();
      }
  
      // Just reconnect clock source, start current count from 0
      void restartTimer()
      {
        // TODO, clear the count
        enableTimer();
      }
      
      void setInterval(const unsigned long interval, timerCallback callback)
      {
          noInterrupts();
          if (!initialized)
          {
            REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID (GCM_TCC2_TC3));
            
            while ( GCLK->STATUS.bit.SYNCBUSY);
                    
            SAMD_TC3->CTRLA.reg &= ~TC_CTRLA_ENABLE;
      
            // Use the 16-bit timer
            SAMD_TC3->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
            
            while (SAMD_TC3->STATUS.bit.SYNCBUSY);
      
            // Use match mode so that the timer counter resets when the count matches the compare register
            SAMD_TC3->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
            
            while (SAMD_TC3->STATUS.bit.SYNCBUSY);
        
            // Enable the compare interrupt
            SAMD_TC3->INTENSET.reg = 0;
            SAMD_TC3->INTENSET.bit.MC0 = 1;
      
            TC3_callback  = callback;

            NVIC_EnableIRQ(TC3_IRQn);
      
            setPeriod_TIMER_TC3(interval); // this also enables the timer
      
            initialized = true;
          }
          else
          {
            setPeriod_TIMER_TC3(interval);
            TC3_callback  = callback;
          }
          interrupts();
      }
  
      void setPeriod_TIMER_TC3(const unsigned long period)
      {
        TcCount16* _Timer = (TcCount16*) TC3;
        uint16_t ctrla = _Timer->CTRLA.reg;

        ctrla &= ~(TC_CTRLA_PRESCALER_DIV1024 | TC_CTRLA_PRESCALER_DIV256 | TC_CTRLA_PRESCALER_DIV64 | TC_CTRLA_PRESCALER_DIV16 | TC_CTRLA_PRESCALER_DIV4 | TC_CTRLA_PRESCALER_DIV2 | TC_CTRLA_PRESCALER_DIV1);
        
        if (period > 300000) 
        {
          // Set prescaler to 1024
          ctrla |= TC_CTRLA_PRESCALER_DIV1024;
          _prescaler = 1024;
        } 
        else if (80000 < period && period <= 300000) 
        {
          // Set prescaler to 256
          ctrla |= TC_CTRLA_PRESCALER_DIV256;
          _prescaler = 256;
        } 
        else if (20000 < period && period <= 80000) 
        {
          // Set prescaler to 64
          ctrla |= TC_CTRLA_PRESCALER_DIV64;
          _prescaler = 64;
        } 
        else if (10000 < period && period <= 20000) 
        {
          // Set prescaler to 16
          ctrla |= TC_CTRLA_PRESCALER_DIV16;
          _prescaler = 16;
        } 
        else if (5000 < period && period <= 10000) 
        {
          // Set prescaler to 8
          ctrla |= TC_CTRLA_PRESCALER_DIV8;
          _prescaler = 8;
        } 
        else if (2500 < period && period <= 5000) 
        {
          // Set prescaler to 4
          ctrla |= TC_CTRLA_PRESCALER_DIV4;
          _prescaler = 4;
        } 
        else if (1000 < period && period <= 2500) 
        {
          // Set prescaler to 2
          ctrla |= TC_CTRLA_PRESCALER_DIV2;
          _prescaler = 2;
        } 
        else if (period <= 1000) 
        {
          // Set prescaler to 1
          ctrla |= TC_CTRLA_PRESCALER_DIV1;
          _prescaler = 1;
        }

        _compareValue = (int)(TIMER_HZ / (_prescaler / (period / 1000000.0))) - 1;

        // Make sure the count is in a proportional position to where it was
        // to prevent any jitter or disconnect when changing the compare value.
        _Timer->READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10); // 0x10 is the offset of the 16-bit count register
        while (_Timer->STATUS.bit.SYNCBUSY);
        uint16_t prev_counter = _Timer->COUNT.reg;

        // cannot update ctrla at the same time as the enabled bit is set
        _Timer->CTRLA.reg &= ~TC_CTRLA_ENABLE;
        while (_Timer->STATUS.bit.SYNCBUSY);

        // need to synchronise the count and cc values
        _Timer->READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x18); // 0x18 is the offset of the 16-bit CC0 register
        while (_Timer->STATUS.bit.SYNCBUSY);

        uint16_t old_compare_value = _Timer->CC[0].reg;
        _Timer->CC[0].reg = (uint16_t)(-1); // max, so that changing Count doesn't end up wrapping it back to zero again due to the continuous comparison

        _Timer->COUNT.reg = map(prev_counter, 0, old_compare_value, 0, _compareValue);
        while (_Timer->STATUS.bit.SYNCBUSY);
        
        _Timer->CC[0].reg = _compareValue;
        while (_Timer->STATUS.bit.SYNCBUSY);

        _Timer->CTRLA.reg = ctrla | TC_CTRLA_ENABLE;
        while (_Timer->STATUS.bit.SYNCBUSY);
      }
  };
       
  MyTC3Timer ITimer;
  #endif // mytc3timer

    
  void TimerClass::stop(void)
  {
    ITimer.stopTimer();
  }
  
  void TimerClass::setPeriod(unsigned long period)
  {
    ITimer.setInterval(period, wave);
  }

  void TimerClass::initialize()
  {
    setPeriod(100000);
    stop();
  }

#endif

TimerClass Timer;
