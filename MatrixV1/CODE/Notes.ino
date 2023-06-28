void midiSendNoteOn(byte pitch, byte vel, byte channel, int stepOffNum)
{
//  if(conductor && conductorOut[channel - 1][0] < 3)
//    {ccOutMidi1(102,127,channel);} // send message saying this is a transpose
  if(notesStepOff[channel-1][pitch] > -1){midiSendNoteOff(pitch, vel, channel);}
  MIDI.sendNoteOn(pitch,vel,channel);
  MIDI2.sendNoteOn(pitch,vel,channel);
  notesStepOff[channel-1][pitch] = stepOffNum;
    
  Serial.print(" P:");Serial.print(pitch);
  Serial.print(" V:");Serial.print(vel); 
  Serial.print(" s:");Serial.print(stepOffNum);  
  Serial.print(" C: ");Serial.println(channel);   
}

void midiSendNoteOff(byte pitch, byte vel, byte channel)
{
//  if(conductor && conductorOut[channel - 1][0] < 3)
//    {ccOutMidi1(102,126,channel);} // send message saying this is a transpose off
  MIDI.sendNoteOff(pitch,vel,channel);
  notesStepOff[channel-1][pitch] = -1;  // set step to -1 
    
  Serial.print(" P:");Serial.print(pitch);
  Serial.print(" V:");Serial.print(vel); 
  Serial.print(" C: ");Serial.println(channel);
}

void playNote(int stepNum)
{
  byte count = 0;
  do
  {
    if(nextPitch >= mutePitch[0] && nextPitch <= mutePitch[1])
    {
      Serial.print(" S:");Serial.print(stepNum);  
      if(enRecLoopFlag != 0)                             // Recording of notes enabled.
      {
        if(bitRead(enRecLoopFlag,12))                     // Add Velocity value if button pressed and encoder turned
        {
          int v = nextVelocity + enRecLoopValue;
          if(v<1){v=1;} if(v>127){v=127;}
          nextVelocity = v;                     
          noteArray[4][lastAddr] =  v;   
          if(touchEventType == 3){ledShowNoteVelocity(0);}    //  If draw mode active   
        }
        if(bitRead(enRecLoopFlag,13))                     // Add Sustain value if button pressed and encoder turned
        {
          int newStepOff;
          if(nextStepOff > stepNum) // note off is after on
          {
            newStepOff = nextStepOff + enRecLoopValue;
            if(newStepOff <= stepNum){newStepOff = stepNum + 1;}
            if(newStepOff > stepEnd){newStepOff = newStepOff - stepEnd;}
          }
          if(nextStepOff < stepNum) // note off is before on
          {
            newStepOff = nextStepOff + enRecLoopValue;
            if(newStepOff >= stepNum){newStepOff = stepNum - 1;}
            if(newStepOff < stepStart){newStepOff = stepEnd - (stepStart - newStepOff);}
          }
          nextStepOff = newStepOff;            
          noteArray[5][lastAddr] = newStepOff >> 8; // Off step byte1
          noteArray[6][lastAddr] = newStepOff & 0xFF; // Off step byte 2            
        }
      }
      midiSendNoteOn(nextPitch,nextVelocity, channelOut, nextStepOff);
    }
    nextEvent(Addr);
    count ++;
    if(nextStep > stepNum){break;}
    if(nextStep > 32766){break;}
  }while(count < 12);
}



void playOffNote(int stepN)
{
  for(int i=noteMin; i<= noteMax; i++) // check if off note needs to be played for this current step.
  {
    int noteOffStep = notesStepOff[channelOut-1][i];
    if(noteOffStep == stepN)  
    {
      Serial.print(" S:");Serial.print(stepN); 
      midiSendNoteOff(i, 0, channelOut);
    }
    if(stepN == stepEnd)
    {
      if((noteOffStep >= stepEnd) && (noteOffStep < 32765))// || noteOffStep < stepStart)
      {
        if(!recordOvertop)  // allows overlap
        {
          //Serial.print(" noteOffStep: "); Serial.print(noteOffStep); Serial.print(" StepEnd:");Serial.print(stepN); 
          midiSendNoteOff(i, 0, channelOut);
        }
      }  
    }    
  }
}


