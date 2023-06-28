void conductorPlay()
{
  //  find 1st, 2nd and 3rd note 
  int chordPitchLast[3];                                      // Remember Last Finger Pitch State
  chordPitchLast[0] = chordPitch[0]; chordPitchLast[1] = chordPitch[1]; chordPitchLast[2] = chordPitch[2];
  chordPitch[0] = 0; chordPitch[1] = 0; chordPitch[2] = 0;     // Get New Finger Pitch State
  
  int n = 0;
  for(int i=24; i<108; i++)
  {   
    if(notesStepOff[channelOut-1][i] > -1) 
    {
      chordPitch[n] = i; 
      n++;
    }
    if(n>2){break;}
  }  
    Serial.print("chordPitch[0]: ");Serial.print(chordPitch[0]);Serial.print(" ");
    Serial.print(chordPitch[1]);Serial.print(" ");
    Serial.print(chordPitch[2]);Serial.println(" ");
  
  if(transposeActive)  // Check if transpose is active - so can shift local notes
  {
    int finger = conductorOut[channelIn][0];
//    Serial.print("trans finger: "); Serial.println(finger);
    if(finger > 0 && finger < 5)
    {  
      offsetNotes(chordPitch[finger-1]);  
//      Serial.print("trans Note: "); Serial.println(chordPitch[finger-1]);
    }      
  }

  //  Assign from Matrix and send.
  for(int i=1; i<17; i++)
  {
    int finger = conductorOut[i][0];
    if(finger > 0 && finger < 5 && i != channelOut)
    {
//      Serial.print("midi: ");Serial.print(i);Serial.print(" finger: ");Serial.println(finger);
      finger = finger - 1; // as array starts from 0.
      if(chordPitch[finger] != chordPitchLast[finger] && chordPitch[finger] > 24)  // check if same Pitch as last time so we don't resend the same note.
      {
        byte pitchIn = chordPitch[finger];
        byte pitchOut = pitchIn + conductorOut[i][1];  // shift note up or down if need be
        if(pitchIn > 107){pitchIn = 107;}
        if(pitchIn < 21){pitchIn = 21;}
        byte velocity = 100;
        byte midiChan = i; 
        midiSendNoteOn(pitchOut, velocity, midiChan, 32765); 
      }  
    } 
  }
}

void conductorCCSendChord(byte channel)
{
  uint16_t scaleKeys = 0;
  for(int i=24; i<36; i++){bitWrite(scaleKeys,i-24,chordArray[i]);}
  //Serial.print(" scaleKeys Send: "); Serial.println(scaleKeys,BIN);
  byte b1 = scaleKeys >> 8;
  byte b2 = scaleKeys & 0xFF;
  ccOutMidi1 (102, 127, channel-1);
  ccOutMidi1 (103, b1, channel-1);
  ccOutMidi1 (104, b2, channel-1);
}


void conductorAdjustMatrix (byte xPos, byte yPos)
{
   byte midiChan = xPos - 16; 
   byte finger = yPos + 1;
   byte currentState = conductorOut[midiChan][0];
   if(currentState == finger)  // is the same, so user removing
    {conductorOut[midiChan][0]=0;conductorOut[midiChan][1]=0;} 
   else
    {conductorOut[midiChan][0]=finger;}
   conductorMidiOutEdit = midiChan + 1;
   ledDisplay(3, 0);
}

void conductorAdjustOffset (byte yPos)
{
  int midiChan = conductorMidiOutEdit;
  if(midiChan > 0)  // just making sure the user just modified the conductor matrix
  {
    midiChan = midiChan - 1;
    int currentState = conductorOut[midiChan][1];
    if(yPos < 3)  {currentState --;}   // Down
    else          {currentState ++;}   // Up    
    if(currentState > 24){currentState=24;}
    if(currentState < -24){currentState=-24;}
    conductorOut[midiChan][1] = currentState;
    ledDisplay(3, 0); 
  }
}
