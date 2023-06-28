#include "variables.h"
//

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

//  INLINE CONTROLLER STUFF  *******************************************************
//  Uart Serial3 (&sercom1, 9, 8, SERCOM_RX_PAD_1, UART_TX_PAD_0);  // RX on D13 (SERCOM1.1)   TX on D11 (SERCOM1.0)  Using Arduino Zero: D11 = D8, D13=D9
//  void SERCOM1_Handler(){Serial3.IrqHandler();}
//  MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
// **********************************************************************************


void setup() 
{  
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(RDY1pin, INPUT);
  pinMode(RDY2pin, INPUT);  
  pinMode(RDY3pin, INPUT);
  pinMode(RDY4pin, INPUT);   
  pinMode(PedalInL, INPUT);  
  pinMode(PedalInL, INPUT_PULLUP);       
  pinMode(PedalInRing, OUTPUT);  
  digitalWrite(PedalInRing, HIGH);    // sends high so foot switch can detect it.. 
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW); 
  
  Serial.begin(115200);  
//  Serial3.begin(115200);  

  Wire.begin();  
  Wire.setClock(800000);   
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  MIDI.setHandleNoteOn(noteIn);                                // Tells the Midi Library to call NoteIn when a NOTE ON command is received.
  MIDI.setHandleNoteOff(noteOffIn);                            // Tells the Midi Library to call NoteOffIn when a NOTE OFF command is received.
  MIDI.setHandleControlChange(ccIn);
  
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI2.turnThruOff();    
  MIDI2.setHandleNoteOn(noteIn);                                // Tells the Midi Library to call NoteIn when a NOTE ON command is received.
  MIDI2.setHandleNoteOff(noteOffIn);                            // Tells the Midi Library to call NoteOffIn when a NOTE OFF command is received.
  MIDI2.setHandleControlChange(ccIn);
  
  // Assign pins 3 & 4 SERCOM functionality
  pinPeripheral(3, PIO_SERCOM_ALT);
  pinPeripheral(4, PIO_SERCOM_ALT);

  recordSwitchOff = digitalRead(PedalInL);     //  Setting the offState of the recordswitch on startup.
  recordSwitchLast = recordSwitchOff;           // Setting the last last as this one.

  uint8_t b[2];
  readEEPROM(0,b,1);
  if(b[0] != 68){loadDefaultValues();}
  loadSavedValues();
  
  for(int i=0; i<16; i++) 
  { 
    for(int p=20; p<108; p++) {notesStepOff[i][p] = -1;} 
    conductorOut[i][0]=0; conductorOut[i][1]=0; conductorOut[i][2]=0; conductorOut[i][3]=0; conductorOut[i][4]=0; conductorOut[i][5]=0; 
  }
  
  initialiseNoteArray();
  for(int y=0;y<13;y++){for(int x=0;x<32;x++){ledNoteArray[y][x] = 0;}}   // Clear ledNoteArray to ensure zeros in there
  //prePattern();  
  nextEvent(Addr); nextEvent(Addr);
  
  checkMidiClock();

//  INLINE CONTROLLER STUFF  ******************************************************************************
/*
  // Check to see if touch screen available - if not disable touch screen and led.
  Wire.beginTransmission(112);
  byte screenAvailable = Wire.endTransmission();
  if(screenAvailable != 0)
  {
    touchScreen = false; ledScreen = false;startStop = true; loopStart();
    pinMode(RDY4pin, OUTPUT);
    digitalWrite(cdData, HIGH);  digitalWrite(dataPin, LOW); digitalWrite(RDY4pin, HIGH); 
  }

  pinPeripheral(11, PIO_SERCOM);
  pinPeripheral(13, PIO_SERCOM);
  
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI3.turnThruOff();
  MIDI3.setHandleControlChange(ccIn);   

  checkForController(); // Check if Controller is present

*/
//  *******************************************************************************************************
   
  
  ledRefresh(); 
  ledRefreshNotes();
  
  ledTimeLast = micros();    
  ledTurnOffKeys = micros();
  panelTimeLast = millis();    
        
  intClockuS = (1000000/(bpm/60))/24;   
}

