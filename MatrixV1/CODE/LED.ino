  
void updateLEDs(byte ledType)                            // Shift out LED data to LED's                                    
{ 
  uint32_t xAxis = 0; 
  uint16_t yAxis = 0;
  bitSet(yAxis,ledRow);
  
  if(ledRow < 14)
  {
    switch (ledType)
    {
      case 0:     // When not a temporary display of numbers, it displays the note pattern
      {    
        for(int x=0; x<32; x++)                                       // Sweeping LED's horizontally  col, by col.
        {
          if(ledRow == 13){xAxis = xAxis | (ledArray[ledRow][x] << x);}
          else
          {
            xAxis = xAxis | (ledNoteArray[ledRow][x] << x);   
            if(startStop)                                             // Flash if step is here (to show step movement across panel)  
            {
              int stepPos = stepCount/(24 / notesPerBeat);
              bitWrite(xAxis,stepPos,0);
            }         
          }       
        }
      }
      break;
      case 1:      // If type one (using LED array as crude screen to diplay info, on action)
      { 
        for(int x=0; x<32; x++)                                       // Sweeping LED's horizontally  col, by col.
        {xAxis = xAxis | (ledArray[ledRow][x] << x);}
      }
      break;             
      case 2:      // Sequencer Mode
      { 
        for(int x=0; x<32; x++)                                       // Sweeping LED's horizontally  col, by col.
        {xAxis = xAxis | (ledArray[ledRow][x] << x);}
      }
      break; 
      case 3:     // For canon.  So you can see keys without notes.
      { 
        for(int x=0; x<32; x++)                                       // Sweeping LED's horizontally  col, by col.
        {xAxis = xAxis | (ledArray[ledRow][x] << x);}
      }
      break;   
      case 4:      // If type one (using LED array as crude screen to diplay info, on action)
      { 
        for(int x=0; x<32; x++)                                       // Sweeping LED's horizontally  col, by col.
        {xAxis = xAxis | (ledArray[ledRow][x] << x);}
      }
      break;                      
    }       
  }
  else
  {  
    const byte keyArray[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0,  1,0,1,0,0,1,0,1,0,1,0,0};
    switch (ledType)
    {    
      case 0:     // Loop Mode
      { 
        for(int y=0; y<13; y++){bitWrite(yAxis,y,keyArray[winYNoteStart + y]);}
        xAxis = ledSteps;  
        //if(seqMode){bitWrite(xAxis,seqStep, 0);}  // if viewing notes in seq mode.  Show seq step position         
      }
      break;
      case 1:     // When not a temporary display of numbers, it displays the note pattern
      { 
            
      }
      break;
      case 2:     // Seq Mode
      { 
        yAxis = 0xFFFF;
        xAxis = 0; 
        int sE = seqArray[254] - 1;
        if(sE>32){sE=31;}
        bitSet(xAxis,sE);  bitSet(xAxis,seqStep);           
      }
      break;  
      case 3:     // For canon.  So you can see keys without notes.
      { 
        for(int y=0; y<13; y++){bitWrite(yAxis,y,keyArray[winYNoteStart + y]);}
        xAxis = ledSteps;            
      }
      break;    
      case 4:     // For Composer Mode.  So you can see chord notes without rest of notes.
      { 
        bitWrite(yAxis,0,1);bitWrite(yAxis,1,1);bitWrite(yAxis,2,1);
        xAxis = 0xFFFF0000;          
      }
      break;                     
    }   
    ledTurnOffKeys = micros() + 350;  
  }
  digitalWrite(latchPin, LOW);                                  // Start the shift out sequence  x-axis first, y-axis last        
  shiftOut(dataPin, clockPin, MSBFIRST, (xAxis>>24));           // MSB first because: Is first carrage on data train down y then along x axis, stopping bottom right of array
  shiftOut(dataPin, clockPin, MSBFIRST, (xAxis>>16)); 
  shiftOut(dataPin, clockPin, MSBFIRST, (xAxis>>8)); 
  shiftOut(dataPin, clockPin, MSBFIRST, xAxis); 
         
  shiftOut(dataPin, clockPin, MSBFIRST, (yAxis >> 8));          // MSb first because: Fills LSB from Top/Left array. So want LSB to be last on the data train..
  shiftOut(dataPin, clockPin, MSBFIRST, yAxis);   
  
  digitalWrite(latchPin, HIGH);                                 // End the shift out sequence       
  digitalWrite(A4, LOW);   

  ledRow ++; if(ledRow > 14){ledRow = 0;}  
}


