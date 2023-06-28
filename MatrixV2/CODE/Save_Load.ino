void nextEvent (byte addr)
{
  lastAddr = addr;
  Addr = noteArray[0][addr]; 
  byte byte1 = noteArray[1][addr]; 
  byte byte2 = noteArray[2][addr]; 
  nextStep = (byte1 << 8) + byte2;                   
  nextPitch = noteArray[3][addr];                    
  nextVelocity = noteArray[4][addr];     
  nextStepOff = (noteArray[5][addr] << 8) + noteArray[6][addr];        
}

void getNewAddress()
{
  int nAddr = newAddr;  if(nAddr > 253){nAddr = 0;}  //if got to end, start from begining
  for(int i=nAddr; i< 255; i++)
  {
    if(noteArray[0][i] == 255){newAddr = i; i=256;}
  }
}

void insertEvent(int stepN, byte pitch, byte vel, int noteStepEnd) // (0 = main, 1 2nd (if dual display)
{  
 //Serial.print(" Insert Start.  Step: ");Serial.print(stepN);Serial.print(" Pitch: ");Serial.print(pitch);Serial.print(" Vel: ");Serial.println(vel);  
  int searchAddr = noteArray[0][0];; 
  int prevAddr = 0;
  int searchStep;
  
  for(int i=0; i<256; i++)                                // find an event with the same step number
  {   
    searchStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr];  
    int nextAddr = noteArray[0][searchAddr];
    if(searchStep >= stepN)  // Found suitable slot.  Add and point to new Address 
    {
      noteArray[0][prevAddr] = newAddr;                     // send previous link to this
      
      // Add to end bloc
      noteArray[0][newAddr] = searchAddr;   
      byte byte1 = stepN >> 8; byte byte2 = stepN & 0xFF;  
      noteArray[1][newAddr] = byte1;
      noteArray[2][newAddr] = byte2;
      noteArray[3][newAddr] = pitch; 
      noteArray[4][newAddr] = vel;   
      
      if(noteStepEnd != -1){byte1 = noteStepEnd >> 8; byte2 = noteStepEnd & 0xFF;}  // if user knows already the end point
      else if(noteStepEnd == -1){byte1 = 32765 >> 8; byte2 = 32765 & 0xFF;}         // insert dummy step, that can be found
      noteArray[5][newAddr] = byte1; // Off step byte1
      noteArray[6][newAddr] = byte2; // Off step byte 2

      getNewAddress();  // update new address.     

      i=256; break;
    }
    prevAddr = searchAddr;
    searchAddr = nextAddr;   
    if(searchStep == 32767){break;}
    
    //Serial.print(" PreAddr ");Serial.print(prevAddr);Serial.print(" addr: ");Serial.print(addr);Serial.print(" Step ");Serial.println(s);
  }
}

  
void insertNoteEnd(int noteStepStart, byte pitch, int noteStepEnd)
{
//  Serial.print(" noteStepStart ");Serial.print(noteStepStart);
//  Serial.print("  noteStepEnd ");Serial.println(noteStepEnd);
  int searchAddr = 0; 
  searchAddr = noteArray[0][searchAddr]; // get first note.

  if(noteStepStart < 0 && pitch == 2) // Finish unfineded notes .  -1= don't know startStep, Pitch:2= lets function know to add any unfinised at end
  {
    int currentStep;
    do 
    {   
      currentStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr]; 
      if(currentStep == 32767){break;}
      
      int searchPitch = noteArray[3][searchAddr];
      int noteStepEnding = (noteArray[5][searchAddr] << 8) + noteArray[6][searchAddr]; 

      if(noteStepEnding == 32765) // Found a pitch event with no registered off note.  Use this.
      {
        noteArray[5][searchAddr] = noteStepEnd >> 8;
        noteArray[6][searchAddr] = noteStepEnd & 0xFF;      
      }
    
      searchAddr = noteArray[0][searchAddr];  // go to next address. 
    } while(currentStep != 32767);        
  }
  else if(noteStepStart < 0) // user does not know start point of Note
  {
    int currentStep;
    do 
    {   
      currentStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr]; 
      if(currentStep == 32767){break;}
      
      int searchPitch = noteArray[3][searchAddr];
      int noteStepEnding = (noteArray[5][searchAddr] << 8) + noteArray[6][searchAddr]; 
//      Serial.print(" currentStep ");Serial.print(currentStep);
//      Serial.print("  searchPitch ");Serial.print(searchPitch);
//      Serial.print("  noteStepEnding ");Serial.println(noteStepEnding);
      if(searchPitch == pitch && noteStepEnding == 32765) // Found a pitch event with no registered off note.  Use this.
      {
        noteArray[5][searchAddr] = noteStepEnd >> 8;
        noteArray[6][searchAddr] = noteStepEnd & 0xFF;   
        currentStep == 32767;  
        break;         
      }
    
      searchAddr = noteArray[0][searchAddr];  // go to next address. 
    } while(currentStep < noteStepEnd || currentStep != 32767);    
  }
  else // Find noteStepStart
  {
    int currentStep;
    do
    {   
      currentStep = (noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr]; 
      int searchPitch = noteArray[3][searchAddr];
      if(currentStep == noteStepStart && searchPitch == pitch)
      {
        noteArray[5][searchAddr] = noteStepEnd >> 8;
        noteArray[6][searchAddr] = noteStepEnd & 0xFF;  
        currentStep = 32767;
        break;
      }
      if(currentStep == 32767){break;}
      searchAddr = noteArray[0][searchAddr];  // go to next address. 
    } while(currentStep < noteStepEnd || currentStep != 32767);    
  }
}