void noteIn (byte chan, byte pitch, byte velocity)
{    
  if(chan == channelIn)
  {  
//    if(!conductor)   // position incoming notes in centre of screen  
//    {  
//      winYNoteStart = pitch - 2; 
//      if(winYNoteStart < noteMin){winYNoteStart = noteMin;}                               // Make sure does not go out of bounds
//      if(winYNoteStart > (noteMax-12)){winYNoteStart = noteMax-12;}                             // Make sure does not go out of bounds
//    }

    if((transposeActive || conductorTransposeFlag) && transposeChordFlag == 0 && !conductor)
    { 
      transposeFlag = true; 
      transposePitch = pitch; 
    }
    else if(transposeChordFlag > 0) // user wants to use keyboard to edit chord
    {
      if(transposeChordFlag == 1) // if using keyboard, this clears the transpose, as the user cannot 'see' the saved notes.
      {
        transposeChordFlag = 2;  // to show that the transpose has been clear, so do not clear again till next session
        transposeClearChord();  // clear transpose
      }
      transposeEditChord(pitch);  // add to transpose scale
    }
    else if(conductor)
    {         
      conductorNoteOnIn(pitch,velocity);
    }   
    else if(recordOvertop || recording)
    {
      if(recording && newAddr == 2){stepCount = 0; stepTick = 0;}                   // Reset, so notes start from begining of loop.                            

      int s = stepCount;  
      if(s%2 == 1){--s;} // if an odd number make even, as note starts go on even steps.
      insertEvent(s, pitch, velocity, -1); // record note
      midiSendNoteOn(pitch, velocity, channelOut, 32765);
    }
    else
      {midiSendNoteOn(pitch, velocity, channelOut, 32765);}
  }
  else
  {
    //midiSendNoteOn(pitch, velocity, chan, 32765);
  }
}

void noteOffIn (byte chan, byte pitch, byte velocity)
{   
  if(chan == channelIn)
  {
    if(recordOvertop || recording)
    {
     insertNoteEnd(-1, pitch, stepCount);  //Serial.print("Recording over top. Note Off In.  Pitch");Serial.println(pitch);
     midiSendNoteOff(pitch, 0, channelOut);
     ledRefreshScreen();
    }
    else if(conductor)  
    {
      conductorNoteOffIn(pitch,velocity);
    } 
    else if(conductorTransposeOffFlag == true){conductorTransposeOffFlag = false;} // do nothing.
    else if(!transposeActive && !conductor){midiSendNoteOff(pitch, 0, channelOut);}
  }
  else
  {
    midiSendNoteOff(pitch, velocity, chan);
  }
}

void notesOffAll()
{
  for(int i=0;i<16;i++)
  {
    if(conductorOut[i+1][0] > 0)
    {
      Serial.print(" notesOffAll Ch:");Serial.println(i);
      MIDI.sendControlChange(123,0,i+1);   //https://www.midi.org/specifications-old/item/table-3-control-change-messages-data-bytes-2
      for(int y=20;y<108;y++)
      {
        midiSendNoteOff(y, 0, i+1); 
        notesStepOff[i][y] = -1;   // pitch, velo, channel 
      }       
    }
  }
}

void tieNotes()
{
 //Serial.println(" Tie Notes Start.. ");
  byte pitch = tieNotePSE[0];
  int noteStepStart = tieNotePSE[1];  
  int noteStepEnd = tieNotePSE[2];
  int range = noteStepEnd - noteStepStart;

  removeEvent(noteStepStart, range, pitch);
  insertEvent(noteStepStart, pitch, 100, noteStepEnd);                
}