void ledRefresh()
{
  if(ledScreen)
  {
    ledArray[13][0] = startStop; 
    ledArray[13][1] = 0; 
    ledArray[13][2] = mute;
    ledArray[13][3] = 0; 
    ledArray[13][4] = recordOvertop; 
    ledArray[13][5] = 0; 
    ledArray[13][6] = loopLoad; 
    ledArray[13][7] = 0; 
    ledArray[13][8] = 0;
    ledArray[13][9] = seqMode;
    ledArray[13][10] = 0;
    ledArray[13][11] = noteVelo;
    ledArray[13][12] = 0;
    ledArray[13][13] = 0;
    ledArray[13][14] = 0;
    ledArray[13][15] = stepViewActive;
    ledArray[13][16] = setStartEnd;
    ledArray[13][17] = 0;          
    ledArray[13][18] = transposeActive; 
    ledArray[13][19] = 0; 
    ledArray[13][20] = phaseActive; 
    ledArray[13][21] = 0; 
    ledArray[13][22] = canonActive;    //dualMode;
    ledArray[13][23] = 0; 
    ledArray[13][24] = 0; 
    ledArray[13][25] = clkSync; 
    ledArray[13][26] = 0; 
    ledArray[13][27] = saveLoop;     
    ledArray[13][28] = 0; 
    ledArray[13][29] = settings;     
    ledArray[13][30] = 0; 
    ledArray[13][31] = clearNotes; 
  }
}


void ledRefreshNotes()
{
  if(!recording && ledScreen)
  {     
    for(int x=0; x<32; x++){ for(int y=0; y<13; y++){ledNoteArray[y][x] = 0;} }  // clear LedNoteArray

    ledDrawNotes(32);
  }
}

void ledDrawNotes(int ledWidth)
{
    int stepsPerLed = 24 / notesPerBeat; 
    int firstStep = winXStepStart * stepsPerLed;                             // Windowed Notes.  Which step count is on the left (typcially step 0)
    int lastStep = firstStep + (ledWidth * stepsPerLed) -1;                           // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
    int lowPitch = winYNoteStart;                                            // Windowed Notes.  Which step count is on the Right 32 cols - therefore xStart + 32
    int highPitch = winYNoteStart + 12;                                      // Windowed Notes.  Which Note pitch is on the y axis rows - therefore yStart + 14
    int maxRange = ledWidth - 1;
    int minRange = 0;
    
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
        if (searchStep < firstStep)
        {
          if(noteEndStep >= firstStep)
          {
            int ledStart = minRange;
            int ledEnd = (((noteEndStep - firstStep)/stepsPerLed)+1) + ledStart;
            if(ledEnd >  maxRange){ledEnd = maxRange ;}   
            int yPos = searchPitch - lowPitch;
            for(int x=ledStart; x<ledEnd; x++){ledNoteArray[yPos][x] = 1;}
          }
        }
        else if(searchStep >= firstStep && searchStep <= lastStep) // have found a note on in this range
        {
          int ledStart = ((searchStep - firstStep)/stepsPerLed) + minRange ;
          int ledEnd = (((noteEndStep - firstStep)/stepsPerLed)+1) + minRange;   
          if(ledEnd > maxRange){ledEnd = maxRange ;}   
          int yPos = searchPitch - lowPitch; 
          for(int x=ledStart; x<ledEnd; x++){ledNoteArray[yPos][x] = 1;}
        }
      }  
    }
    
    // NOw do the key lines in the background 
    int s = ((stepStart/stepsPerLed) - winXStepStart) + minRange;  if(s< minRange){s= minRange;} 
    int e = ((stepEnd/stepsPerLed) - winXStepStart) + minRange;  if(e>maxRange){e=maxRange;}  
    ledSteps = 0;
    for(int y=s; y<=e; y++) {bitSet(ledSteps,y);}
  
}