void removeEvent(int targetStep, int range, byte pitch)  
{
  int targetStepEnd = targetStep + range;
  int currentAddr = 0; 
  int prevAddr = 0;
  currentAddr = noteArray[0][0]; // get first note.
  int currentStep;
  int count = 0;
  
  do // find event
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    if(currentStep == 32767 || count > 255){break;}   

    int nextEventAddress = noteArray[0][currentAddr];  // get next address. 
    int searchPitch = noteArray[3][currentAddr];
    
    if(searchPitch == pitch)
    {
      if(currentStep >= targetStep && currentStep < targetStepEnd) // have found a note on in this range
      {
        // Remove and mark as removed
//    Serial.print(" currentAddr: ");Serial.print(currentAddr);
//    Serial.print(" currentStep: ");Serial.print(currentStep);
//    Serial.print(" nextEventAddress: ");Serial.println(nextEventAddress);
        noteArray[0][prevAddr] = nextEventAddress;   // redirect previous to next address
        noteArray[0][currentAddr] = 255;                              // reset values
        noteArray[3][currentAddr] = 0;                                // reset values
        Serial.print("RemoveStep: "); Serial.println(currentStep); 
      }
      else if (currentStep < targetStep)
      {
        int noteEndStep = (noteArray[5][currentAddr] << 8) + noteArray[6][currentAddr]; 
        if(noteEndStep >= targetStep)
        {
          // Trim note   
          int newEndStep = targetStep - 1;
          noteArray[5][currentAddr] = newEndStep >> 8;
          noteArray[6][currentAddr] = newEndStep & 0xFF;
          Serial.print("TrimStep:  "); Serial.print(newEndStep); 
        }
      }
      else if (currentStep >= targetStepEnd)
      {
        currentStep = 32767; break;
      }
    }  
    prevAddr = currentAddr;  // Location to redirect
    currentAddr = nextEventAddress;  // get next address. 
    count ++;
  } while(currentStep < targetStepEnd || currentStep != 32767);    
}
   


void initialiseNoteArray()
{
  //Serial.print(" initialiseNoteArray: ");
  noteArray[0][0] = 1;  // nextAddr
  noteArray[1][0] = 0;  // stepNum byte 1
  noteArray[2][0] = 0;  // stepNum byte 2
  noteArray[3][0] = 0;  // pitch
  noteArray[4][0] = 0;  // vel
  noteArray[5][0] = 0;  // Off Step byte 1
  noteArray[6][0] = 0;  // Off Step byte 2
    
  noteArray[0][1] = 0;  // nextAddr
  int s = 32767;  // wrong: 10 mins of playing at 140bpm and 8 stepsPerBeat. 
  byte byte1 = s >> 8; byte byte2 = s & 0xFF;  
  noteArray[1][1] = byte1; // stepNum byte 1 
  noteArray[2][1] = byte2; // stepNum byte 2  
  noteArray[3][1] = 0;  // pitch
  noteArray[4][1] = 0;  // vel  
  noteArray[5][1] = 0;  // Off Step byte 1
  noteArray[6][1] = 0;  // Off Step byte 2 
    
  newAddr = 2;            //  Reset address.

  for(int i=2; i< 256; i++) {noteArray[0][i] = 255;} // setto 255 so makes array size smaller (can reuse)
}

