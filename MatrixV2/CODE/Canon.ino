// ********************* TRANSPOSE ******************************************************************
void transposeEncoder(int sign)
{
  if(sign == 0){sign = -1;}
  byte p = getTransposeTonic();
  Serial.print("tranpose: ");Serial.print(p);
  transposePitch = p + sign;
  Serial.print(" to: ");Serial.println(transposePitch);
  if(!startStop){offsetNotes(transposePitch);}
  else{transposeFlag = true;}
}

void offsetNotes(byte pitch)
{
  if(transposeNoteTonic == 127)  // transpose not set.
  {
    transposeCopy();
    transposeNoteTonic = getTransposeTonic();
  }

  if(conductorTransposeFlag)  // this stops the remote Canon tranpose from going larger than 1 octave from tranpose in.  Need to find bettr way..
  {
    conductorTransposeFlag = false;  
    if(conductorTransposeOffset == 127) // offset has not been set for this instance (reset when conductor activated)
    {
     int currentNote = getTransposeTonic();
     Serial.print(" pitch: ");Serial.print(pitch);
     int delta = pitch - currentNote;
     Serial.print(" delta: ");Serial.print(delta);
     int reduce = delta / 12; //pitch = pitch - (reduce * 12);
     Serial.print(" reduce: ");Serial.print(reduce);
     conductorTransposeOffset = reduce * 12; 
     
    }
    pitch = pitch - conductorTransposeOffset;
    if(pitch < noteMin){pitch = noteMin;} if(pitch>noteMax){pitch = noteMax;}
    Serial.print(" pitch: ");Serial.println(pitch);          
  }
 
  int amount = pitch - transposeNoteTonic;
   
  //Serial.print(" Amount: ");Serial.print(pitch); Serial.print(" transposeNoteTonic: ");Serial.print(transposeNoteTonic);
  
  bool scaleOn = false;                                                               // check if chord array populated.  
  for(int n=21;n<33;n++){if(chordArray[n] >0){scaleOn = true;}}  
  
  // check if harmonic (+/- 12)  if so, then just copy and shift.  No need to massage notes onto scale.  
  int noteHarmonic = (transposeNoteTonic%12)+12; // get lowest pitch harmonic (multiple) of this note.
  bool harmonic = false;
  for(int i=noteHarmonic; i<=noteMax; i++) //checking to see if is the same as the saved transpose array.  Just use that
  {
    if(i == pitch){harmonic = true;}
    i=i+11;
  }
  
  byte searchAddr = 0; 
  searchAddr = noteArray[0][searchAddr]; // get first note.

  int lastStep = 0; int lastNote = 0;  // need something better - so notes are note doubled up.
  
  for(int i=0;i<255;i++)
  {                             
    int currentStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr];
    if(currentStep != lastStep){lastStep = currentStep; lastNote = 0;}  // looking for notes on the same step - if not - then reset
    
    int storedPitch = noteArray[7][searchAddr];  //    
    if(storedPitch >= noteMin && storedPitch <= noteMax)
    {
      int newNote = storedPitch + amount;                                // Shift note up or down by amount

      if(newNote < noteMin){newNote = noteMin;}
      if(newNote > noteMax){newNote = noteMax;}
      if(scaleOn && !harmonic)                                            // If not harmonic or chord array is populated (that user has entered in desired chords)
      {        
        int deltaLast = newNote - noteArray[3][searchAddr];            // find the shift from last time (this will make shifting notes nicer
        if(deltaLast < 0){deltaLast = -1;} 
          else if(deltaLast > 0){deltaLast = 1;}
            else{deltaLast = 0;}
        if(chordArray[newNote] == 1)                                     // If a '1' this means its ok to place a note here
        {
          if(newNote == lastNote) {newNote = newNote+2;}            // Not adding a note on the last one you shifted.   Move down one if Positve transpose, up if Neg transpose                       
        }
        else                                                                // If a zero - then move up/down one more note.
        {
          newNote = newNote + deltaLast; 
          if(newNote == lastNote) {newNote = newNote - 2;}
        }     
      }
      noteArray[3][searchAddr] = newNote; lastNote = newNote;  
    }
    searchAddr = noteArray[0][searchAddr];  // go to next address. 
    if(searchAddr == 1){break;}                // If end event detected finish
  } 
  
  // End - but before we do... 
  nextPitch = noteArray[3][lastAddr];       // refreshes the next pitch - as we just shifted it..    
  ledRefreshScreen();      
}

