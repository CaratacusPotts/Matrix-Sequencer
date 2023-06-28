#include <Arduino.h>                  // required before wiring_private.h
#include "wiring_private.h"           // pinPeripheral() function
#include <FastLED.h>

Uart Serial2 (&sercom2, 12, 11, SERCOM_RX_PAD_1, UART_TX_PAD_0);  //TX on D4 (SERCOM2.0) and RX on D3 (SERCOM2.1) Using Arduino Zero: 3to12 4to11
void SERCOM2_Handler(){Serial2.IrqHandler();}
// For above, see: https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
// https://docs.arduino.cc/tutorials/communication/SamdSercom
// https://docs.arduino.cc/hardware/mkr-zero
// Bootloader https://github.com/adafruit/uf2-samdx1/releases
// https://learn.adafruit.com/how-to-program-samd-bootloaders/programming-the-bootloader-with-atmel-studio
// https://www.farnell.com/datasheets/2553083.pdf
//C:\Users\Carasquid\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.7.11\variants\itsybitsy_m0

// Adafruit IstyBitsy M0 Express
// PA00 = Pin 40
// PA01 = Pin 41
// PA02 = Pin 14
// PA03 = Pin 42/A12
// PA04 = Pin 17/A3
// PA05 = Pin 18/A4 
// PA06 = Pin 24?
// PA07 = Pin 9/A11
// PA08 = Pin 4/A8
// PA09 = Pin 3/A9
// PA10 = Pin 1/A7
// PA11 = Pin 0/A6
// PA12 = Pin 28
// PA13 = Pin 
// PA14 = Pin 2
// PA15 = Pin 5
// PA16 = Pin 11
// PA17 = Pin 13
// PA18 = Pin 10
// PA19 = Pin 12
// PA20 = Pin 6 (?)
// PA21 = Pin 7
// PA22 = Pin 26
// PA23 = Pin 27
// PA24 = Pin D-
// PA25 = Pin D+ 
// PB02 = Pin 19/A5
// PB03 = Pin 36 (?)
// PB08 = Pin 15/A1
// PB09 = Pin 16/A2 
// PB10 = Pin 29  
// PB11 = Pin 30  

#include <MIDI.h>
#include <Wire.h> 

#define extMem 0x50
#define Expander1_ADR 0x22    // set it according to your IO expander device address
#define Expander2_ADR 0x23    // set it according to your IO expander device address
#define ledOut 2 // for serial leds
#define ledNum 27

#define latchPin 9                      // LED array Latch (when active, enables led switch)
#define clockPin 7                      // LED array Clock (moves the data over to the switches one bit, one pulse at a time)
#define dataPin 5                       // LED array Data (the data line, that is sent to the switches)
#define RDY1pin 12                      // Touch Area 1, data available pin
#define RDY2pin 11                      // Touch Area 2, data available pin
#define RDY3pin 13                      // Touch Area 3, data available pin
#define RDY4pin 10                      // Touch Area 4, data available pin
#define PedalInL A0                      // Touch Area 1, data available pin
#define LEDBlank 18
#define touchTime 50000                  // How often to check the touch RDY pins in uS. (event pins)
#define ledTime 830                      // Sweep Time of LEDs in mS - not total time, but time between cols. For 10Hz refresh = (1000mS / 10Hz) / 32Cols = 3.125mS 
#define ledBlankTime 200

//  VARIABLE FOR DIFFERENT DEVICES  ***********************************************************************
#define nRows 13
#define nCols 16
#define nGrids 2
// **********************************************************************

// DATA
  // Calculate Maximum Number of Events
    // Current memory is 512k bits or 64k bytes
    // At 12 memory slot =  5333 bytes &@ 6 bytes per event = 888 events (@3 events per sec = 5 mins)
    // At 35 memory slots (7*5+ = 1828 bytes &@ 6 bytes per event = 304 events (@3 events per sec = 1.68mins)
    // At 254 events, @ 3 events per sec = 1.4 mins

  // Patterns Sequence from Byte 256 to Byte 511.
  // Loops from 1024 every 3584, with first 106 bytes loop info.
  // BPM, notesPerBeat, StepsInLoop,  ...


unsigned long writeTimeLast = 0;
int seqMemStart = 256;
int globalMemStart = 0;   
    // 1= channelOut, 2= channelIn, 3= , 4= , 5= phaseAmount[0], 6= phaseAmount[1], 10-22 = ChordArray
    // 32-64 Conductor Out