int getLoopLocation(int loop)
{
  // 255 events * 6 bytes an event = 1530
  // 128 byte Page * 12 = 1536
  // 8 channels to record @ 256 bytes each = 2048;
  // Total is 1536 + 2048 =  3584;
  // 512k mem = 64,000 / 3584 = 17.85
  // LOOP MEM:  |128 Settings | 1536 (12*128) Events | 2048 (16*128) Knobs
  // One Loop = 29*128 = 3712Bytes
  // MAIN MEM: | 128 SYS Setting | 896 (7*128) Song | LOOPS
  // Therefore Loop 0 starts at 1024
  
  int loopBaseAddr = 1024 + (loop * 3712);  
  return loopBaseAddr;
}

void saveTheLoop(int butt)
{
  const int sizeEvent = 6;
  const int sizeNumEvent = 255;
  const int sizeMemBlock = 128;

  loopNum = butt;
  
  int baseAddr = getLoopLocation(loopNum);
  int noteAddr = baseAddr + 128;

  uint8_t p[13];     
  p[0]= 69; // checksum..
  p[1]=bpm;
  p[2]=stepsPerLED;
  p[3]=stepsInLoop >> 8;
  p[4]=stepsInLoop & 0xFF;
  if(stepStart > stepEnd || stepStart < 0){stepStart = 0;}  
  p[5]=stepStart >> 8;
  p[6]=stepStart & 0xFF;  
  if(stepEnd > stepsInLoop || stepEnd < stepStart){stepEnd = stepsInLoop;}  
  p[7]=stepEnd >> 8;
  p[8]=stepEnd & 0xFF;   
  p[9]=0;  
  p[10]=winYNoteStart;   
  writeEEPROM(baseAddr,p,11);  
  
  byte searchAddr = noteArray[0][0];

  byte buff[1536];
  int index = 0;
  for(int i=0; i<1536; i++)
  {
    buff[index]   = noteArray[1][searchAddr];                  // get step byte 1
    buff[index+1] = noteArray[2][searchAddr];                  // get step byte 2
    buff[index+2] = noteArray[3][searchAddr];    
    buff[index+3] = noteArray[4][searchAddr];  
    buff[index+4] = noteArray[5][searchAddr];    
    buff[index+5] = noteArray[6][searchAddr]; 
    index += 6;
    int s = (noteArray[1][searchAddr] << 8) +  noteArray[2][searchAddr]; 
    if(s>32766){break;}  
    searchAddr = noteArray[0][searchAddr];   // get next address
  }

  int count = 0;
  for(int i=0; i<12; i++)
  {
    byte buf[128];
    int c=0;
    for(int i=0; i<128 && count<index; i++)
    {
      buf[i]= buff[count];
      Serial.print(buf[i]);Serial.print(" "); if(i%6 == 0){Serial.println();}
      count ++; c=i;
    }
    writeEEPROM(noteAddr,buf,c);
    noteAddr = noteAddr + 128; 
    Serial.print("Count ");Serial.println(count);
    if(count>=index){break;} 
  }
  
  Serial.print("SavedLoop: ");Serial.println(loopNum);
  keyboardRefresh(0);
}


