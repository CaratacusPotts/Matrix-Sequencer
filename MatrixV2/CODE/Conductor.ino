void conductorFindNoteOrder()    //  find 1st, 2nd and 3rd note 
{
  conductorFinger[0] = 0; conductorFinger[1] = 0; conductorFinger[2] = 0;  conductorFinger[3] = 0;   // Get New Finger Pitch State
  int n = 0;
  for(int i=noteMin; i<=noteMax; i++)
  {   
    if(conductorNoteIn[i] > 0) 
    {
      conductorFinger[n] = i; 
      ++n;
    }
    if(n>3){break;}
  }  
}

void conductorNoteOnIn(byte pitch, byte velocity)
{
  conductorChange = true; 
  conductorNoteIn[pitch] = velocity;
  for(int i=0; i<16; i++)
  {
    bool all = bitRead(conductorOut[i][0],5);
    if(all)    // all notes out.
    {
      pitch = pitch + conductorOut[i][1];  // add offset if available.
      if(pitch > noteMax){pitch = pitch - conductorOut[i][1];} if(pitch<20){pitch = pitch - conductorOut[i][1];}  // keep in bounds 
      midiSendNoteOn(pitch, velocity, (i+1), 32765);
    }     
  }  
}

void conductorNoteOffIn(byte pitch, byte velocity)
{
  conductorNoteIn[pitch] = 0;
  for(int i=0; i<16; i++)
  {
    if(conductorOut[i][0] > 0) // an output has been set.
    {
      int pitchOffset = conductorOut[i][1];
      pitch = pitch + pitchOffset;  // add offset if available.
      if(pitch > noteMax){pitch = pitch - pitchOffset;} if(pitch<noteMin){pitch = pitch - pitchOffset;}  // keep in bounds 
      midiSendNoteOff(pitch, 0, (i+1));
    }      
  }
  conductorFindNoteOrder();  
}

void conductorPlayNote(byte pitch, int chan)
{
  byte pitchOut = pitch + conductorOut[chan][1];  // shift note up or down if need be
  byte velocity = conductorNoteIn[pitch];
  
  if(pitchOut > noteMax){pitchOut = noteMax;}
  if(pitchOut < noteMin){pitchOut = noteMin;}
  
  midiSendNoteOn(pitchOut, velocity, chan+1, 32765);   
}

void conductorPlay()
{
  int fingerLast[] = {conductorFinger[0],conductorFinger[1],conductorFinger[2],conductorFinger[3]};           // Remember Last Finger Pitch State
  conductorFindNoteOrder();
  
  for(int i=0; i<16; i++)
  {
    int conduct = conductorOut[i][0];
    bool allThrough = bitRead(conduct,5);
    if(conduct != 0 && !allThrough) 
    {
      if(bitRead(conduct,0) == 1 && conductorFinger[0] != fingerLast[0]) {conductorPlayNote(conductorFinger[0],i);}
      if(bitRead(conduct,1) == 1 && conductorFinger[1] != fingerLast[1]) {conductorPlayNote(conductorFinger[1],i);}
      if(bitRead(conduct,2) == 1 && conductorFinger[2] != fingerLast[2]) {conductorPlayNote(conductorFinger[2],i);}
      if(bitRead(conduct,3) == 1 && conductorFinger[3] != fingerLast[3]) {conductorPlayNote(conductorFinger[3],i);}
    }
  } 
  
//  if(transposeActive)  // Check if transpose is active - so can shift local notes
//  {
//    int finger = conductorOut[channelIn][0];
////    Serial.print("trans finger: "); Serial.println(finger);
//    if(finger > 0 && finger < 5)
//    {  
//      offsetNotes(conductorFinger[finger-1]);  
////      Serial.print("trans Note: "); Serial.println(conductorFinger[finger-1]);
//    }      
//  }
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
