void recordStart()
{   
    Serial.println("RecStart");
    loopStop();
    recording = true;                                                     // Changes recording flag state due to button press
    
    notesPerBeat = 24;
    stepsInLoop = float(32/notesPerBeat) * 24;

    initialiseNoteArray();
    
MIDI.sendControlChange(120,127,channelOut);   //https://www.presetpatch.com/midi-cc-list.aspx
for(int y=20;y<108;y++)
{
  midiSendNoteOff(y, 0, channelOut); 
  notesStepOff[channelOut][y] = -1;   // pitch, velo, channel 
}   
    nextStep = 32765;

    for(int y=0;y<13;y++){for(int x=0;x<32;x++){ledNoteArray[y][x] = 0;}}
}


void recordEnd(int stepEnded)
{                                             
    recordingStop = false;
    recording = false;
    
    int beatsInLoop = stepEnded / 24;                         // the +1 here is so loop finishes on the beat, not the step.
    int remainder = stepEnded % 24;
    if(remainder > 5){beatsInLoop ++;}
 
    stepsInLoop = (beatsInLoop * 24)-1;
    stepEnd = stepsInLoop;     

    // check for off notes not played
    for(int y=20;y<108;y++)
    {
      if(notesStepOff[channelOut-1][y] > -1)
      {
        insertNoteEnd(-1, y, stepEnd - 1); // Finish unfineded notes .  -1= don't know startStep, Pitch:127= lets function know to add any unfinised at end
        notesStepOff[channelOut-1][y] = -1;
      }
    }       
   
    winXStepStart = 0;  
    Addr = 0;  
    nextEvent(Addr); 
    if(nextStep < stepStart) // if the start point is after step 0 - advance the Addr marker till you find its on or after the stepStart step.
     {for(int i=0;i<255;i++) { nextEvent(Addr); if(nextStep >= stepCount){break;} }}

    notesPerBeat = 4;

    saveTheLoop(loopNum);
    ledDisplayMode = 0; ledRefresh(); ledRefreshNotes();      

    transposeNoteTonic = 127;  // reset, as a new note will not get get noticed on the transpose without this reset.
}


void recOvertop()
{
  if(recordOvertop)  // clear log
  { 
    recordOvertop = false;
    Serial.println("RecOvertop Off");
  } 
  else           
  { 
    recordOvertop = true;
    Serial.println("RecOvertop On"); 
  } 
}


void recordSwitch() //  Read Recording Pin (foot pedal)
{
  bool stateNow = digitalRead(PedalInL);
  if(stateNow != recordSwitchLast)
  {
    recordSwitchLast = stateNow;
    if(stateNow == recordSwitchOff){ccIn (channelIn, 64, 0);}  // Send as 'off' cc value.
    else{ccIn (channelIn, 64, 127);}  // Send as 'On' cc value.
  }
}
