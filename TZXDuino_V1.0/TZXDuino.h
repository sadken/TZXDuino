#define outputPin           9

const char TZXTape[7] = {'Z','X','T','a','p','e','!'};
const char TAPcheck[7] = {'T','A','P','t','a','p','.'};

//TZX block list - uncomment as supported
#define ID10                0x10    //Standard speed data block
#define ID11                0x11    //Turbo speed data block
#define ID12                0x12    //Pure tone
#define ID13                0x13    //Sequence of pulses of various lengths
#define ID14                0x14    //Pure data block
#define ID15                0x15    //Direct recording block
#define ID18                0x18    //CSW recording block
#define ID19                0x19    //Generalized data block
#define ID20                0x20    //Pause (silence) ot 'Stop the tape' command
#define ID21                0x21    //Group start
#define ID22                0x22    //Group end
#define ID23                0x23    //Jump to block
#define ID24                0x24    //Loop start
#define ID25                0x25    //Loop end
#define ID26                0x26    //Call sequence
#define ID27                0x27    //Return from sequence
#define ID28                0x28    //Select block
#define ID2A                0x2A    //Stop the tape is in 48K mode
#define ID2B                0x2B    //Set signal level
#define ID30                0x30    //Text description
#define ID31                0x31    //Message block
#define ID32                0x32    //Archive info
#define ID33                0x33    //Hardware type
#define ID35                0x35    //Custom info block
#define ID5A                0x5A    //Glue block (90 dec, ASCII Letter 'Z')
#define TAP                 0xFE    //Tap File Mode
#define EOF                 0xFF    //End of file


//TZX File Tasks
#define GETFILEHEADER         0
#define GETID                 1
#define PROCESSID             2

//TZX ID Tasks
#define READPARAM             0
#define PILOT                 1
#define SYNC1                 2
#define SYNC2                 3
#define DATA                  4
#define PAUSE                 5

//Buffer size
#define buffsize              64

//Spectrum Standards
#define PILOTLENGTH           619
#define SYNCFIRST             191
#define SYNCSECOND            210
#define ZEROPULSE             244
#define ONEPULSE              489
#define PILOTNUMBERL          8063
#define PILOTNUMBERH          3223
#define PAUSELENGTH           1000           

//Keep track of which ID, Task, and Block Task we're dealing with
byte currentID = 0;
byte currentTask = 0;
byte currentBlockTask = 0;

//Temporarily store for a pulse period before loading it into the buffer.
word currentPeriod=1;

//ISR Variables
volatile byte pos = 0;
volatile word wbuffer[buffsize+1][2];
volatile byte morebuff = HIGH;
volatile byte workingBuffer=0;
volatile byte isStopped=false;
volatile byte pinState=LOW;
volatile byte isPauseBlock = false;
volatile byte wasPauseBlock = false;
volatile byte intError = false;

//Main Variables
byte btemppos = 0;
byte copybuff = LOW;
unsigned long bytesRead=0;
unsigned long bytesToRead=0;
byte pulsesCountByte=0;
word pilotPulses=0;
word pilotLength=0;
word sync1Length=0;
word sync2Length=0;
word zeroPulse=0;
word onePulse=0;
byte usedBitsInLastByte=8;
word loopCount=0;
byte seqPulses=0;
unsigned long loopStart=0;
word pauseLength=0;
byte outByte=0;
word outWord=0;
unsigned long outLong=0;
byte count=128;
volatile byte currentBit=0;
volatile byte currentByte=0;
byte pass=0;
unsigned long debugCount=0;
byte EndOfFile=false;