// TIMING
long intClockuS = 0;
unsigned long clockTimeLast;   // internal midi clock
unsigned long panelTimeLast;    // check the touch and input panels
unsigned long encoderTimeLast;

// NOTES
byte noteArray[8][256];   // 0=Addr  1,2=stepPos  3=Pitch  4=Velocity  5,6=EndStep  7=transpose ||   8,9=SavedStepPos  10,11 SavedstepEnd
const byte keyArray[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0};
const byte noteMin = 21;  // A0
const byte noteMax = 106; // B7

// System Settings
byte Addr;
byte lastAddr = 0;
byte newAddr = 2;                                 // Addr 0 is first.  Addr 1 is last.  Next Addr (new item) goes on 2, then 3 etc
int nextStep = 0;
byte nextPitch = 0;
byte nextVelocity = 0;
int nextStepOff = 0;
byte bpm = 140;
const byte stepsInBeat = 24;
byte stepsPerLED = 6;
const int stepsPerLedArray[] = {2,3,4,6,8,12,24,48,96};    // 1/12,1/8,1/6,1/4,1/3,1/2,1,2,1,2,4
byte channelOut = 16;  
byte channelIn = 16;     
int stepsInLoop = 95;      
int stepStart = 0;
int stepEnd = 95;
int notesStepOff[16][128];


// LOOP
bool clkSync = false;
bool clkSyncFree = true;
bool startStop = false;
byte clkTick = 0;
byte stepTick = 0;
int stepCount = 0;        // total number of steps so far
bool loopStartFlag = false;
byte loopNum = 0;
bool loadLoopActive = false;
bool mute = false;
bool recCCNextPattern = false;



// SONG MODE
byte songStep = 0;
byte songArray [128];
int songCurrent = 0;
int songStartStep;
int songEndStep;
const int songSteps = 125;
int songWindowStart = 0;

// RECORDING
byte cc64 = 0;   // CC PEDAL IN
bool recordSwitchOff;                 // Baseline.  So if switch is active high or low - accepts either one.  Checked on startup                         
bool recordSwitchLast;                // Current State.  Checked on startup
bool recordPedalLast = false;
unsigned long recordPedalTimeLast = 0;
bool recording = false;   // If true, enables MIDI in to be saved to EERPOM.
bool recordingStop = false;
bool recordOvertop = false;
bool recordFlag = false;


// Encoders/Expanders/Buttons
const byte ccNum = 32;
byte en1Last[4];
byte en2Last[4];
unsigned long knobActionTimeLast = 0;                              // check the touch and input panels
unsigned long buttActionTimeLast = 0;                              // check the touch and input panels
int buttHoldFlag = -1;

const int tbList[]={0,0,0,40,41,42,15,16,17,29,30,28};
int tbState = 0;

// EDITING
int enRecLoopValue = 0;             // Flag and Value indicating Encoder rec.  If 0=off.  1 or -1 is amount to move
int enRecLoopFlag = 0;              // Which Encoder is being Recorded.  Using bits, so can record multiple encoders at once.
int enRecLoopStartStep = 0;         // which Step is the first of the rec cycle.
int editSettings = 0;
bool editSettingsChord = false;
bool settingsChanged = false; 
uint32_t settingsXAxis = 0; 
uint16_t settingsYAxis = 0;
/*
1 = midi
2 = Conductor
3 = Scale Notes
4 = Phase Amount 
 */
 
 // Controller 
bool controllerAttached = false;
int Mode = 0; 
byte keyboardActive = false;
long encoderButtonsActive = 0;
long encoderButtonEdit = 0;
int mutePitch[4] = {20,108,20,108};
bool mutePitchActiveHigh = false;
bool mutePitchActiveLow = false;
int enCanonMode = true;
int tempChanChange = 0;
int keyboardPos = 0;
byte ccByte1 = 0;
bool ccByte1Flag= false;

//  GENERAL CANON stuff
//bool notesChanged = false;   // so if modifying notes, and user has edited notes meanwhile, then you know to update.


// TRANSPOSE
byte chordArray [109];  //  Up to MIDI 108 (key88, C8) https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
bool transposeActive = false; // live transposing of notes sequence using keyboard in.
int transposeNoteTonic = 127; // seed with a note
int transposePitch = 47;
bool transposeFlag = false;
int transposeChordFlag = 0;



// PHASE
bool phaseActive = false;
int phaseAmount = 2;
int phaseClkCount = 0;

