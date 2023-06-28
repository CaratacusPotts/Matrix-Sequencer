  
void updateLEDs()                            // Shift out LED data to LED's                                    
{ 
  int xAxis = 0; 
  int yAxis = 0;
  bitSet(yAxis,ledRow);

  if(ledRow < nRows)  // do notes
  {
    for(int x=0; x<nCols; x++)                                       // Sweeping LED's horizontally  col, by col.
    {
      xAxis = xAxis | (ledArray[ledRow][x] << x);   
      if(startStop)                                             // Flash if step is here (to show step movement across panel)  
      {
        int stepPos = stepCount/stepsPerLED;
        if(stepPos < nCols){bitWrite(xAxis,stepPos,0);}
      }            
    }          
  }
  else    // do background (dimed leds)
  {  
    if(songMode)
    {
      yAxis = 0xFFFF;
      int sS = songStartStep - songWindowStart; 
      int sE = songEndStep - songWindowStart;
      if(sS < nCols)        {bitSet(xAxis,sS);}
      if(sE < nCols)        {bitSet(xAxis,sE);}
      if(songStep < nCols)  {bitSet(xAxis,songStep);}   
    } 
    else if(editSettings != 0) // user editing Settings
    {
      xAxis = settingsXAxis;
      yAxis = settingsYAxis;
    }
    else
    {
      for(int y=0; y<13; y++){bitWrite(yAxis,y,keyArray[winYNoteStart + y]);}
      xAxis = ledSteps;       
    } 
    ledTurnOffKeys = micros() + ledBlankTime;  
  }
  digitalWrite(latchPin, LOW);                                  // Start the shift out sequence  x-axis first, y-axis last        
  
  shiftOut(dataPin, clockPin, MSBFIRST, (xAxis>>8)); 
  shiftOut(dataPin, clockPin, MSBFIRST, xAxis);     
  shiftOut(dataPin, clockPin, MSBFIRST, (yAxis >> 8));          // MSb first because: Fills LSB from Top/Left array. So want LSB to be last on the data train..
  shiftOut(dataPin, clockPin, MSBFIRST, yAxis);   
//  for(int i=nCols; i>0; i--)
//  {
//    digitalWrite(dataPin, bitRead(xAxis,i-1));
//    digitalWrite(clockPin, HIGH);
//    digitalWrite(clockPin, LOW);  
//  }
//  for(int i=15; i>-1; i--)
//  {
//    digitalWrite(dataPin, bitRead(yAxis,i));
//    digitalWrite(clockPin, HIGH);
//    digitalWrite(clockPin, LOW);  
//  }  
  digitalWrite(latchPin, HIGH);                                 // End the shift out sequence       
  digitalWrite(LEDBlank, LOW);   

  if(ledRow == nRows){ledRow = 0;} 
  else{ledRow ++;}  
  //delayMicroseconds(100);
}

void ledClearArray()
{
  for(int x=0; x<nCols; x++)
  {
    for(int y=0; y<nRows; y++){ledArray[y][x] = 0;}
  }
}

void ledRefreshScreen()
{
  //Serial.println("refresh");
  ledClearArray();
  if(songMode){ledDrawSong();} else{ledDrawNotes();}
}