void ledResetStateDefault()
{
  noteVelo = false;
  ledDisplayMode = 0; 
  ledRefresh();  
}

void ledDisplay(byte ledType, byte inputVal)                  
{    
    const byte num[][10]=                                         // Lookup Number Array.  Row = Number you want to display
    {                                                             // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
    {62,65,65,65,65,65,65,62},                                    // 0    
    {1,1,1,1,1,1,1,1},                                            // 1
    {63,64,64,64,62,1,1,126},                                     // 2
    {126,1,1,1,62,1,1,126},                                       // 3 etc
    {8,8,8,62,72,64,64,64},
    {126,1,1,1,126,64,64,127},
    {62,65,65,65,126,64,64,62},
    {1,1,1,1,1,1,1,126},
    {62,65,65,65,62,65,65,62},
    {1,1,1,1,62,65,65,62}
    };

    const byte numSmall[][6]=     // Lookup Number Array.  Row = Number you want to display
      {               // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
        {7,5,5,5,7},  // 0    
        {1,1,1,1,1},  // 1
        {7,4,7,1,7},  // 2
        {7,1,3,1,7},  // 3 etc
        {2,2,7,6,4},
        {7,1,7,4,7},
        {7,5,7,4,4},
        {1,1,1,1,7},
        {7,5,7,5,7},
        {1,1,7,5,7}
      };  

    const byte iconSmall[][6]=     // Lookup Number Array.  Row = Number you want to display
      {               // !!!! Numbers are (yAxis) upsidedwon, as the Notes will be coming im bottom up.  Just makes the notes eaiser..
        {9,6,6,9,0},  // X 
        {12,10,9,10,12},  // Out Arrow
        {9,5,3,5,9}  // In Arrow
      }; 
      
  switch (ledType)
  {   
    case 1:   // BPM = Number to show.
    {
      for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   // Clear Array 
    
      byte xOffset = 8;                                             // Where in the xAxis to Start the numbers (from Left Side)
      byte yOffset = 3;                                             // Where in the xAxis to Start the numbers (from Top)
      if(inputVal > 99)
      {
        byte n = (float)(inputVal / 100);
        inputVal = inputVal - (n*100);
        for(int y=0; y<8; y++)
        {  
          for(int b=0; b<8; b++)
          {
            int x = (xOffset + 7) - b;
            ledArray[y + yOffset][x] = bitRead(num[n][y],b);
          }
        }
      }
      if(inputVal > 9)
      {
        byte n = (float)(inputVal / 10);
        inputVal = inputVal - (n*10);
        for(int y=0; y<8; y++)
        {  
          for(int b=0; b<8; b++)
          {
            int x = (xOffset + 15) - b;
            ledArray[y + yOffset][x] = bitRead(num[n][y],b);
          }
        } 
      }  
      byte n = inputVal;
      for(int y=0; y<8; y++)
      {         
        for(int b=0; b<8; b++)
        {
          int x = (xOffset + 23) - b;
          ledArray[y + yOffset][x] = bitRead(num[n][y],b);
        }
      } 
    }
    break;     
    case 2:   // Steps Per Beat = Number to show.
    {
      for(int x=16; x<32; x++){for(int y=0; y<13; y++){ledNoteArray[y][x] = 0;}}   // Clear Array 
      
      byte xOffset = 8;                                             // Where in the xAxis to Start the numbers (from Left Side)
      byte yOffset = 3;                                             // Where in the xAxis to Start the numbers (from Top)

      if(inputVal > 9)
      {
        byte n = (float)(inputVal / 10);
        inputVal = inputVal - (n*10);
        for(int y=0; y<8; y++)
        {  
          for(int b=0; b<8; b++)
          {
            int x = (xOffset + 15) - b;
            ledNoteArray[y + yOffset][x] = bitRead(num[n][y],b);
          }
        } 
      }  
      byte n = inputVal;
      for(int y=0; y<8; y++)
      {         
        for(int b=0; b<8; b++)
        {
          int x = (xOffset + 23) - b;
          ledNoteArray[y + yOffset][x] = bitRead(num[n][y],b);
        }
      }  
    }
    break; 
    case 3:   // Midi Channel Numbers
    {
      for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   // Clear Array 

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
        for(int y=0; y<5; y++){ledArray[y + 7][25] = 1;}
      }  
      for(int y=0; y<5; y++)
      {         
        for(int b=0; b<3; b++) {ledArray[y + 7][29 - b] = bitRead(numSmall[chanOut][y],b);}
      }  

    // Show Conductor Matrix.
      for(int x=16; x<32; x++)
      {
        byte b = conductorOut[x-16][0];
        if(b > 0){ledArray[b-1][x]=1;}
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
          ledArray[2][6] = 1; ledArray[2][7] = 1; // draw negative sign
        }
        if(n > 9) // draw Most Significant Number
        {
          byte nn = (float)(n / 10);
          n = n - (nn*10);
          for(int y=0; y<5; y++)
          {  
            for(int b=0; b<3; b++)
            {
              ledArray[y][10-b] = bitRead(numSmall[nn][y],b);
            }
          } 
        }   // draw Least Significant Number
        for(int y=0; y<5; y++)
        {         
          for(int b=0; b<3; b++) {ledArray[y][14 - b] = bitRead(numSmall[n][y],b);}
        } 
      }
    }
    break;               
    case 5:   // Display Velocity
    {  
      int stepsPerLed = 24 / notesPerBeat; 
      int firstStep = winXStepStart * stepsPerLed;                                       // Windowed Notes.  Which step count is on the left (typcially step 0)
      int lastStep = firstStep + (32 * stepsPerLed);                                 // Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)

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
              unsigned int x = (searchStep - firstStep) / stepsPerLed; 
              if(y<h){ledArray[y][x] = 1;}  // create bar
              else{ledArray[y][x] = 0;}      // blank the resst
            }
          }
        }
        if(searchStep > lastStep || searchStep > 32766)  {break;}
        searchAddr = noteArray[0][searchAddr];   
      }    
    }
    break;  
    case 6:   // Phase Amount
    {
      for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   // Clear Array 

      byte xOffset = 8;
      byte yOffset = 3;                                             // Where in the xAxis to Start the numbers (from Top)
      
      int p = phaseAmount;
      if(p < 0)   // if a negative - draw sign   
        {
          p = p * -1;
          int loc = 3;
          for(int i=0; i<5; i++){ledArray[6][i+loc] = 1;}
        } 
      //Serial.print("p: ");Serial.println(p);
      if(p > 99){p = 99; phaseAmount = 99;}  // make sure in bounds
      if(p < -99){p = -99; phaseAmount = -99;}  // make sure in bounds
      if(p > 9)
      {
        byte n = (float)(p / 10);  // n = most significant.  27  n = 2
        p = p - (n*10);            // p = remainedr.  27 p = 7
        for(int y=0; y<8; y++){for(int b=0; b<8; b++){ledArray[y + yOffset][7] = bitRead(num[n][y],b);}}
      }
      for(int y=0; y<8; y++){for(int b=0; b<8; b++){ledArray[y + yOffset][15-b] = bitRead(num[p][y],b);}}
    }
    break;  
    case 7:  // Conductor Matrix
    {
    // Clear Array 
      for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   
    // Show Note Array
      for(int y=0; y<13; y++){ledArray[y][0] = chordArray[winYNoteStart+y];}   
    }
  }   
}