// ************* VELOCITY ***************************
    
void notesEncoderVelocity(int sign)  // adjust velocity using the encoder.
{
  if(sign == 0){sign = -1;} 
  if(enRecLoopValue == 0)                       //  loop to record not started yet.
  {
    bitWrite(enRecLoopFlag,12,1);               // Set Flag to enable recording of values
    int stepToEnd = stepCount - 1;
    if(stepToEnd < stepStart){stepToEnd = stepEnd;}
    enRecLoopStartStep = stepToEnd;         // get current loop point so can reset when pass this next loop around.
  }
  enRecLoopValue += sign*4;
  Serial.print(" enRecLoopValue: ");Serial.println(enRecLoopValue);     
}

void notesEnButtDrawVelocity()
{
  if(touchEventType != 3)
  { 
    touchEventType = 3; 
    ledShowNoteVelocity(0); 
    leds[12] = CRGB(0,0,255);
    Serial.println("VelDraw On ");
  }
  else
  {
    touchEventType = 0; ledRefreshScreen();
    leds[12] = CRGB(0,0,0);
    Serial.println("VelDraw Off ");
  }
  Serial.print("touchEventType: ");Serial.println(touchEventType);
  FastLED.show();
}

void updateVelDisplay()
{
  ledClearArray();
  int firstStep = winXStepStart * stepsPerLED;                                       // Windowed Notes.  Which step count is on the left (typcially step 0)
  int lastStep = firstStep + (nCols * stepsPerLED);                                 // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
  
  //  Get velocity Values.
  int yS = winYNoteStart;                                       // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
  int yE = winYNoteStart + 13;                                  // Windowed Notes.  Which Note pitch is on the y axis rows - therefore yStart + 14

  for(int i=0; i<nCols; i++){velArray[i]=0;}                      // Clear Vel Array.
  
  byte searchAddr = 0; 
  for(int i=0;i<255;i++)
  {               
    byte byte1 = noteArray[1][searchAddr]; 
    byte byte2 = noteArray[2][searchAddr]; 
    int searchStep = (byte1 << 8) + byte2;    // Get step num - so you know when to end loop.   
    byte searchPitch = noteArray[3][searchAddr];               
    byte searchVel = noteArray[4][searchAddr];  
    
    if(searchStep > 32766){break;}                            // Don't do anything if end event detected.

    if(searchVel >0)
    {
      if(searchStep >= firstStep && searchStep < lastStep)                             // If they are inside the x axis extent of the window
      {
        if(searchPitch >= yS && searchPitch < yE)                               // If they are inside of the y axis extent of the window
        { 
            int LEDStep = (searchStep - firstStep) / stepsPerLED ; //Serial.print("  searchVel: ");Serial.print(searchVel);
            float v = 13/127;// Serial.print("  v: ");Serial.print(v);
            int screenVel = (float)v * searchVel;// Serial.print("  screenVel: ");Serial.println(screenVel);
            velArray[LEDStep] = screenVel;
        }
      }
    }
    searchAddr = noteArray[0][searchAddr];   
  } 
}

// ******************* SUSTAIN *********************************************

void notesSustainChange(int sign)
{
  if(sign == 0){sign = -1;}
  enRecLoopValue += sign;
  bitWrite(enRecLoopFlag,13,1);
  enRecLoopStartStep = stepCount - 1;  // get current loop point so can reset when pass this next loop around.
  Serial.print(" Sustain: ");Serial.println(sign);      
}


// ******************* MUTE NOTES *********************************************