void transposeByInterval(int v)
{
  byte p = getTransposeTonic();
  Serial.print("tranpose: ");Serial.print(p);
  transposePitch = p + v;
  Serial.print(" to: ");Serial.println(transposePitch);
  transposeFlag = true;
}

void transposeState(int state)  // activate the transpose function.
{
  switch (state)
  {
    case 1: // User has button pressed
    {
      editSettingsChord = true;
      transposeChordFlag = 1;
      keyboardRefresh(5); 
    }
    break;
    case 2: // User has relased button
    {
      editSettingsChord = false;  // stops editing chord using buttons
      transposeChordFlag = 0;     // stops editing chord using midi keyboard  
      if(settingsChanged)
      {
        settingSave(10);       
      }
      else
      {
        if(!transposeActive)
        {
          transposeActive = true;
          // Find the first note in loop.  This becomes the tonic.
          byte searchAddr = 0; 
          searchAddr = noteArray[0][searchAddr]; // get first note.        
          transposeNoteTonic = getTransposeTonic();
          transposeCopy();
          Serial.println("Transpose On");
        }
        else
        {
          transposeActive = false;
          Serial.println("Transpose off");
        }
        byte red = 255*transposeActive; leds[5] = CRGB(0,red,0); FastLED.show();    
      }           
      keyboardRefresh(0);    
    }
    break;
  }  
}

byte getTransposeTonic()
{
  byte searchAddr = 0; 
  searchAddr = noteArray[0][searchAddr]; // get first note.

  int firstStep = -1;
  int lowestPitch = 128;
  for(int i=0;i<255;i++)
  {                             
    int currentStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr];
    if(firstStep == -1){firstStep = currentStep;}
    if(currentStep > firstStep){break;} // you have left the first step.  No point in looking further.

    int searchPitch = noteArray[3][searchAddr];  //    
    if(searchPitch < lowestPitch){lowestPitch = searchPitch;}
    
    searchAddr = noteArray[0][searchAddr];  // go to next address. 
   } 
   return lowestPitch;
   Serial.print("TransposeTonic: ");Serial.println(transposeNoteTonic);
}

void transposeEditChord(byte note)
{
  Serial.print("Chord N:");Serial.println(note);
  byte stateLast = chordArray[note];
  if(stateLast == 0){stateLast = 1;} else{stateLast = 0;}  // could just invert - but this stops value corruption - ie: 255.
  int n = (note%12)+12; // get lowest pitch of this note.
  for(int i=n; i<=noteMax; i++) //Fill Array
  {
    chordArray[i]= stateLast;
    i=i+11;
  }  
}

void transposeSaveChord()
{
  byte b[12]; 
  for(int y=0; y<12; y++){b[y] = chordArray[44+y]; }      
  writeEEPROM(globalMemStart+10,b,12);  
}

void transposeClearChord()
{
  for(int i=0; i<=noteMax; i++){chordArray[i] = 0;} 
}

void transposeCopy()
{
  byte searchAddr = 0; 
  searchAddr = noteArray[0][searchAddr]; // get first note.
  for(int i=0;i<255;i++)
  {               
    noteArray[7][searchAddr] = noteArray[3][searchAddr];  // pitchAddr   
    searchAddr = noteArray[0][searchAddr];  // go to next address. 
    if(searchAddr == 1){break;}                            // Don't do anything if end event detected.  
  } 
  Serial.println(" TransposeCopied ");
}

//********************** PHASE ****************************************************************

void phaseOnOff()
{
  phaseActive = !phaseActive; 
  if(phaseActive)
  {
    byte r = 0, g = 0, b = 0;
    int lum = phaseAmount + 10;
    if(lum>20){lum=20;} if(lum<0){lum=0;}
    r = (255/20)*lum;
    b = 255 - r;
    leds[6] = CRGB(g,r,b); FastLED.show();  
  }
  else{leds[6] = CRGB(0,0,0); }
  FastLED.show();
}