void loadTheLoop(int butt)
{  
  loopNum = butt;
  initialiseNoteArray();

  int baseAddr = getLoopLocation(loopNum);
  int noteAddr = baseAddr + 128;
  
  uint8_t b[12];
  readEEPROM(baseAddr,b,11);

  int checkSum = b[0];
  if(checkSum == 69)
  {
    Serial.print("validLoop: ");Serial.println(loopNum);
    bpm = b[1];  
    stepsPerLED = b[2];  
    stepsInLoop = (b[3] << 8) + b[4]; 
    stepStart = (b[5] << 8) + b[6];   
    stepEnd = (b[7] << 8) + b[8];    
//    winXStepStart = b[9];  
    winYNoteStart = b[10];   
//    Serial.println(bpm);
//    Serial.println(notesPerBeat);
//    Serial.println(stepsInLoop);
//    Serial.println(stepStart);
//    Serial.println(stepEnd);
//    Serial.println(winXStepStart);
//    Serial.println(winYNoteStart);

    for(int i=0; i<256; i++)
    {
      unsigned int s, blocks;
      uint8_t buf[31];
      readEEPROM(noteAddr,buf,30);
      for(int g=0; g<5; g++)
      {
        blocks = g;
        int a = g*6;
        Serial.print(buf[a]);Serial.print(" ");
        Serial.print(buf[a+1]);Serial.print(" ");
        Serial.print(buf[a+2]);Serial.print(" ");
        Serial.print(buf[a+3]);Serial.print(" ");
        Serial.print(buf[a+4]);Serial.print(" ");
        Serial.print(buf[a+5]);Serial.println(" ");  
        s = (buf[a] << 8) + buf[a+1]; 
        if(s > 32766){break;}  
        byte p = buf[a+2];      
        byte v = buf[a+3];  
        byte noteEnd = (buf[a+4] << 8) + buf[a+5]; 
        insertEvent(s, p, v, noteEnd);           
      }
      if(s>32766){break;} 
      noteAddr = noteAddr + 30; 
    } 
  }
  else
  {
    Serial.println("noValidLoop");
  }
  nextEvent(0);
  ledRefreshScreen();
  keyboardRefresh(0);
}


byte findNote(int targetStep, int range, byte pitch)  
{
  int targetStepEnd = targetStep + range;
  int currentAddr = 0;  
  currentAddr = noteArray[0][0]; // get first note.
  int currentStep;
  byte state = 0;
  
  do // find event
  {   
    currentStep = (noteArray[1][currentAddr] << 8) + noteArray[2][currentAddr]; 
    int searchPitch = noteArray[3][currentAddr];
    
    if(searchPitch == pitch)
    {
      if(currentStep >= targetStep && currentStep < targetStepEnd) // have found a note on in this range
      {
        state = 1;  // note in location
        break;
      }
      else if (currentStep < targetStep)
      {
        int noteEndStep = (noteArray[5][currentAddr] << 8) + noteArray[6][currentAddr]; 
        if(noteEndStep >= targetStep)
        {
          state = 1;  // overlap / long note
          break;
        }
      }
      else if (currentStep >= targetStepEnd)
      {
        break;
      }
    }
    if(currentStep == 32767){break;}
    currentAddr = noteArray[0][currentAddr];  // get next address. 
  } while(currentStep < targetStepEnd || currentStep != 32767);    

  return state;
}

void saveSong(int value)
{
// MEMORY
// SYSTEM DATA 0-127
// SONG DATA 128-1023 (7*128 songs)
// Bytes 0-songSteps(124) are step data. 125=checksum, 126=stepStart, 127 stepEnd

  if(value > 7){value = 7;}
  Serial.print(" SaveSong: ");Serial.println(value);
  int songMemData = 128 + (value * 128);
  byte b[128];                 
  for(int i=0; i<songSteps; i++){b[i] = songArray[i];}
  b[125] = 69;
  b[126] = songStartStep;
  b[127] = songEndStep;        

  writeEEPROM(songMemData,b,128);                 // Read Song Block   
  songCurrent = value;
}

void loadSong(int value)
{
  if(value > 7){value = 7;}if(value < 0){value = 0;}
  Serial.print(" LoadingSong: ");Serial.println(value);
  
  int songMemData = 128 + (value * 128);
  byte b[128];                 
  readEEPROM(songMemData,b,128);                 // Read Song Block
  if(b[125] == 69)   // valid data
  {
    Serial.println(" ValidSong!");
    for(int i=0; i<125; i++)
    {
      if(b[i] < nRows){songArray[i] = b[i];}
      else{songArray[i] = 0;} // not valid
    }
    songStartStep = b[126];
    songEndStep = b[127];    
  }
  else
  {
     Serial.println(" No ValidSong..");
    for(int i=0; i<125; i++){songArray[i] = b[0];}
    songStartStep = b[0];
    songEndStep = b[4];       
  }   
}

