// Controller for Canon Matrix
#include "variables.h"

MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

void setup() 
{
  Serial.begin(115200);      
  Wire.begin();  
  Wire.setClock(1000000);      // try 100kHz bus first
  
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff(); 

  MIDI.setHandleControlChange(ccIn);

//  lcd.init();
//  lcd.setCursor(0,0);
//  lcd.print("Boobs");
  
  portExpanderConfig();
  getCurrentEncoderState();
  pinMode(36, INPUT);   
  pinMode(36, INPUT_PULLUP);   

  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  int tbMap[] = {18,9,4,3,29,30,28,2,5,12,7,19}; for(int i=0; i<12; i++){pinMode(tbMap[i], INPUT); pinMode(tbMap[i], INPUT_PULLUP); }
  int enTbMap[] = {40,41,14,42,16,17}; for(int i=0; i<6; i++){pinMode(enTbMap[i], INPUT); pinMode(enTbMap[i], INPUT_PULLUP); }

  for(int i=0; i<22; i++){buttonState[i] = 0;}  // set button state to zero.
  for(int i=0; i<48; i++){ledState[i] = 0;}     // set LED array to zero
  for(int i=0; i<127; i++){ccOutputLast[i] = 0;}  // set debounce out to zero
    
  keyTimeLast = micros();
}

void loop() 
{
  MIDI.read(); 

  if(!digitalRead(36)){readEncoders();}
  
  unsigned long timeNow = micros(); 
  if(timeNow - keyTimeLast > 5000) {keyTimeLast = timeNow; calcButtons(); outputLed();}   // Read at 5mS interval
  
  if(Serial.available() > 0)  { readSerial(); }
}