void phaseStepAmount(int sign)
{
  if(sign == 1){phaseAmount++;} else{phaseAmount--;}   
  ledShowPhaseAmount();
  resetScreenActionLast = millis(); resetScreenFlag = true;
  byte r = 0, g = 0, b = 0;
  if(phaseActive)
  {
    int lum = phaseAmount + 10;
    if(lum>20){lum=20;} if(lum<0){lum=0;}
    r = (255/20)*lum;
    b = 255 - r;  
  }
  leds[6] = CRGB(g,r,b); FastLED.show();
}

void phaseNotes(int amount)
{
  if(amount == 0){amount = -1;}
  Serial.print(" phasing by amount: ");Serial.println(amount);

  int currentAddr = 0;  
  int currentStep = 0;

  int stepsToShift = phaseAmount * 6;  // minimum is a quarter note)
  
  currentAddr = noteArray[0][0]; // get first note address
  
  do // find event
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}

    int nextEventAddress = noteArray[0][currentAddr];  // get next address. 
    int currentPitch = noteArray[3][currentAddr]; 
    int currentVel = noteArray[4][currentAddr]; 
    int currentStepOff = (noteArray[5][currentAddr] << 8) + noteArray[6][currentAddr];
      
    if(amount < 0) // phase left
    {
      if(currentStep < stepsToShift)
      {
        removeEvent(currentStep, 1, currentPitch); Serial.println("StepRemoved");
        if(currentStepOff < stepsToShift)  {currentStepOff = stepsInLoop + currentStepOff + 1;} 
        insertEvent(stepsInLoop + currentStep + 1, currentPitch, currentVel, currentStepOff); 
      }
      else
      {
        int newStartStep = currentStep - stepsToShift;
        int newEndStep = currentStepOff - stepsToShift;
        noteArray[1][currentAddr] = newStartStep >> 8;
        noteArray[2][currentAddr] = newStartStep & 0xFF;    
        noteArray[5][currentAddr] = newEndStep >> 8;
        noteArray[6][currentAddr] = newEndStep & 0xFF;                 
      }
    }
    else if(amount > 0) // phase right
    {
      int newStartStep = currentStep + stepsToShift;
      int newEndStep = currentStepOff + stepsToShift;      
      
      if(newEndStep > stepsInLoop)
      {
        removeEvent(currentStep, 1, currentPitch); 
        newStartStep = (newStartStep % stepsInLoop) - 1; 
        newEndStep = newEndStep % stepsInLoop - 1;
        insertEvent(newStartStep, currentPitch, currentVel, newEndStep);           
      }
      else
      {
        noteArray[1][currentAddr] = newStartStep >> 8;
        noteArray[2][currentAddr] = newStartStep & 0xFF;    
        noteArray[5][currentAddr] = newEndStep >> 8;
        noteArray[6][currentAddr] = newEndStep & 0xFF;            
      }
    }
    currentAddr = nextEventAddress;  // load next address. 
  } while(currentStep != 32767);    

  ledRefreshScreen();
  nextEvent(Addr);  // refreshing the next pitch and step - as the ones in memory will be wrong after this move.  
}


// **************************  FREQUENCY ******************************************