void ledDrawNotes()
{
  int firstStep = winXStepStart * stepsPerLED;                             // Windowed Notes.  Which step count is on the left (typcially step 0)
  int lastStep = firstStep + (nCols * stepsPerLED)-1;                  // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
  int lowPitch = winYNoteStart;                                            // Windowed Notes.  Which step count is on the Right 32 cols - therefore xStart + 32
  int highPitch = winYNoteStart + 12;                                      // Windowed Notes.  Which Note pitch is on the y axis rows - therefore yStart + 14
  
  byte searchAddr = 0; 
  
  for(int i=0;i<255;i++)
  {
    int searchStep = ((noteArray[1][searchAddr] << 8) + noteArray[2][searchAddr]);                              //  actual step number.       
    byte searchPitch = noteArray[3][searchAddr];                    
    int noteEndStep = ((noteArray[5][searchAddr] << 8) + noteArray[6][searchAddr]);        //  end step number.  
    
    searchAddr = noteArray[0][searchAddr]; 
   
    if(searchStep > 32766 || searchStep > lastStep) {break;}               // Check to see if any notes are still 'on' before exiting
    
    if(searchPitch >= lowPitch && searchPitch <= highPitch)           //  if pitch is in window range - log or do something
    {     
      if(searchStep < firstStep)
      {
        if(noteEndStep >= firstStep)
        {
          int ledStart = 0;
          int ledEnd = (((noteEndStep - firstStep)/stepsPerLED)+1) + ledStart;
          if(ledEnd > nCols){ledEnd = nCols;}   
          int yPos = searchPitch - lowPitch;
          for(int x=ledStart; x<ledEnd; x++){ledArray[yPos][x] = 1;}
        }
      }
      else if(searchStep >= firstStep && searchStep <= lastStep) // have found a note on in this range
      {
        int ledStart = (searchStep - firstStep)/stepsPerLED;
        int ledEnd = ((noteEndStep - firstStep)/stepsPerLED)+1;   
        Serial.print("\t");Serial.print(ledStart);Serial.print("\t");Serial.println(ledEnd);
        if(ledEnd > nCols){ledEnd = nCols;}   
        int yPos = searchPitch - lowPitch; if(yPos < 0){yPos=0;}if(yPos >= nRows){yPos=nRows-1;}
        for(int x=ledStart; x<ledEnd; x++){ledArray[yPos][x] = 1;}
      }
    }  
  }    
  // NOw do the key lines in the background 
  int s = ((stepStart/stepsPerLED) - winXStepStart);  if(s<0){s= 0;} 
  int e = ((stepEnd/stepsPerLED) - winXStepStart)+1;  if(e>nCols){e=nCols;}  
  ledSteps = 0;
  if(s<nCols){for(int y=s; y<e; y++) {bitSet(ledSteps,y);} }
}

void ledDrawSong()
{
  for(int x=0; x<nCols; x++)                                                              // Load Array with seqencer plot
  {
    byte loopNum = songArray[x];
    if(loopNum < 14){ledArray[loopNum][x] = 1;}                                     // If valid loop number (<128)
    else{ledArray[0][x] = 0;}                                                        // Otherwise put a blank in there
  } 

}

void ledScrollScreen(int sign)
{        
  Serial.print("Swipe Up/Down: "); Serial.println(sign);
  winYNoteStart = winYNoteStart - sign; 
  if(winYNoteStart < 21){winYNoteStart = 21;}                               // Make sure does not go out of bounds
  if(winYNoteStart > 95){winYNoteStart = 95;}                             // Make sure does not go out of bounds  

  touchSwipeLast = millis();
  ledRefreshScreen();         
}

void ledPanLeftRight(int sign)
{        
  if(sign == 0){sign = -1;} 
  int panAmount = sign * (nCols/4);
  winXStepStart = winXStepStart - panAmount;
  if(winXStepStart < 0){winXStepStart = 0;}                               // Make sure does not go out of bounds
  ledRefreshScreen();  
  Serial.print("winXStepStart: "); Serial.println(winXStepStart);       
}