void loadDefaultValues()
{
  delay(6000);
  // Check to see if touch screen available - if not disable touch screen and led.
  Wire.beginTransmission(extMem);
  byte memAvailable = Wire.endTransmission();
  if(memAvailable != 0){Serial.print(" No Memory..");}
  else
  {
    uint8_t b[16];
    b[0] = 68;
    b[1] = channelOut;  
    b[2] = channelIn;  
    b[3] = 132;  
    b[5] = 130; 
    writeEEPROM(globalMemStart,b, 7); 
    for(int i=0; i<7; i++){Serial.print(b[i]);Serial.print(" ");} Serial.println(" ");
    for(int y=0; y<12; y++){b[y] = 0;}    // chord stuff 
    writeEEPROM(globalMemStart+10,b, 12);  
    // Sequence Array
    for(int i=0; i<16; i++){b[i] = 0;} 
    writeEEPROM(seqMemStart+254,b,1); 
    writeEEPROM(seqMemStart,b,16);      
  }
}

void loadSavedValues()
{
  unsigned long wireWriteLast = millis() - writeTimeLast;
  if(wireWriteLast <= 4){int dTime = 4-wireWriteLast; delay(dTime);}
  Wire.beginTransmission(extMem);
  byte memAvailable = Wire.endTransmission();
  if(memAvailable != 0){Serial.print(" No Memory..");}
  uint8_t b[13];
  readEEPROM(globalMemStart,b,9);
  channelOut = b[1];
  channelIn = b[2];

  phaseAmount = b[5] - 128;
  
//  for(int i=0; i<7; i++){Serial.print(b[i]);Serial.print(" ");} Serial.println(" ");
//  Serial.print(channelOut);Serial.print(" ");
//  Serial.print(channelIn);Serial.print(" ");
//  Serial.print(phaseAmount);Serial.print(" ");
  // Chord Array
  readEEPROM(globalMemStart + 10,b,12);  // load chord array from memory
  for(int x=0;x<7;x++) {int n = 20+(x*12); for(int y=0; y<12; y++){chordArray[n+y] = b[y];}}    

  byte red=0, green=0, blue=0;  
  red = 255*startStop; blue = 255*clkSync; leds[0] = CRGB(0,red,blue);
  leds[1] = CRGB(0,0,0);
  leds[2] = CRGB(0,0,0);  
  red = (stepsPerLED*32)-1; blue = (256/stepsPerLED)-1; leds[3] = CRGB(0,red,blue);
  leds[4] = CRGB(0,255,0);
  leds[5] = CRGB(0,0,0);
  leds[6] = CRGB(0,0,0);
  leds[7] = CRGB(0,0,0);
  leds[8] = CRGB(0,0,0);
  leds[9] = CRGB(0,0,0);
  leds[10] = CRGB(0,0,0);
  leds[11] = CRGB(0,0,0);
  leds[12] = CRGB(0,0,0);
  leds[13] = CRGB(0,0,0);  
  leds[14] = CRGB(0,0,0);   
  keyboardRefresh(0); 
  FastLED.show();
}
  
void readEEPROM(unsigned int memAddr, byte* buffer, int length)
{
  Wire.beginTransmission(extMem);

  Wire.write((int)(memAddr >> 8)); // MSB
  Wire.write((int)(memAddr & 0xFF)); // LSB
  Wire.endTransmission(false);

  Wire.requestFrom(extMem, length);
  //for(byte i=0; i<length && Wire.available(); i++) { buffer[i]=Wire.read(); }
  for (byte i = 0; i < length; i++) 
  {
    while (!Wire.available()) { /* Wait for data to become available */ }
    buffer[i] = Wire.read();
  }
}

void writeEEPROM(unsigned int memAddr, byte* buffer, int length) 
{
  unsigned long timeLast = millis() - writeTimeLast;
  if(timeLast < 4){int dTime = 4-timeLast; delay(dTime);}
  Wire.beginTransmission(extMem);
  Wire.write((int)(memAddr >> 8));   // MSB
  Wire.write((int)(memAddr & 0xFF)); // LSB
  Wire.write(buffer, length);
  Wire.endTransmission(true);
  writeTimeLast = millis();
  //delay(4);
}
