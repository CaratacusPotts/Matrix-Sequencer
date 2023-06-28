#include <Arduino.h>                  // required before wiring_private.h
#include "wiring_private.h"           // pinPeripheral() function

Uart Serial2 (&sercom2, 12, 11, SERCOM_RX_PAD_1, UART_TX_PAD_0);  //TX on D4 (SERCOM2.0) and RX on D3 (SERCOM2.1) Using Arduino Zero: 3to12 4to11
void SERCOM2_Handler(){Serial2.IrqHandler();}
// For above, see: https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
// https://docs.arduino.cc/tutorials/communication/SamdSercom
// https://docs.arduino.cc/hardware/mkr-zero
// Bootloader https://github.com/adafruit/uf2-samdx1/releases
// https://learn.adafruit.com/how-to-program-samd-bootloaders/programming-the-bootloader-with-atmel-studio
// https://www.farnell.com/datasheets/2553083.pdf


#include <MIDI.h>
#include <Wire.h> 

#define extMem 0x50

#define latchPin 9                      // LED array Latch (when active, enables led switch)
#define clockPin 7                      // LED array Clock (moves the data over to the switches one bit, one pulse at a time)
#define dataPin 5                       // LED array Data (the data line, that is sent to the switches)
#define cdData 2                        // SD Card Data
#define RDY1pin 12                      // Touch Area 1, data available pin
#define RDY2pin 11                      // Touch Area 2, data available pin
#define RDY3pin 13                      // Touch Area 3, data available pin
#define RDY4pin 10                      // Touch Area 4, data available pin
#define PedalInL A0                      // Touch Area 1, data available pin
#define PedalInR A1                      // Touch Area 2, data available pin
#define PedalInRing A2                   // Touch Area 3, data available pin
#define PedalInAlt A3                    // Touch Area 4, data available pin
#define touchTime 50000                  // How often to check the touch RDY pins in uS. (event pins)
#define ledTime 800                      // Sweep Time of LEDs in mS - not total time, but time between cols. For 10Hz refresh = (1000mS / 10Hz) / 32Cols = 3.125mS 


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

// NOTES
byte noteArray[8][256];   // 0=Addr  1,2=stepPos  3=Pitch  4=Velocity  5,6=EndStep  7=transpose ||   8,9=SavedStepPos  10,11 SavedstepEnd
int enRecLoopValue = 0;


// System Settings
byte Addr;
byte lastAddr = 0;
byte newAddr = 2;                                 // Addr 0 is first.  Addr 1 is last.  Next Addr (new item) goes on 2, then 3 etc
int nextStep = 0;
byte nextPitch = 0;
byte nextVelocity = 0;
int nextStepOff = 0;
byte bpm = 140;
byte stepsInBeat = 24;
byte notesPerBeat = 4; 
byte channelOut = 16;  
byte channelIn = 16;     
int stepsInLoop = 95;      
int stepStart = 0;
int stepEnd = 95;
int notesStepOff[16][108];


// LOOP
bool clkSync = false;
bool startStop = false;
byte clkTick = 0;
byte stepTick = 0;
int stepCount = 0;        // total number of steps so far
bool loopStartFlag = false;
byte loopNum = 0;
bool loopLoadActive = false;
bool mute = false;
bool recCCNextPattern = false;



// SONG MODE
byte seqStep = 0;
byte seqArray [255];


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



// TRANSPOSE
byte chordArray [109];  //  Up to MIDI 108 (key88, C8) https://www.inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
bool transposeActive = false; // live transposing of notes sequence using keyboard in.
byte transposeNoteTonic = 127; // seed with a note
byte transposePitch = 47;
bool transposeFlag = false;
byte transposeChordFlag = 0;



// PHASE
bool phaseActive = false;
int phaseAmount = 2;
int phaseClkCount = 0;

// StepFrequency
int stepFreq = 0;       // how fast the loop is playing relative to intClk.  2x = twice for every ClkTick.  -2x(1/2)= once for every Clktick 
int stepFreqCount = 0;  // c


// CONDUCTOR
bool conductor = false;
bool conductorChange = false;
int chordPitch[] = {0,0,0};
int conductorOut [17][2];
int conductorMidiOutEdit = -1;
bool conductorTransposeFlag = false;
bool conductorTransposeOffFlag = false;
int conductorTransposeOffset = 127;

// Controller 
bool controllerAttached = false;
int modeSelectState = 0; 
byte keyboardActive = false;
byte encoderButtonsActive = 0;
byte touchButtonFlag = 0;
byte encoderButtonEdit = 0;
byte mutePitch[4] = {108,20,127,20};
bool mutePitchActive = false;
unsigned int ccButtonLast = 0;
int enRecLoopFlag = 0;
int enRecLoopStartStep = 0;
int enCanonMode = true;
int tempChanChange = 0;
int keyboardPos = 0;
byte ccByte1 = 0;
bool ccByte1Flag= false;


// LED SCREEN
bool ledScreen = true;     // if LED screen connected..
unsigned long ledTimeLast;                                       // Timer for last pulse
byte winXStepStart = 0;                                  // Units: LED (not step)  Windowed Notes for ledNoteArray.  Which step count is on the left (typcially step 0) 
byte winYNoteStart = 47;                                 // Units: Midi Note  Windowed Notes for ledNoteArray.  Which note pitch is on the ly axis (typcially bottom 59 middle B)
bool stepViewActive = false; // led show
byte velArray[32];                                 // For storing on screen velocity values.
bool velDisplay = false;
byte drawVelStep;  
int drawVel;

byte ledRow = 0;
byte ledDisplayMode = 0;                                // What to display on LED's. Type 0=Notes 1= Command Info (numbers etc)
byte ledNoteArray[13][32];                                // To store note position on LED array
byte ledArray[14][32];                                  // To store Number or Text position for LED array
// 12 + 23 = 35.  24 + 23 = 47 Middle B
unsigned long ledTurnOffKeys = 0;
bool keysOn = true;
uint32_t ledSteps = 0xFFFFFFFF; //0x11111111;  // this is the 'black notes' shown on screen
bool statusLed = false;


//  TOUCH SCREEN
bool touchScreen = true;   // If touchScreen Connected
unsigned long buttonLastArray[] = {0,0,0,1};                     // 0= xPos, 1=yPos, 2= timeNow 3=Action (0 finger down, 1 tap, 2 scroll, 3 draw, 4 zoom)
unsigned long swipeLast = 0;
unsigned long tapLast = 0;
unsigned long swipeStart[] = {0,0,0};
unsigned long eventLast = 0;
bool settings = false;
bool noteVelo = false;
bool loopLoad = true;
bool seqMode = false;
byte tapActive = 0;  
byte swipeActive = 0;
int velPointer = 0;  // if a value is needed by a continuing swipe (ie: note velocity)
byte drawActive = 0;  
unsigned long scrollLast;
unsigned long touchTempArray[4][8];
byte tieNotePSE [3];                          // 0 = Pitch, 1 = stepStart, 2 = stepEnd
bool setStartEnd = false;
bool saveLoop = false;
bool canonActive = false;
bool clearNotes = false;
bool phaseMod = false;
bool channelMod = false;
bool chordEdit = false;