// StepFrequency
int stepFreq = 0;       // how fast the loop is playing relative to intClk.  2x = twice for every ClkTick.  -2x(1/2)= once for every Clktick 
int stepFreqCount = 0;  // c

// canonPitchLimit
int canonInvertAxis = 0;


// CONDUCTOR
bool conductor = false;
bool conductorChange = false;
int conductorFinger[4];
int conductorNoteIn[127];
int conductorOut [17][2];  // [midi channel][finger 1,2,3 (if 12, midi through)]
int conductorMidiOutEdit = -1;
bool conductorTransposeFlag = false;
bool conductorTransposeOffFlag = false;
int conductorTransposeOffset = 127;

// LED SCREEN
CRGB leds[ledNum];
bool ledScreen = true;     // if LED screen connected..
bool resetScreenFlag = false;
unsigned long resetScreenActionLast = 0;
unsigned long ledTimeLast;                                       // Timer for last pulse
int winXStepStart = 0;                                  // Units: LED (not step)  Windowed Notes for ledArray  .  Which step count is on the left (typcially step 0) 
byte winYNoteStart = 47;                                 // Units: Midi Note  Windowed Notes for ledArray.  Which note pitch is on the ly axis (typcially bottom 59 middle B)
bool stepViewActive = false; // led show
byte velArray[32];                                 // For storing on screen velocity values.
bool velDisplay = false;
int drawVelStep;  
int drawVel;

byte ledRow = 0;
byte ledArray[nRows][nCols];                                  // To store Number or Text position for LED array
// 12 + 23 = 35.  24 + 23 = 47 Middle B
unsigned long ledTurnOffKeys = 0;
bool keysOn = true;
uint16_t ledSteps = 0xFFFF; //0x11111111;  // this is the 'black notes' shown on screen
bool statusLed = false;


//  TOUCH SCREEN
bool touchScreen = true;   // If touchScreen Connected
bool settings = false;
bool loopMode = true;
bool songMode = false;
byte tieNotePSE [3];                          // 0 = Pitch, 1 = stepStart, 2 = stepEnd
int xPosLast = 0;
int yPosLast = 0;
unsigned long touchActionLast = 0;
unsigned long touchSwipeLast = 0;
int touchEventType = 0;
//




//const byte num[][10]=                                         // Lookup Number Array.  Row = Number you want to display
//{                                                             // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
//{62,65,65,65,65,65,65,62},                                    // 0    
//{1,1,1,1,1,1,1,1},                                            // 1
//{63,64,64,64,62,1,1,126},                                     // 2
//{126,1,1,1,62,1,1,126},                                       // 3 etc
//{8,8,8,62,72,64,64,64},
//{126,1,1,1,126,64,64,127},
//{62,65,65,65,126,64,64,62},
//{1,1,1,1,1,1,1,126},
//{62,65,65,65,62,65,65,62},
//{1,1,1,1,62,65,65,62}
//};

const byte numSmall[][6]=     // Lookup Number Array.  Row = Number you want to display
  {               // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
    {7,5,5,5,7},  // 0    
    {1,1,1,1,1},  // 1
    {7,4,7,1,7},  // 2
    {7,1,3,1,7},  // 3 etc
    {2,2,7,6,4},
    {7,1,7,4,7},
    {7,5,7,4,4},
    {1,1,1,1,7},
    {7,5,7,5,7},
    {1,1,7,5,7}
  };  

const byte iconSmall[][6]=     // Lookup Number Array.  Row = Number you want to display
  {               // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
    {9,6,6,9,0},  // X 
    {12,10,9,10,12},  // Out Arrow
    {9,5,3,5,9}  // In Arrow
  }; 
 
unsigned int alphabet[26] = 
{
  7775,   //A  0
  14671,  //B  1
  9519,  //C  2
  15694,  //D  3
  18111,  //E  4
  1215,   //F  5
  30399,  //G  6
  31903,  //H  7
  15,    //I  8  
  2041,   //J  9
  17567,  //K  10
  16927,  //L  11
  15439,  //M  12
  31903,  //N  13 
  32319,  //O  14
  7359,   //P  15
  9191,   //Q  16
  1087,   //R  17 
  26291,  //S  18
  2017,   //T  19
  32287,  //U  20
  7687,   //V  21
  32031,  //W  22
  17553,  //X  23
  3971,   //Y  24
  40307, //Z   25
};    
