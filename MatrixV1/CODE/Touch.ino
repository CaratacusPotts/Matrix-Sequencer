void touchAction(int xPos, int yPos)
{
  int xMoved = xPos - xPosLast; 
  int yMoved = yPos - yPosLast;
  xPosLast = xPos;
  yPosLast = yPos; 
   
  unsigned long timeNow = millis();
  unsigned long timePassed = timeNow - touchActionLast;
  touchActionLast = timeNow;

  switch (touchEventType)
  {
    case 0: // last event was the first event in this series.
      {touchEventType = 1;}
    break;
    case 1: // last event was in the same place.
    {
      if(yMoved != 0 && xMoved == 0) // swipe or draw in Y direction
        {touchEventType = 2; ledScrollScreen(yMoved);}
      if(xMoved != 0 && yMoved == 0) // swipe or draw in X direction
        {touchEventType = 4; swipeNoteTieStart(xPosLast, yPosLast);}
    }
    break;
    case 2:   // swipe in Y direction 
    {
      if(yMoved != 0)   {ledScrollScreen(yMoved);}    
    }
    break;         
    case 3:   // Draw Mode
    {
      if(xMoved != 0 || yMoved !=0)   {drawVelocity(xPos, yPos);}     // swipe or draw in X direction 
    }
    break; 
    // 4 = Tie Note (nothing needed here.                                  
  }
 
  Serial.print("\ttouchEventType:"); Serial.println(touchEventType); 
}


void checkTouch()
{
  unsigned long timeNow = millis();
  unsigned long wireWriteLast = timeNow - writeTimeLast;
  if(wireWriteLast <= 4){int dTime = 4-wireWriteLast; delay(dTime);}

  bool fingerOff = false;
  if(touchEventType != 0){fingerOff = true;}
  
  int pins[] = {RDY1pin, RDY2pin, RDY3pin, RDY4pin};
  for(int r=0; r<nGrids; r++)
  { 
    bool pin = digitalRead(pins[r]);  
    if(pin)
    {
      byte touchArr = 112 + r;                                  // IQ5500 = 0x74 stock address = 116 dec Arry1 = 70(112), Arry 2= 71(113), Arry3 = 72(114), Arry 4= 73(115)
      
      byte x1 = azoteqRead(touchArr,0x16);
      byte x2 = azoteqRead(touchArr,0x17);
      byte y1 = azoteqRead(touchArr,0x18);
      byte y2 = azoteqRead(touchArr,0x19);      
      azoteqStopRead(touchArr);
      
      int xPos = (x1 << 8) + x2;                                // Combine the two position bytes
      int yPos = (y1 << 8) + y2;                                // Combine the two position bytes
      
      if(xPos < 768 && yPos < 1024)
      {
        fingerOff = false;
        //Serial.print(" Time: ");Serial.print(millis());Serial.print("\tX: ");Serial.print(xPos);Serial.print("  Y: ");Serial.print(yPos); 
        xPos = xPos/97;
        if(yPos < 10){yPos = 12;}
        else{yPos = 12-((yPos/86)+1);} 
        Serial.print(" \tX: ");Serial.print(xPos);Serial.print("  Y: ");Serial.print(yPos); 
        xPos = xPos + (r*8); // if grid 2, and x = 7, then x = 15.

        touchAction(xPos, yPos);                     // check what type of action it is
      }
    }
  }
  
  if(fingerOff)
  {
    unsigned long timeNow = millis();
    unsigned long timePassed = timeNow - touchActionLast;
    unsigned long swipeTimePassed = timeNow - touchSwipeLast;
    if(timePassed > 60 && swipeTimePassed > 200) // allow for finger skips and buffer from swipe
    {   
      switch(touchEventType)
      {
        case 1:
        {
          Serial.println(" \t finger off!!!");
          if(editSettings != 0) // user is editing settings
          {
            settingChange(xPosLast, yPosLast);
          }
          else    // standard note press
          {
            processTouch(xPosLast, yPosLast); 
            ledRefreshScreen();
          }
        }
        break;
        case 4:
        {
          tieNotePSE[2] = ((winXStepStart + xPosLast + 1) * stepsPerLED) - 1;    
          if(tieNotePSE[2] < tieNotePSE[1]){tieNotePSE[2]= tieNotePSE[1] + (stepsPerLED -1);} // so you don't get a backwards tie..
          tieNotes();    
          ledRefreshScreen();          
        }
        break;
      }
      if(touchEventType != 3){touchEventType = 0;}  
    }
  }           
}

void processTouch(byte xPos, byte yPos)
{  
  if(songMode)
  {
    songArray[xPos] = yPos;      
  }
  else  // is in loop Mode
  {   
    byte pitch = winYNoteStart + yPos;                                // Windowed Notes.  Which step count is on the left (typcially step 0)
    int onStep = (winXStepStart + xPos) * stepsPerLED;           // *2 for actual steps.  Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
    Serial.print("On Step: ");Serial.print(onStep);
    int offStep = onStep + stepsPerLED - 1;                    // stepsPerLED is how many steps a single LED represents.  ie 1LED and stepView = 2 is 2 steps)     
    Serial.print("  Off Step: ");Serial.println(offStep);
    if(offStep <= onStep){offStep = onStep+1;}

    byte newState = findNote(onStep,stepsPerLED, pitch);                       // Invert last state  
    if(newState == 0)                                                     // Send note state to noteWrite.  The *100 is to give nominal velocity.   1=touch input - add off note after (non-tied note)  
    { 
      insertEvent(onStep, pitch, 100, offStep); 
      transposeNoteTonic = 127;  // reset, as a new note will not get get noticed on the transpose without this reset.
    }
    else if (newState == 1)
    { 
      removeEvent(onStep, stepsPerLED, pitch); 
      if(startStop){midiSendNoteOff(pitch, 0, channelOut);}              // so you don't get a stuck note  
    }                                                              
  }
}

void swipeNoteTieStart(int xPosStart, int yPosStart)
{
  int startStep = ((winXStepStart + xPosStart) * stepsPerLED)-1;
  if(startStep < 0){startStep = 0;}
  tieNotePSE[1] = startStep;           // *2 for actual steps.  Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)  
  tieNotePSE[0] = yPosStart + winYNoteStart;       
}


void drawVelocity(int xPos, int yPos)
{
  drawVel = (yPos & 0xFF) * 10;               // x10 = to get to 127.  ie: row 6*10 =60 velocity.  row 12=120vel
  if(drawVel >127){drawVel=127;}
  drawVelStep = (winXStepStart + xPos) * stepsPerLED; 
  ledShowNoteVelocity(1);
}


byte azoteqRead(byte grid, byte addr)
{
  Wire.beginTransmission(grid);            
  Wire.write(byte(0x00)); Wire.write(addr);           // Address 0x000D = touch type (single,press&hold,swipe -x,+x,+y,-y)
  Wire.endTransmission(false);                              // stop transmitting    
  Wire.requestFrom(grid,1,(uint8_t)true);               // Request the byte I asked for    
  return Wire.read();  
}

void azoteqStopRead(byte grid)
{
  Wire.beginTransmission(grid);
  Wire.write(byte(0xEE)); Wire.write(byte(0xEE));           // Datasheet 0xEEEE, followed by a STOP        
  Wire.endTransmission(true);                               // stop transmitting  
}