void canonFreqAdjust(int amount)
{
  if(amount == 0){amount = -1;}
  
  Serial.print(" FreqChan: ");Serial.println(amount);

  int currentAddr = 0;  
  int currentStep = 0;
  
  currentAddr = noteArray[0][0]; // get first note address
  
  do // find event
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}
    int nextEventAddress = noteArray[0][currentAddr];  // get next address. 
    int currentStepOff = (noteArray[5][currentAddr] << 8) + noteArray[6][currentAddr];

    int stepLength = currentStepOff - currentStep;
    int newStepOn, newStepOff;
    if(amount < 0)  {newStepOn = currentStep / 2; stepLength = stepLength / 2; if(stepLength < 1){stepLength = 1;}}
    else            {newStepOn = currentStep * 2; stepLength = stepLength * 2;}
    newStepOff = newStepOn + stepLength;

    noteArray[1][currentAddr] = newStepOn >> 8;
    noteArray[2][currentAddr] = newStepOn & 0xFF;    
    noteArray[5][currentAddr] = newStepOff >> 8;
    noteArray[6][currentAddr] = newStepOff & 0xFF;     

    currentAddr = nextEventAddress;  // load next address. 
  } while(currentStep != 32767);     

  int sS = stepStart, sE = stepEnd;
  Serial.print(" sS ");Serial.print(sS);Serial.print(" sE ");Serial.println(sE);
  if(amount < 0)  {sS = stepStart / 2; sE = ((stepEnd + 1) / 2) -1;}
  else            {sS = stepStart * 2; sE = ((stepEnd + 1) * 2) -1;}
  if(sE <= sS){sE = sE + (stepsPerLED -1);}
  stepStart = sS;
  stepEnd = sE;
  Serial.print(" sS ");Serial.print(sS);Serial.print(" sE ");Serial.println(sE);
  ledRefreshScreen();
  nextEvent(Addr);  // refreshing the next pitch and step - as the ones in memory will be wrong after this move.  
}


// ***********************  PITCH INVERT  *************************************************

void canonInvert(int sign)
{
  if(sign == 0){sign = -1;}

  if(canonInvertAxis == 0) // flip axis has not been set.
  { 
    canonInvertFindMiddle();                    // find high and low notes
    transposeCopy();                            // Save current pattern (need original position to complete operation
  }
  int currentAddr = 0;  
  int currentStep = 0;
  currentAddr = noteArray[0][0]; // get first note address

  int c = 0;
  do // find event
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}
    int nextEventAddress = noteArray[0][currentAddr];  // get next address. 
    int pitch = noteArray[3][currentAddr];
    int storedPitch = noteArray[7][currentAddr];
    
    int newPitch = pitch;
    if(storedPitch < canonInvertAxis)
    {
      newPitch = pitch - sign;
      if(chordArray[newPitch] != 1){newPitch = newPitch - sign;} 
      if(chordArray[newPitch] != 1){newPitch = newPitch - sign;} 
    }
    else
    {
      newPitch = pitch + sign;
      if(chordArray[newPitch] != 1){newPitch = newPitch + sign;} 
      if(chordArray[newPitch] != 1){newPitch = newPitch + sign;} 
    }
    if(newPitch < 24){newPitch = 24;}if(newPitch > 104){newPitch = 104;}
    noteArray[3][currentAddr] = newPitch;

    currentAddr = nextEventAddress;  // load next address. 
    c++;
  } while(currentStep != 32767 || c<255);     

  ledRefreshScreen();
}

void canonInvertFindMiddle()  // find the highs and lowest notes and returns middle note
{
  int currentAddr = noteArray[0][0]; // get first note.
  int currentStep;
  byte lowestNote = noteMax; byte highestNote = noteMin;
  int noteCheck = 0;
  do // find note
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767){break;}
    int currentPitch = noteArray[3][currentAddr];   
    if(currentPitch < lowestNote){lowestNote = currentPitch;}
    if(currentPitch > highestNote){highestNote = currentPitch;}
    
    currentAddr = noteArray[0][currentAddr];  // get next address. 
    noteCheck ++;
  } while(currentStep != 32767);   
  
  int middle = winYNoteStart +6;
  if(noteCheck != 0){middle = ((highestNote - lowestNote)/2) + lowestNote;}
  //Serial.print("lowestNote: ");Serial.print(lowestNote); Serial.print("highestNote: ");Serial.print(highestNote);
  //Serial.print("middle: ");Serial.println(middle);
  canonInvertAxis = middle;
}

void canonInvertSetMiddle(int sign)
{
  if(sign == 0){--canonInvertAxis;}
  else{++canonInvertAxis;}
  //Serial.print("canonInvertAxis: ");Serial.println(canonInvertAxis);
  ledShowInvertLine(1);  
}
