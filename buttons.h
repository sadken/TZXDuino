void setup_buttons();
bool button_play();
bool button_stop();
bool button_root();
bool button_down();
bool button_up();

#ifdef HAVE_MOTOR
bool button_motor();
#endif

typedef bool (*button_fn)(void);
void button_wait(button_fn);
bool button_wait_timeout(button_fn, int timeout);

#define BUTTON_WAIT_DELAY 5 // small delay in ms, used in the button_wait/timeout loops