void noteMutePitchButton(int highLow)  // 1=high 0=low
{
  Serial.print("butt: "); Serial.println(highLow); 
  if(highLow == 1)
  {
    mutePitchActiveHigh = !mutePitchActiveHigh;
    if(mutePitchActiveHigh) {notesMutePitchCalibrate(); notesMutePitchLED(1, mutePitch[3]);}  // find the highest note
    else                    {mutePitch[1] = noteMax;  notesMutePitchLED(1, 127);} // reset so all notes play  
  }
  else
  {
    mutePitchActiveLow = !mutePitchActiveLow;
    if(mutePitchActiveLow)  {notesMutePitchCalibrate(); notesMutePitchLED(0, mutePitch[2]);} // find the lowest note
    else                    {mutePitch[0] = noteMin;   notesMutePitchLED(0, 127);} // reset so all notes play    
  } 
  Serial.print(" mutePitchActiveHigh: "); Serial.println( mutePitchActiveHigh); 
  Serial.print(" mutePitchActiveLow: "); Serial.println( mutePitchActiveLow); 
}

void notesMutePitchLED(int highLow, int note)
{
  Serial.print("adjustLED: "); Serial.print(highLow); Serial.print("  "); Serial.println(note); 
  int noteColor = int(pow((note-20),float(1.2)));  // note 128=255, note 100=200;
  if(noteColor > 255){noteColor=255;}
  byte b = noteColor;
  byte r = 255 - b;
  byte g = 0;
  Serial.print("noteColor: "); Serial.print(noteColor); Serial.print(" r:"); Serial.print(r);Serial.print(" g:"); Serial.print(g);Serial.print(" b:"); Serial.println(b);
  if(note != 127){leds[11-highLow] = CRGB(g,r,b); } 
  else{leds[11-highLow] = CRGB(0,0,0);}
  
  FastLED.show(); 
}

void notesMutePitchEncoder(int highLow, int value)
{ 
  Serial.print("adjust: "); Serial.print(highLow); Serial.print("  "); Serial.println(value); 
  if(highLow)  // high knob
  {
    if(value) {mutePitch[1] ++;}  else {--mutePitch[1];}
    if(mutePitch[1] <= mutePitch[0])    {mutePitch[1] = mutePitch[0] + 1;}
    if(mutePitch[1] > noteMax)              {mutePitch[1]=noteMax;}
  }
  else     // low knob
  {
    if(value) {mutePitch[0] ++;}  else {--mutePitch[0];}
    if(mutePitch[0] >= mutePitch[1])    {mutePitch[0] = mutePitch[1] - 1;}
    if(mutePitch[0] < noteMin)               {mutePitch[0]=noteMin;}
  }  
  notesMutePitchLED(highLow, mutePitch[highLow]);
  Serial.print("muteP: ");Serial.print(mutePitch[0]); Serial.print(" , ");Serial.println(mutePitch[1]);
}

void notesMutePitchCalibrate()  // find the highs and lowest notes
{
  int currentAddr = noteArray[0][0]; // get first note.
  int currentStep;
  byte lowestNote = noteMax; byte highestNote = noteMin;
  
  do // find note
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}
    int currentPitch = noteArray[3][currentAddr];   
    if(currentPitch < lowestNote){lowestNote = currentPitch;}
    if(currentPitch > highestNote){highestNote = currentPitch;}
    
    currentAddr = noteArray[0][currentAddr];  // get next address. 
  } while(currentStep != 32767);   
  mutePitch[3] = highestNote + 2;  mutePitch[2] = lowestNote - 2; 
  mutePitch[0] = mutePitch[2];  mutePitch[1] = mutePitch[3]; 
}


void prePattern()
{
  // B3 = midi val 59, B2 47 = 24.  As the array starts at 0 B0(23) = 0. So 23-23 = 0
  byte pitch = winYNoteStart;
  insertEvent(0, pitch, 100, -1);
  insertEvent(1, pitch, 0, -1);  
  insertEvent(8, pitch, 100, -1);
  insertEvent(9, pitch, 0, -1);   
  insertEvent(16, pitch, 100, -1);
  insertEvent(17, pitch, 0, -1);   
  insertEvent(24, pitch, 100, -1);
  insertEvent(25, pitch, 0, -1);   
}
