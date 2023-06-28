// ********************* TRANSPOSE ******************************************************************

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
    if(pitch < 21){pitch = 21;} if(pitch>107){pitch = 107;}
    Serial.print(" pitch: ");Serial.println(pitch);          
  }
 
  int amount = pitch - transposeNoteTonic;
   
  //Serial.print(" Amount: ");Serial.print(pitch); Serial.print(" transposeNoteTonic: ");Serial.print(transposeNoteTonic);
  
  bool scaleOn = false;                                                               // check if chord array populated.  
  for(int n=21;n<33;n++){if(chordArray[n] >0){scaleOn = true;}}  
  
  // check if harmonic (+/- 12)  if so, then just copy and shift.  No need to massage notes onto scale.  
  int noteHarmonic = (transposeNoteTonic%12)+12; // get lowest pitch harmonic (multiple) of this note.
  bool harmonic = false;
  for(int i=noteHarmonic; i<108; i++) //checking to see if is the same as the saved transpose array.  Just use that
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
    if(storedPitch > 20 && storedPitch < 108)
    {
      int newNote = storedPitch + amount;                                // Shift note up or down by amount

      if(newNote < 21){newNote = 21;}
      if(newNote > 107){newNote = 107;}
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
  ledRefreshNotes();        
}

void transposeByInterval(int v)
{
  byte p = getTransposeTonic();
  Serial.print("tranpose: ");Serial.print(p);
  transposePitch = p + v;
  Serial.print(" to: ");Serial.println(transposePitch);
  transposeFlag = true;
}

void transposeState()  // activate the transpose function.
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
  else{transposeActive = false;Serial.println("Transpose off");}
  ledRefresh();
}


//int transposeFindNote()  //  find 1st, 2nd and 3rd note 
//{
//  int n = 0;
//  chordPitch[0] = 0;chordPitch[1] = 0;chordPitch[2] = 0;
//  for(int i=20; i<108; i++)
//  {
//    if(notesStepOff[channelIn-1][i] > -1 && n < 3) 
//    {
//      chordPitch[n] = i; 
//      n++;
//    }
//  }  
//  return chordPitch[transposeChordNote];
//}

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
  for(int i=n; i<108; i++) //Fill Array
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
  for(int i=0; i<108; i++){chordArray[i] = 0;} 
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
}

//********************** PHASE ****************************************************************

void canonPhaseNotes(int amount)
{
  Serial.print(" phasing by amount: ");Serial.println(amount);

  int stepsPerLed = 24 / notesPerBeat; 
  stepsInLoop;

  int currentAddr = 0;  
  int currentStep = 0;
  
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
      if(currentStep < stepsPerLed)
      {
        removeEvent(currentStep, 1, currentPitch); Serial.println("StepRemoved");
        if(currentStepOff < stepsPerLed)  {currentStepOff = stepsInLoop + currentStepOff + 1;} 
        insertEvent(stepsInLoop + currentStep + 1, currentPitch, currentVel, currentStepOff); 
      }
      else
      {
        int newStartStep = currentStep - stepsPerLed;
        int newEndStep = currentStepOff - stepsPerLed;
        noteArray[1][currentAddr] = newStartStep >> 8;
        noteArray[2][currentAddr] = newStartStep & 0xFF;    
        noteArray[5][currentAddr] = newEndStep >> 8;
        noteArray[6][currentAddr] = newEndStep & 0xFF;                 
      }
    }
    else if(amount > 0) // phase right
    {
      int newStartStep = currentStep + stepsPerLed;
      int newEndStep = currentStepOff + stepsPerLed;      
      
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

  ledRefreshNotes();
  nextEvent(Addr);  // refreshing the next pitch and step - as the ones in memory will be wrong after this move.  
}
