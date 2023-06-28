void settingModeSelect(int enVal)
{
  if(enVal == 1){++Mode; if(Mode > 1) {Mode = 1;}} 
  else          {--Mode; if(Mode < -2){Mode = -2;}}  
 
  switch (Mode)
  {
    case 0:   // LOOP MODE
    {
      if(songMode){saveSong(songCurrent);}
      songMode = false;
      keyboardActive = false;
      loopMode = true;  
      leds[4] = CRGB(0,255,0);  FastLED.show();
      Serial.println("loopMode");                
    }
    break;
    case -1:  // KEYBOARD MODE
    {
      keyboardActive = true; conductor = false;
      leds[4] = CRGB(255,0,0);  FastLED.show();
      Serial.println("keysActiv");              
    }
    break;
    case -2:  // CONDUCTOR MODE
    {
      keyboardActive = false; conductor = true;
      Serial.println("ConductorActiv");  
      // send Chords if channel outs set
      for(int i=0; i<16; i++)
      {if(conductorOut[i][0] > 0){conductorCCSendChord(i+1);}}  
      leds[4] = CRGB(255,255,0);  FastLED.show();                  
    }
    break;  
    case 1:   // SONG MODE
    {
      loopMode = false; songMode = true;     // we know that by whatever action this must be turned off if it was on before.
      loadSong(songCurrent);             
      songStep = songStartStep; loadLoopActive = true;                                                // Load up loop to start playing. 
      leds[4] = CRGB(0,0,255);  FastLED.show();
      Serial.println("songMode");               
    }
    break;                         
  }
  keyboardRefresh(0); 
  ledRefreshScreen(); 
}

void settingsEdit(int value)
{
  if(settingChange){settingSave(editSettings);}  // if user has just changed a setting  Save first.
  
  switch(value)
  {
    case 0: {--editSettings; if(editSettings < 1){editSettings = 1;}  } break; 
    case 1: {editSettings++; if(editSettings > 2){editSettings = 2;}  } break; 
    case -1:{if(editSettings == 0){editSettings = 1;}else{editSettings = 0;}} break; 
  }
  Serial.print(" Edit Setting: ");Serial.println(editSettings);
  switch(editSettings)
  {
    case 0: { ledRefreshScreen();                                     } break;
    case 1: { ledShowMidiChan();                                      } break;
    case 2: { ledShowConductorChan();                                 } break;
//    case 4: { ledShowPhaseAmount();                                   } break;
//    case 10: { ledShowScaleNotes();                                    } break;
  }
}

void settingChange(int xPos, int yPos)
{
  Serial.print(" Editing Setting: ");Serial.print(editSettings); Serial.print(" x: ");Serial.print(xPos);Serial.print(" y: ");Serial.println(yPos);
  switch(editSettings)
  {
    case 1: { settingChangeMidiChans(xPos, yPos);                   } break;
    case 2: { settingChangeConductor(xPos, yPos);                   } break;
//    case 4: { settingChangePhase(yPos);                             } break;
//    case 10: { settingChangeScaleNotes(yPos);                        } break;
  }
  settingsChanged = true;   
}


void settingSave(int setting)
{
  switch (setting)
  {
    case 1:  // Midi In/Out Settings
    {
      byte b[2]; 
      b[0] = channelOut; b[1] = channelIn;
      writeEEPROM(globalMemStart+1,b,2);                   
    }
    break;
    case 2: // Conductor Edit
    {
      conductor = false;
      for(int i=0; i<16; i++){if(conductorOut[i][0] > 0){conductor = true;}}
      conductorMidiOutEdit = -1;  // resets for next time.                   
    }
    break;
    case 4:  // Phase Amount
    {
      byte b[1]; 
      b[0] = phaseAmount + 128;   //Serial.print("phaseAmount[0]: ");Serial.print(b[0]);
      writeEEPROM(globalMemStart+5,b,1);                   
    }
    break;
    case 10:  // Scale Notes
    {
      transposeSaveChord();  // save to memory                 
    }
    break;    
  } 
  settingsChanged = false;
}

void settingChangeMidiChans(int xPos, int yPos)
{
  if(xPos < 8)  // User adjusting input Channel  
  {
    if(yPos < 8) // down
    {
      if(channelIn <= 1){channelIn = 0;}  
      else{--channelIn;}
    }
    else  // up
    {
      if(channelIn >= 15){channelIn = 16;}    
      else{++channelIn;}  
    }
    Serial.print(" ChanIn: ");Serial.println(channelIn);
  }
  else  // User adjusting output Channel  
  {
    if(yPos < 8) // down
    {
      if(channelOut <= 1){channelOut = 0;}  
      else{--channelOut;}
    }
    else  // up
    {
      if(channelOut >= 15){channelOut = 16;}    
      else{++channelOut;}  
    }
    Serial.print(" ChanOut: ");Serial.println(channelOut);
  }                      
  ledShowMidiChan();  
}