void ledShowNoteVelocity(byte inputVal)
{
  ledClearArray();
  int firstStep = winXStepStart * stepsPerLED;                                       // Windowed Notes.  Which step count is on the left (typcially step 0)
  int lastStep = firstStep + (nCols * stepsPerLED);                                 // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)

  // Check to see if note is there
  byte searchAddr = 0; 
  for(int i=0;i<255;i++)
  {
    byte byte1 = noteArray[1][searchAddr]; 
    byte byte2 = noteArray[2][searchAddr]; 
    unsigned int searchStep = ((byte1 << 8) + byte2);         //  actual step number.                       
    byte searchVelocity = noteArray[4][searchAddr]; 
    
    if(searchStep >= firstStep && searchStep <= lastStep)
    {
      if(inputVal == 1 && searchStep == drawVelStep)                      // the velocity has been updated by the draw function
      {
        noteArray[4][searchAddr] = drawVel;                            // Replace Velocity
        searchVelocity = noteArray[4][searchAddr];                             //velocityShow[s];
      }    
      if(searchVelocity > 0) // draw the velocity histogram
      {
        int h = (searchVelocity / 10) + 1; 
        for(int y=0; y<13; y++)
        {
          unsigned int x = (searchStep - firstStep) / stepsPerLED; 
          if(y<h){ledArray[y][x] = 1;}  // create bar
          else{ledArray[y][x] = 0;}      // blank the resst
        }
      }
    }
    if(searchStep > lastStep || searchStep > 32766)  {break;}
    searchAddr = noteArray[0][searchAddr];   
  }    
}

void ledShowMidiChan()   
{
  ledClearArray();
  settingsXAxis = 0;
  settingsYAxis = 0;
  
//  // show name
//  unsigned int c[4] = {12,8,3,8};
//  int xStart = 3;
//  int yStart = 3;
//  for(int z=0; z<4;z++) 
//  {
//    for(int i=0; i<3;i++) 
//    {
//      for(int j=0; j<4; j++)    
//      {
//        ledArray[yStart - j][xStart] = bitRead(alphabet[c[z]],(i*5)+j);
//      }
//      xStart ++;
//      if(c[z]==8){i=3;}
//    }
//    xStart ++;
//  }
  byte chanIn = channelIn; 
  byte chanOut = channelOut;  
 
  // do channel in
  if(chanIn > 9) // draw a 1
  {
    byte n = (float)(chanIn / 10);
    chanIn = chanIn - (n*10);
    for(int y=0; y<5; y++){ledArray[y + 7][2] = 1;}  
  }
  for(int y=0; y<5; y++)
  {         
    for(int b=0; b<3; b++) {ledArray[y + 7][6 - b] = bitRead(numSmall[chanIn][y],b);}
  } 
  
  // do channel out     
  if(chanOut > 9) // draw a 1
  {
    byte n = (float)(chanOut / 10);
    chanOut = chanOut - (n*10);
    for(int y=0; y<5; y++){ledArray[y + 7][10] = 1;}
  }  
  for(int y=0; y<5; y++)
  {         
    for(int b=0; b<3; b++) {ledArray[y + 7][14 - b] = bitRead(numSmall[chanOut][y],b);}
  }  
}

void ledShowConductorChan()
{
  ledClearArray();
  settingsYAxis = 0;
  bitWrite(settingsYAxis,0,1);bitWrite(settingsYAxis,1,1);bitWrite(settingsYAxis,2,1);bitWrite(settingsYAxis,3,1);bitWrite(settingsYAxis,5,1);
  settingsXAxis = 0xFFFF;

// Show Conductor Matrix.
  for(int x=0; x<16; x++)
  {
    ledArray[0][x] = bitRead(conductorOut[x][0],0);
    ledArray[1][x] = bitRead(conductorOut[x][0],1);
    ledArray[2][x] = bitRead(conductorOut[x][0],2);
    ledArray[3][x] = bitRead(conductorOut[x][0],3);
    ledArray[5][x] = bitRead(conductorOut[x][0],5);
  } 

// Show Shift Number  
  int midiChan = conductorMidiOutEdit;
  int n = 0;
  if(midiChan > -1)  // User has adjusted matrix after startup
  {
    n = conductorOut[midiChan-1][1];
    if(n < 0)
    {
      n = n * -1; 
      ledArray[9][6] = 1; ledArray[9][7] = 1; // draw negative sign
    }
    if(n > 9) // draw Most Significant Number
    {
      byte nn = (float)(n / 10);
      n = n - (nn*10);
      for(int y=0; y<5; y++)
      {  
        for(int b=0; b<3; b++)
        {
          ledArray[y+7][10-b] = bitRead(numSmall[nn][y],b);
        }
      } 
    }   // draw Least Significant Number
    for(int y=0; y<5; y++)
    {         
      for(int b=0; b<3; b++) {ledArray[y+7][14 - b] = bitRead(numSmall[n][y],b);}
    } 
  }  
}

