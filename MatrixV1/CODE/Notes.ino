void midiSendNoteOn(byte pitch, byte vel, byte channel, int stepOffNum)
{
  if(conductor && !conductorChange && channel != channelOut && channel != channelIn)
    {ccOutMidi1(102,127,channel);} // send message saying this is a transpose
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
  if(conductor && !conductorChange && channel != channelOut && channel != channelIn)
    {ccOutMidi1(102,126,channel);} // send message saying this is a transpose off
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
    if(nextPitch > mutePitch[1] && nextPitch < mutePitch[0])
    {
      Serial.print(" S:");Serial.print(stepNum);    
      if(bitRead(enRecLoopFlag,5) && enRecLoopValue != 0) // Add Velocity value if button pressed and encoder turned
      {
        nextVelocity = enRecLoopValue;                     
        noteArray[4][lastAddr] =  nextVelocity; 
      }          
      if(bitRead(enRecLoopFlag,6) && enRecLoopValue != 0) // Add Sustain value if button pressed and encoder turned
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
  for(int i=20; i< 108; i++) // check if off note needs to be played for this current step.
  {
    int noteOffStep = notesStepOff[channelOut-1][i];
    if(noteOffStep == stepN)  
    {
      Serial.print(" S:");Serial.print(stepN); 
      midiSendNoteOff(i, 0, channelOut);
    }
    if(stepN == stepEnd)
    {
      if((noteOffStep > stepEnd) && (noteOffStep < 32764))// || noteOffStep < stepStart)
      {
        if(!recordOvertop)  // allows overlap
        {
          Serial.print(" noteOffStep: "); Serial.print(noteOffStep); Serial.print(" StepEnd:");Serial.print(stepN); 
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
    // position incoming notes in centre of screen  
    if(!conductor)
    {  
      winYNoteStart = pitch - 2; 
      if(winYNoteStart < 21){winYNoteStart = 21;}                               // Make sure does not go out of bounds
      if(winYNoteStart > 95){winYNoteStart = 95;}                             // Make sure does not go out of bounds
    }

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
      conductorChange = true; 
      for(int i=1; i<17; i++)
      {
        if(conductorOut[i][0] == 5)    // main out.
          {midiSendNoteOn(pitch, velocity, i, 32765);}
        else if(i==channelOut){notesStepOff[i-1][pitch] = 32765;} // note does not want to be played - but needs to be logged (for conductor play)        
      }
    }   
    else if(recordOvertop || recording)
    {
      if(recording && newAddr == 2){stepCount = 0; stepTick = 0;}                   // Reset, so notes start from begining of loop.                            

      int stepsPerNote = 24 / notesPerBeat; 
      int s = (stepCount / stepsPerNote );  // Make sure the on Note is set at first step indicies.
      s = s * stepsPerNote;
      insertEvent(s, pitch, velocity, -1); // record note
      midiSendNoteOn(pitch, velocity, channelOut, 32765);
    }
    else
      {midiSendNoteOn(pitch, velocity, channelOut, 32765);}
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
     ledRefreshNotes();
    }
    else if(conductor)  
    {
      for(int i=1; i<17; i++)
      {
        if(conductorOut[i][0] > 0) // an output has been set.
        {
          int offPitch = pitch;
          if(conductorOut[i][0] < 5){offPitch += conductorOut[i][1];}
          if(offPitch < 21){offPitch = 21;}if(offPitch >107){offPitch = 107;}
          midiSendNoteOff(offPitch, 0, i);
        }
        else if(i==channelOut){notesStepOff[i-1][pitch] = -1;} // note does not want to be played - but needs to be logged (for conductor play)        
      }
    } 
    else if(conductorTransposeOffFlag == true){conductorTransposeOffFlag = false;} // do nothing.
    else if(!transposeActive && !conductor){midiSendNoteOff(pitch, 0, channelOut);}
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

  ledRefreshNotes();                      
}


void updateVelDisplay()
{
  int stepsPerLed = 24 / notesPerBeat; 
  int firstStep = winXStepStart * stepsPerLed;                                       // Windowed Notes.  Which step count is on the left (typcially step 0)
  int lastStep = firstStep + (32 * stepsPerLed);                                 // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
  
  //  Get velocity Values.
  int yS = winYNoteStart;                                       // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
  int yE = winYNoteStart + 13;                                  // Windowed Notes.  Which Note pitch is on the y axis rows - therefore yStart + 14

  for(int i=0; i<32; i++){velArray[i]=0;}                      // Clear Vel Array.
  
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
            int LEDStep = (searchStep - firstStep) / stepsPerLed ; //Serial.print("  searchVel: ");Serial.print(searchVel);
            float v = 13/127;// Serial.print("  v: ");Serial.print(v);
            int screenVel = (float)v * searchVel;// Serial.print("  screenVel: ");Serial.println(screenVel);
            velArray[LEDStep] = screenVel;
        }
      }
    }
    searchAddr = noteArray[0][searchAddr];   
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

void notesMutePitch(int value)
{ 
  if(value == 1) // up
  {
    mutePitch[0] ++; mutePitch[1] ++;
    if (mutePitch[1] > mutePitch[2]) {--mutePitch[0]; --mutePitch[1];}
  } 
  else if(value == 0)  // down
  { 
    --mutePitch[0]; --mutePitch[1];
    if (mutePitch[0] < mutePitch[3]) {mutePitch[0]++; mutePitch[1]++;}
  }
  if(mutePitch[0] > 108){mutePitch[0] = 108;}
  if(mutePitch[1] < 20){mutePitch[1] = 20;}
  Serial.print("muteP: ");Serial.print(mutePitch[0]); Serial.print(" , ");Serial.println(mutePitch[1]);
}

void notesMutePitchCalibrate()
{
  int currentAddr = noteArray[0][0]; // get first note.
  int currentStep;
  byte lowestNote = 108; byte highestNote = 20;
  
  do // find note
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}
    int currentPitch = noteArray[3][currentAddr];   
    if(currentPitch < lowestNote){lowestNote = currentPitch;}
    if(currentPitch > highestNote){highestNote = currentPitch;}
    
    currentAddr = noteArray[0][currentAddr];  // get next address. 
  } while(currentStep != 32767);   
  mutePitch[2] = highestNote + 2;  mutePitch[3] = lowestNote - 2; 
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
