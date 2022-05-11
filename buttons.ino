#include "buttons.h"

#ifdef BUTTONS_ADC

// all buttons (except motor) routed through a single pin using a resistor potential divider network
// Analog levels (= outputs of the divider network) are set in userconfig.h
// See docs for more details
void setup_buttons(void)
{
  pinMode(BUTTONS_ADC_PIN, INPUT_PULLUP);

  #ifdef HAVE_MOTOR
  pinMode(btnMotor, INPUT_PULLUP);
  digitalWrite(btnMotor,HIGH);
  #endif
}

bool button_play()
{
  // todo - use isr to capture buttons?
  int sensorValue = analogRead(BUTTONS_ADC_PIN);
  return(sensorValue>=BUTTONS_ADC_PLAY_LOW);
}

bool button_stop()
{
  // todo - use isr to capture buttons?
  int sensorValue = analogRead(BUTTONS_ADC_PIN);
  return(sensorValue>=BUTTONS_ADC_STOP_LOW && sensorValue<BUTTONS_ADC_PLAY_LOW);
}

bool button_root()
{
  // todo - use isr to capture buttons?
  int sensorValue = analogRead(BUTTONS_ADC_PIN);
  return(sensorValue>=BUTTONS_ADC_ROOT_LOW && sensorValue<BUTTONS_ADC_STOP_LOW);
}

bool button_down()
{
  // todo - use isr to capture buttons?
  int sensorValue = analogRead(BUTTONS_ADC_PIN);
  return(sensorValue>=BUTTONS_ADC_DOWN_LOW && sensorValue<BUTTONS_ADC_ROOT_LOW);
}

bool button_up()
{
  // todo - use isr to capture buttons?
  int sensorValue = analogRead(BUTTONS_ADC_PIN);
  return(sensorValue>=BUTTONS_ADC_UP_LOW && sensorValue<BUTTONS_ADC_DOWN_LOW);
}

#else // BUTTONS_ADC

void setup_buttons(void)
{
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
  pinMode(btnRoot, INPUT_PULLUP);
  digitalWrite(btnRoot, HIGH); 

  #ifdef HAVE_MOTOR
  pinMode(btnMotor, INPUT_PULLUP);
  digitalWrite(btnMotor,HIGH);
  #endif
}

bool button_play()
{
  return (digitalRead(btnPlay)==LOW);
}

bool button_stop()
{
  return (digitalRead(btnStop)==LOW); 
}
 
bool button_root()
{
  return (digitalRead(btnRoot)==LOW);
}

bool button_down()
{
  return (digitalRead(btnDown)==LOW);
}

bool button_up()
{
  return (digitalRead(btnUp)==LOW);
}

#endif // BUTTONS_ADC


#ifdef HAVE_MOTOR
bool button_motor()
{
  return (digitalRead(btnMotor));
}
#endif

void button_wait(button_fn f) {
  // returns when the button has been released
  while(f()) {
    //prevent button repeats by waiting until the button is released.
    delay(BUTTON_WAIT_DELAY);
  }
}

bool button_wait_timeout(button_fn f, int timeout) {
  // returns when the button has been released OR timeout reached
  // returns true if the button was still pressed (i.e. when timeout reached)
  while(f() && timeout > 0) {
    //prevent button repeats by waiting until the button is released.
    if (timeout >= BUTTON_WAIT_DELAY) {
      delay(BUTTON_WAIT_DELAY);
      timeout -= BUTTON_WAIT_DELAY;
    }
    else
    {
      delay(timeout);
      timeout = 0;
    }
  }
  
  return (timeout==0);
}