void ledShowStepsPerBeat()
{ 
  // stepsPerLedArray[] = {2,3,4,6,8,12,24,48,96};    // 1/12,1/8,1/6,1/4,1/3,1/2,1,2,1,2,4
  
  for(int y=7; y<13; y++)  {for(int x=8; x<16; x++){ledArray[y][x] = 0;}} // clear 

  int npBArray[] = {12,8,6,4,3,2,1,2,1,2,4};
  int pos = 0;
  for(int i=0; i<9; i++) {if(stepsPerLedArray[i] == stepsPerLED){pos = i;}}
  int digit1 = 0, digit2 = 0;
  if(pos<6)
  {
    if(pos == 0)  {digit2 = 1;digit1 = 2;} // is  1/12
    else          {digit1 = npBArray[pos];}
    for(int y=0; y<5; y++)  // do digit 2
    {         
      for(int b=0; b<3; b++)
      {
        ledArray[y + 8][11-b] = bitRead(numSmall[digit2][y],b);
      }
    }      
  }
  else
    {digit1 = npBArray[pos];}
  
  for(int y=0; y<5; y++)  // do second digit
  {         
    for(int b=0; b<3; b++)
    {
      ledArray[y + 8][15-b] = bitRead(numSmall[digit1][y],b);
    }
  }  
  settingsYAxis = 0;
  for(int y=0; y<8; y++){bitWrite(settingsYAxis,y,keyArray[winYNoteStart + y]);}
  ledSteps = 
  settingsXAxis = ledSteps;
}
    
//void ledShowScaleNotes()
//{
//  ledClearArray();
//  settingsYAxis = 0;
//  for(int y=0; y<13; y++){bitWrite(settingsYAxis,y,keyArray[winYNoteStart + y]);}
//  settingsXAxis = ledSteps;
//
//  for(int y=0; y<13; y++){ledArray[y][0] = chordArray[winYNoteStart+y];}    
//}

void ledShowCurrentLoop()
{
  ledClearArray();  
  ledArray[loopNum][0] = 1;                                               // Mark current loop
}

void ledShowPhaseAmount()
{
  settingsYAxis = 0;
  for(int y=0; y<8; y++){bitWrite(settingsYAxis,y,keyArray[winYNoteStart + y]);}
  settingsXAxis = ledSteps;
  
  int p = phaseAmount;
  ledArray[10][8] = 0; ledArray[10][7] = 0;
  if(p < 0)   // if a negative - draw sign   
    {
      p = p * -1;
      int loc = 3;
      ledArray[10][8] = 1;ledArray[10][7] = 1; // draw line
    } 
  //Serial.print("p: ");Serial.println(p);
  if(p > 99){p = 99; phaseAmount = 99;}  // make sure in bounds
  if(p > 9)
  {
    byte n = (float)(p / 10);  // n = most significant.  27  n = 2
    for(int y=0; y<5; y++){for(int b=0; b<3; b++){ledArray[y + 8][11-b] = bitRead(numSmall[n][y],b);}}
    p = p - (n*10);            // p = remainedr.  27 p = 7
    for(int y=0; y<5; y++){for(int b=0; b<3; b++){ledArray[y + 8][15-b] = bitRead(numSmall[p][y],b);}}
  }
  else
  {
    for(int y=0; y<5; y++){for(int b=0; b<3; b++){ledArray[y + 8][11-b] = bitRead(numSmall[0][y],b);}}
    for(int y=0; y<5; y++){for(int b=0; b<3; b++){ledArray[y + 8][15-b] = bitRead(numSmall[p][y],b);}}
  }
}


