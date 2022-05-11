#pragma once

class TimerClass
{
    public:
      void stop(void);
      void setPeriod(unsigned long period);
      void initialize();
};

extern TimerClass Timer;