void loop() 
{  
  if(MIDI2.read())
  {
    switch(MIDI2.getType())
    {
      case midi::Clock :{if(clkSync){intClock();}} break;
      case midi::Start :{MIDI2.sendRealTime(MIDI_NAMESPACE::Start); startStop = true; if(!recording){loopStart();}} break;            
      case midi::Stop :{MIDI2.sendRealTime(MIDI_NAMESPACE::Stop); startStop = false; stepCount = 0; Addr = 0; loopStop();} break;
      default: break;
    }
  } 
  MIDI.read();  
//  MIDI3.read();
  
  unsigned long timeNow = micros();                                                 // Do clock timer
  if(!clkSync)
    {if(timeNow - clockTimeLast > intClockuS){clockTimeLast = timeNow; intClock();}}
  
  if(nextStep == stepCount) 
    {if(!mute && startStop){ playNote(nextStep);}} 

  if(ledScreen)
  {
    if(timeNow - ledTimeLast > ledTime){ledTimeLast = timeNow; updateLEDs(ledDisplayMode);}                 // ****************  Update LED (frame) ********************** 
    if(timeNow > ledTurnOffKeys){ledTurnOffKeys = ledTurnOffKeys + 90000; digitalWrite(A4, HIGH);}  
  }
  
  if(timeNow - panelTimeLast > touchTime)   // Read at 50mS interval.  For Touch Panel & Record Switch
  {
    panelTimeLast = timeNow; 
    if(touchScreen){checkTouch();}              // Check Touch Screen States
    recordSwitch();                             //  Check Record Switch State.
  }               
  

  if(loopStartFlag)  // if someone hits play, while the loop in in the middle of a beat.  Wait for beat to play start.
  {
    int b = fmod(stepCount,24); // 24 because there is 24 steps per beat
    if(b == 0)
    {
      loopStartFlag = false;
      loopStart();
    }
  } 

  if(recordFlag)  // somebody has used the record switch to record
  {
    if(micros()- recordPedalTimeLast > 750000)
    {
      if(transposeActive){transposeState();} // tun off - just in case user forgot
      recordFlag = false; recordStart(); Serial.println(" Rec.. ");
    }
  }
  if(recCCNextPattern)  //  somebody has used the record switch to alternate pattern
  {
    if(micros() - recordPedalTimeLast > 600000)
      {
        recCCNextPattern = false;
        if(loopNum == 0){loopNum = 1;}else{loopNum = 0;} 
        loopLoadActive=true;
        Serial.print(" LoadLoop: ");Serial.println(loopNum);
      }
  }
  if(enRecLoopFlag != 0 && enRecLoopStartStep == stepCount) // has done one loop since knob moved..
  {
    enRecLoopFlag = 0; enRecLoopValue = 0; Serial.println("FlagOff");
  }

  if(touchButtonFlag > 0  && modeSelectState == 0)  // IF user is long pressing button.  Indicate time has passed with LED
  {
    if(millis() - ccButtonLast > 1500)
    {
      ccOutSerial3(touchButtonFlag,5);     // Change LED to indicate time reached
      touchButtonFlag = 0;
    }
  }
        
  if(loopLoadActive)
  {
    loopLoadActive = false;
    if(seqMode)
    {    
      int loopLast = loopNum;
      seqStep ++; 
      if(seqStep >= seqArray[254] || seqStep < 0) {seqStep = 0;}  
      loopNum = seqArray[seqStep];
    }
    notesOffAll();
    if(loopNum < 0 || loopNum > 13){loopNum = 0;}
    //if(seqMode && loopNum != loopLast){loadTheLoop(loopNum);}   // no point in reloading the same loop.  
    loadTheLoop(loopNum);
    Addr = 0;  
    nextEvent(Addr);
    if(swipeActive == 0 && drawActive == 0){ledRefreshNotes();} 
  }
  
  
  if(Serial.available() > 0) {readSerial();}
}