void ledShowBPM()                  
{    
  settingsYAxis = 0;
  settingsXAxis = 0;
  
  ledClearArray();

  byte inputVal = bpm;
    
  if(inputVal > 99)
  {
    byte n = (float)(inputVal / 100);
    inputVal = inputVal - (n*100);
    for(int b=5; b<10; b++) {ledArray[b][6] = 1;}
  }
  if(inputVal > 9)
  {
    byte n = (float)(inputVal / 10);
    inputVal = inputVal - (n*10);
    for(int y=0; y<5; y++) {for(int b=0; b<3; b++)  {ledArray[y+5][10-b] = bitRead(numSmall[n][y],b);} } 
  }  
  byte n = inputVal;
  for(int y=0; y<5; y++){ for(int b=0; b<3; b++)    {ledArray[y+5][14-b] = bitRead(numSmall[n][y],b);} } 
}

void ledShowInvertLine(int on)
{
  Serial.print(" invertState: ");Serial.println(on);
  if(on == 1)
  {
    if(canonInvertAxis == 0){canonInvertFindMiddle();}
    Serial.print(" canonInvertAxis: ");Serial.println(canonInvertAxis);
    if(canonInvertAxis < winYNoteStart || canonInvertAxis > winYNoteStart + nRows) // make sure you can see it
      {winYNoteStart = canonInvertAxis - (nRows / 2);}
    if(winYNoteStart < 20){winYNoteStart = 20;}
    Serial.print(" winYNoteStart: ");Serial.println(winYNoteStart);
    ledRefreshScreen();
    int  y = canonInvertAxis - winYNoteStart;
    Serial.print("y: ");Serial.println(y);
    for(int x=0; x<nCols; x++){ledArray[y][x]=1;}  
  }
  else
  {
    transposeCopy(); 
    ledRefreshScreen();
  }
}

void keyboardRefresh(byte state)
{
  byte loop[] = {0,2,4,5,7,9,11};
  byte patch[] = {1,3,6,8,10};
  
  if(state == 0) // go default
  {
    if(loopMode){state = 1;}
    else if(songMode){state = 2;}
    else if(keyboardActive){state = 3;}
    else if(conductor){state = 4;}
  }
  
  switch(state)
  {
    case 1:  // Loop Mode
    {
      for(int i=15; i<ledNum; i++) {leds[i] = CRGB(0,0,0);}
      byte r = 255, b=0;
      if(mute){b = 255;}
      leds[15+loopNum] = CRGB(0,r,b);
    }
    break;
    case 11:  // Long Press In Loop Mode
    {
      for(int i=15; i<ledNum; i++) {leds[i] = CRGB(0,0,0);}
      leds[15+buttHoldFlag] = CRGB(0,0,255);
    }
    break;      
    case 2:  // Song Mode
    {
     // for(int i=15; i<ledNum; i++) {leds[i] = CRGB(0,0,0);}
      //leds[15+loop[loopNum]] = CRGB(0,255,0);
    }
    break;    
    case 3: // Keyboard Mode
    {
      for(int i=0; i<7; i++) {leds[15+loop[i]] = CRGB(100,100,100);}   
      for(int i=0; i<5; i++) {leds[15+patch[i]] = CRGB(0,0,255);}      
    }
    break;    
    case 4: // Conductor Mode
    {
      leds[15] = CRGB(0,255,0);
      leds[16] = CRGB(0,200,50);
      leds[17] = CRGB(0,150,100);
      leds[18] = CRGB(0,100,150);
      leds[19] = CRGB(0,50,200);
      leds[20] = CRGB(0,50,200);
      leds[21] = CRGB(0,100,150);
      leds[22] = CRGB(0,150,100);
      leds[23] = CRGB(0,200,50);
      leds[24] = CRGB(0,255,0);
      leds[25] = CRGB(0,0,0);
      leds[26] = CRGB(0,0,0);            
    }
    break;  
    case 5: // Transpose - Show Scale Notes
    {
      for(byte i=0; i<12; i++) 
      {
        Serial.print(chordArray[i+48]);Serial.print(",");
        if(chordArray[48+i] == 1){leds[i+15] = CRGB(0,100,255);}
        else{leds[i+15] = CRGB(0,0,0);}
      }    
    }
    break;         
  }
  FastLED.show();    
}