void settingChangeConductor(int xPos, int yPos)
{
  if(yPos < 6 && xPos < 16) // User adjusting conductor matrix.
  {
     byte midiChan = xPos; 
     int mask = 1 << yPos;
     conductorOut[midiChan][0] ^= mask;
     conductorOut[midiChan][1]=0;
     conductorMidiOutEdit = midiChan + 1;
  }
 
  if(yPos > 5) // User adjusting conductor Note Offset.
  {
    int midiChan = conductorMidiOutEdit;
    if(midiChan > 0)  // just making sure the user just modified the conductor matrix
    {
      midiChan = midiChan - 1;
      int currentState = conductorOut[midiChan][1];
      if(yPos < 10) {--currentState; if(currentState < -24){currentState=-24;}}   // Down
      else          {++currentState; if(currentState > 24){currentState=24;}}   // Up     
      conductorOut[midiChan][1] = currentState;
    }
  }  
  ledShowConductorChan();
}

void settingStartStop()
{
  startStop = !startStop;                                       // toggle state
  if(startStop && clkSync){loopStartFlag = true;}  
  else if(startStop && !clkSync){ loopStart(); }               
  else{ loopStop(); }   
  byte r = 255*startStop;
  byte g = 255*clkSync;
  leds[0] = CRGB(g,r,0);
  FastLED.show();
  Serial.print("Play: ");Serial.print(startStop);   
}
void settingChangeBPM(int value)
{
  if(value == 1){bpm ++;} else if(value == 0){bpm --;}
  intClockuS = (60000000 /(long)bpm) / 24; 
  ledShowBPM();
  resetScreenActionLast = millis(); resetScreenFlag = true;
  Serial.print(" bpm");Serial.println(bpm); 
}

void settingClkSync (int value)
{
  if(value == 1){clkSync = true;}   
  else if(value == 0){clkSync = false;}  
  Serial.print(" clkSync: ");Serial.println(clkSync);  
}

void settingChangeLoopEndPoint(int value)
{
  if(songMode)
  {
    if(value == 1)  {++songEndStep; if(songEndStep>=songSteps){songEndStep = songSteps-1;}} 
    else            {--songEndStep; if(songEndStep<=songStartStep){songEndStep = songStartStep+1;}} 
    Serial.print("songStepEnd ");Serial.println(songEndStep);  
  }
  else
  {
    int newEnd = stepEnd;
    if(value == 1){newEnd = newEnd + stepsPerLED;} else{newEnd = newEnd - stepsPerLED;}   
    //if(newEnd > stepsInLoop){newEnd = stepsInLoop;}
    if(newEnd <= stepStart){newEnd = stepStart + stepsPerLED;}
    stepEnd = newEnd;
    ledRefreshScreen();    
    Serial.print("stepEnd ");Serial.println(stepEnd);      
  }
}

void settingChangeLoopStartPoint(int value)
{
  if(songMode)
  {
    if(value == 1)  {++songStartStep; if(songStartStep>=songEndStep){songStartStep = songEndStep-1;}} 
    else            {--songStartStep; if(songStartStep<0){songStartStep = 0;}} 
    Serial.print("songStepStart ");Serial.println(songStartStep);  
  }
  else
  {  
    int newStart = stepStart;
    if(value == 1){newStart = newStart + stepsPerLED;} else{newStart = newStart - stepsPerLED;}        
    if(newStart >= stepEnd){newStart = stepEnd - stepsPerLED;}
    if(newStart < 0){newStart = 0;}
    stepStart = newStart;             
    ledRefreshScreen();  
    Serial.print("stepStart ");Serial.println(stepStart);  
  }
}

void settingClearNotes()
{
  initialiseNoteArray(); 
  ledClearArray();
  notesOffAll();  
  Serial.println("Clear");
}

void settingChangeStepPerBeat(int value)
{
  // stepsPerLedArray[] = {2,3,4,6,8,12,24,48,96};    // 1/12,1/8,1/6,1/4,1/3,1/2,1,2,1,2,4
  int pos = 0;
  for(int i=0; i<9; i++)
  {
    if(stepsPerLedArray[i] == stepsPerLED){pos = i;}
  }
  if(value == 1){++pos;} else{--pos;}
  if(pos<0){pos=0;}if(pos>8){pos=8;}
  
  stepsPerLED = stepsPerLedArray[pos];
  ledRefreshScreen(); 
  ledShowStepsPerBeat(); 
  resetScreenActionLast = millis(); resetScreenFlag = true;
  byte r = (pos*32)-1;
  byte b = (255/pos);
  leds[3] = CRGB(0,r,b); FastLED.show();
  Serial.print("stepsPerLED ");Serial.println(stepsPerLED);   
}

void settingChangeScaleNotes(int butt)
{
  Serial.print(" Editing Scale: ");
  byte note = 48 + butt;  // 48 = C3  
  Serial.println(note);
  transposeEditChord(note);
  keyboardRefresh(5);
  //ledShowScaleNotes();
}

void settingChangePhase(int yPos)
{
  if(yPos < 6){phaseAmount --;}
  else{phaseAmount ++;}
  if(phaseAmount < -99){phaseAmount = -99;}
  if(phaseAmount > 99){phaseAmount = 99;}        
  ledShowPhaseAmount();
}