void intClock()
{
  MIDI.sendRealTime(MIDI_NAMESPACE::Clock);
  if(startStop)
  {
    stepCount++; 
    playOffNote(stepCount);       // play end notes
    
    if(recordingStop){recordEnd(stepCount -1);}  // Finish off the recording after the foot pedal has deactivated.  
  
    if(stepCount == stepEnd && seqMode)
      {loopLoadActive = true;}    
    if((stepCount > stepEnd && !recording))
      {loopEnd();}
            
    int stepsInWindow = float(32/notesPerBeat) * 24;                     // ie: 24 steps a beat.  If 4 notesPerBeat, then 32/4notesPerBeat = 8Beats.  8*24 = 192
    if(stepsInLoop >= stepsInWindow)   // Scroll screen
    { 
      int stepsPerNote = 24/notesPerBeat;
      int wS = stepCount/stepsPerNote;
      if(wS < 32){winXStepStart = 0;}
      else{winXStepStart = wS;}
      if(swipeActive == 0 && drawActive == 0){ledRefreshNotes();}
    }
  
    int noteStep = stepCount % (24 / notesPerBeat);
    if(conductorChange && noteStep){ conductorChange = false; conductorPlay(); }
    if(transposeFlag && noteStep){transposeFlag = false; offsetNotes(transposePitch);}    
  }
}
  
void loopEnd()  
{  
  stepCount = stepStart;                                // go back to begining (would be step 0, but as we can change the start step..                
  playOffNote(stepCount);       // play end notes
    
  int stepsInWindow = (32/notesPerBeat) * 24;                     // ie: 24 steps a beat.  If 4 notesPerBeat, then 32/4notesPerBeat = 8Beats.  8*24 = 192
  if(startStop){if(stepStart < stepsInWindow){winXStepStart = 0;}else{winXStepStart = stepStart;}} 

  if(phaseActive){stepCount = stepCount + phaseAmount;}
  
  Addr = 0; 
  nextEvent(Addr);
  if(nextStep < stepCount) // if the start point is after step 0 - advance the Addr marker till you find its on or after the stepStart step.
    {for(int i=0;i<255;i++) { nextEvent(Addr); if(nextStep >= stepCount){break;} }}

  if(!touchScreen && !ledScreen)
  {
  //  statusLed = !statusLed;  digitalWrite(RDY4pin, statusLed);Serial.println(statusLed);
  } 
}

void loopStart()
{ 
  //Serial.println("loop start");
  stepCount = stepStart; seqStep=0;
  playOffNote(stepCount);       // play end notes 
  
  mute = false;  
  winXStepStart = 0; 
  
  if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;}
  ledRefresh();

  Addr = 0;  
  nextEvent(Addr); 
  if(nextStep == stepCount && !mute) {playNote(nextStep);} 

  ledRefreshNotes();
}


void loopStop()
{ 
  notesOffAll();    
}


void  checkMidiClock()
{
  unsigned long timeNow = millis();
  do
  {
    if(MIDI2.read())
    {
      switch(MIDI2.getType())
      {
        case midi::Clock :{clkSync = true; ledRefresh();} break;
        default: break;
      }
    } 
  }while (millis() - timeNow < 6000);
}

void checkForController()
{
  ccOutSerial3(102,channelIn);
  delay (10);
 // MIDI3.read();
}

void readSerial()
{
    byte b = Serial.available();
    char a[b];
    Serial.readBytes(a,b);
    byte num;
    if(b>4){num = ((a[1]-48)*10) + (a[2]-48);}
    switch (a[0])
    {
      case 'n':{noteIn (channelIn, num, 100);} break;  // note in  
      case 'i':{channelIn = num; byte ch[] = {num}; writeEEPROM(globalMemStart+2,ch,1);Serial.print("Chan In:");Serial.println(channelIn);} break;   
      case 'o':{channelOut = num; byte ch[] = {num}; writeEEPROM(globalMemStart+1,ch,1);Serial.print("Chan Out:");Serial.println(channelOut);} break;   
      case 'p':{Serial.print("P, Play   Clksync:");Serial.println(clkSync);startStop = true; loopStart();} break; 
      case 't':{touchScreen = !touchScreen;Serial.print("T ");Serial.println(touchScreen); pinMode(RDY4pin, OUTPUT);}   break; // T Touch off/on
      case 'l':{ledScreen = !ledScreen;Serial.print("L "); Serial.println(ledScreen);} break;      // L LED off/on
      case 'r':{recOvertop();} break; 
      case 'R':{if(recording){ccIn(channelIn, 64, 0);} else{ccIn(channelIn, 64, 127);}} break; 
      case 'c':{transposeEditChord(num); transposeSaveChord();} break;
      case '<':{canonPhaseNotes(-1);} break;
      case '>':{canonPhaseNotes(1);} break;
    }
}
