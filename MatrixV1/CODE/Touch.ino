void checkTouch()
{
  unsigned long timeNow = millis();
  unsigned long wireWriteLast = timeNow - writeTimeLast;
  if(wireWriteLast <= 4){int dTime = 4-wireWriteLast; delay(dTime);}
  
  int pins[] = {RDY1pin, RDY2pin, RDY3pin, RDY4pin};
  for(int r=0; r<4; r++)
  { 
    bool pin = digitalRead(pins[r]);  
    if(pin)
    {
      byte touchArr = 112 + r;                                  // IQ5500 = 0x74 stock address = 116 dec Arry1 = 70(112), Arry 2= 71(113), Arry3 = 72(114), Arry 4= 73(115)
      Wire.beginTransmission(touchArr);            
     
      Wire.write(byte(0x00)); Wire.write(byte(0x0D));           // Address 0x000D = touch type (single,press&hold,swipe -x,+x,+y,-y)
      Wire.endTransmission(false);                              // stop transmitting    
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the byte I asked for    
      byte touchType1 = Wire.read();
 
      Wire.beginTransmission(touchArr);      
      Wire.write(byte(0x00)); Wire.write(byte(0x11));           // Address 0x000E = touch type (2 finger, scroll, zoom)
      Wire.endTransmission(false);                              // stop transmitting  
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the 6 bytes I asked for    
      byte touchType2 = Wire.read();
  
      Wire.beginTransmission(touchArr);
      Wire.write(byte(0x00)); Wire.write(byte(0x16));           // Address 0x0016 = Absolute X pos (byte 1)
      Wire.endTransmission(false);                              // stop transmitting  
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the byte I asked for    
      byte x1 = Wire.read();
  
      Wire.beginTransmission(touchArr);
      Wire.write(byte(0x00)); Wire.write(byte(0x17));           // Address 0x0017 = Absolute X pos (byte 2)
      Wire.endTransmission(false);                              // stop transmitting  
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the byte I asked for    
      byte x2 = Wire.read();
  
      Wire.beginTransmission(touchArr);
      Wire.write(byte(0x00)); Wire.write(byte(0x18));           // Address 0x0016 = Absolute y pos (byte 1)
      Wire.endTransmission(false);                              // stop transmitting  
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the byte I asked for    
      byte y1 = Wire.read();
  
      Wire.beginTransmission(touchArr);
      Wire.write(byte(0x00)); Wire.write(byte(0x19));           // Address 0x0017 = Absolute y pos (byte 2)         
      Wire.endTransmission(false);                              // stop transmitting  
      Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the byte I asked for    
      byte y2 = Wire.read();

//        Wire.beginTransmission(touchArr);
//        Wire.write(byte(0x00)); Wire.write(byte(0x12));           // Address 0x0014 = Relative x pos (byte 1)
//        Wire.endTransmission(false);                              // stop transmitting  
//        Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the 6 bytes I asked for    
//        byte xR1 = Wire.read();
//    
//        Wire.beginTransmission(touchArr);
//        Wire.write(byte(0x00)); Wire.write(byte(0x13));           // Address 0x0015 = Relative x pos (byte 2)         
//        Wire.endTransmission(false);                              // stop transmitting  
//        Wire.requestFrom(touchArr,1,(uint8_t)true);               // Request the 6 bytes I asked for    
//        byte xR2 = Wire.read();
      
      Wire.beginTransmission(touchArr);
      Wire.write(byte(0xEE)); Wire.write(byte(0xEE));           // Datasheet 0xEEEE, followed by a STOP        
      Wire.endTransmission(true);                               // stop transmitting
  
      int xPos = (x1 << 8) + x2;                                // Combine the two position bytes
      int yPos = (y1 << 8) + y2;                                // Combine the two position bytes

//      int xRel = (xR1 << 8) + xR2;                                // Combine the two position bytes
//      //int xRel = (~xRel) ^ 1;                                // Combine the two position bytes  
//Serial.print(" xRel = ");Serial.println(xRel,BIN);

      if(xPos < 768 || yPos < 1024)
      {
        float x = xPos;
        x = (x/768)*7;                                          // Find closest button: (x Res is? 768)   8 because 8 cols   
        xPos = int( x + 0.5);                                   // rounds it      
        float y = yPos + 35;                                    // the + num is an offset to better place the touch relative to the LED (a tweak)
        y = (y/1024)*14;                                        // Find closest button: (y Res is? 1024)  15 because 15 rows
        yPos = int( y + 0.5);                                   // rounds it
        yPos = (0-yPos)+14;                                     // flips y axis so row 15 is top   

        xPos = xPos + (r*8);                                    // If reading from pad 2, x=0 will be 0 + 8*1 = 8
 
        //Serial.print("      xPos: "); Serial.print(xPos); Serial.print("  yPos: "); Serial.println(yPos); 

        int xMoved = xPos - buttonLastArray[0];
        int yMoved = yPos - buttonLastArray[1];
        

        if(touchType1 == 1)
        {
          unsigned long t = timeNow - swipeLast;
          if(t > 200) // Just making sure is not an accidental tap from a swipe movement.
          {
            Serial.print("TAP"); Serial.println(tapActive);
            if(yPos == 13) {tapActive = 0; processTouch(xPos, yPos);}
            else if(tapActive > 0) {processTap(tapActive, xPos, yPos);}
            else if(swipeActive == 0 && drawActive == 0) {processTouch(xPos, yPos);}
            tapLast = timeNow;
            buttonLastArray[3] = 1;  // register a tap happend
          }
          
        }  
        else if(touchType2 == 2 && (yMoved == 1 || yMoved == -1)) {zoomScreen(yMoved);swipeLast = timeNow; buttonLastArray[3] = 4;}  // register a tap happend}    
        else 
        {        
          if(buttonLastArray[3] == 0) // first part of touch is registered.  Only if you move to another cell can it be concidered a swipe/drag
          {
            int x = abs(xMoved); 
            int y = abs(yMoved);    
              
            if(x < 3 && y < 3)
            {
              if(x != 0 || y != 0)
              {  
                unsigned long s = timeNow - swipeStart[2];
                if(s > 500) {swipeStart[0] = buttonLastArray[0];swipeStart[1] = buttonLastArray[1];}  // is a new swipe - grap previous position as start point.
                swipeStart[2] = buttonLastArray[2];;
                swipeLast = timeNow;
                if(drawActive > 0){Serial.print("Draw: "); Serial.println(drawActive); processDraw(drawActive, xPos, yPos);}
                else
                {
                  Serial.print("Swipe"); Serial.println(swipeActive);
                  processSwipe(swipeActive, xMoved, yMoved, xPos,yPos);
                }
              } 
            }
          }
          buttonLastArray[3] = 0; // finger is held down
        }
        buttonLastArray[0] = xPos; buttonLastArray[1] = yPos; buttonLastArray[2] = timeNow; 
      }
    }
  }  

  // Time OUT stuff (user leaves screen too long ******************
  
  unsigned long timePast = timeNow - buttonLastArray[2];               //  find last button press
  
  if(tapActive > 0)
  {
    switch (tapActive) 
    {      
      case 6: // Loop Button  Waiting for user to enter patch   
       {  if(timePast > 2000){tapActive = 0; ledDisplayMode = 0; ledRefresh();} }
      break;     
      case 27: // Save Button    
      {
        if(timePast > 2000)
        {
          tapActive = 0; 
          saveLoop = false;
          if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;} 
          ledRefresh();
        } 
      }
      break;            
    }
  }      
  if(swipeActive > 0)
  {
    switch (swipeActive) 
    {      
      case 1: // Horizontal Swipe - Tie Notes.  
      {
        if(timePast > 250) // no touch activity for the last (500)ms, turn off and process
        {
          swipeActive = 0;
          ledRefreshNotes();                                 
        }                
      }
      break;      
      case 15: // Steps Per Beat 
      { 
        if(timePast >8000)
        {
          stepViewActive = false;
          swipeActive = 0; 
          ledDisplayMode = 0; ledRefresh();ledRefreshNotes();
        } 
      }
      break;                     
    }
  }      
  if(drawActive > 0)
  {
    switch (drawActive) 
    {      
      case 1: // Change Velocity of Single Note 
        { 
          if(timePast > 750)
          {
            drawActive = 0; 
            if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;} 
          } 
        }
      break;  
      case 2: // Scrolling screen up or down
        { 
          if(timePast > 150)
          {
            drawActive = 0; 
            buttonLastArray[3] = 1;  // register a tap happend to clear swipe.
            if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;} 
          } 
        }
      break;
      case 4: // Tie Notes
        { 
          if(timePast > 150)
          {
            drawActive = 0; 
            buttonLastArray[3] = 1;  // register a tap happend to clear swipe.
            
            byte xOffset = 0;

            int stepsPerLed = 24 / notesPerBeat; 
            int relXPos = buttonLastArray[0] - xOffset;
            tieNotePSE[2] = ((winXStepStart + relXPos + 1) * stepsPerLed) - 1;    
            if(tieNotePSE[2] < tieNotePSE[1]){tieNotePSE[2]= tieNotePSE[1] + (stepsPerLed -1);} // so you don't get a backwards tie..
            tieNotes();
            if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;} 
          } 
        }
      break;        
      case 3: // Scrolling screen up or down
        { 
          if(timePast > 150)
          {
            drawActive = 0; 
            if(seqMode){ledDisplayMode = 2;} else{ledDisplayMode = 0;} 
          } 
        }        
      break;            
      case 16: // StepStart Draw
        { if(timePast > 1200){drawActive = 0; setStartEnd = false; ledRefresh();} }
      break;   
      case 22: // Canon Transpose
        { if(timePast > 150){drawActive = 0; swipeActive = 23; ledRefresh();} }
      break;     
      case 23: // Canon Phase
        { if(timePast > 150){drawActive = 0; swipeActive = 23; ledRefresh();} }
      break;              
    }
  }
  
    
 // *****************************************************************                
}

void processTouch(byte xPos, byte yPos)
{  
  if(yPos < 13)                                                      // Is a Note (13 = note rows, touchtype 1 = bit 1 (single tap event)
  { 
    if(seqMode)
    {
      ledArray[seqArray[xPos]][xPos] = 0;
      seqArray[xPos] = yPos;  
      ledArray[yPos][xPos] = 1;       
    }
    else  // is in loop Mode
    {   
      int stepsPerLed = 24 / notesPerBeat; 
      byte pitch = winYNoteStart + yPos;                                // Windowed Notes.  Which step count is on the left (typcially step 0)
      int onStep = (winXStepStart + xPos) * stepsPerLed;           // *2 for actual steps.  Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)
      int offStep = onStep + stepsPerLed - 1;                    // stepsPerLED is how many steps a single LED represents.  ie 1LED and stepView = 2 is 2 steps)     
      if(offStep <= onStep){offStep = onStep+1;}

      byte newState = findNote(onStep,stepsPerLed, pitch);                       // Invert last state  
      if(newState == 0)                                                     // Send note state to noteWrite.  The *100 is to give nominal velocity.   1=touch input - add off note after (non-tied note)  
        { 
          insertEvent(onStep, pitch, 100, offStep); 
          transposeNoteTonic = 127;  // reset, as a new note will not get get noticed on the transpose without this reset.
        }
      else if (newState == 1)
        { 
          removeEvent(onStep, stepsPerLed, pitch); 
          if(startStop){midiSendNoteOff(pitch, 0, channelOut);}              // so you don't get a stuck note  
        }                                                              
      ledRefreshNotes(); 
    }
  }

  
  if(yPos > 12)                                                        // Is Button Along Top, and a button push.
  {
    switch (xPos) 
    {      
      case 0: // Play Button   
      {
        if(settings)
        {
          channelMod = true;
          ledDisplay(3, channelOut); 
          ledDisplayMode = 4; 
          swipeActive = 0; tapActive = 1;
        }   
        else
        {
          startStop = !startStop;                                       // toggle state
          if(startStop && clkSync){loopStartFlag = true;}  
          else if(startStop && !clkSync){ loopStart(); }               
          else{ loopStop(); } 
          settings = false; // good back to normal.  
          if(seqMode){ledDisplayMode = 2;}else{ledDisplayMode = 0;}
          ledRefresh();          
        }
      }
      break;  
      case 2: // Mute Button   
      {
        mute = !mute;                                // toggle state                           
        ledRefresh();
      }
      break;         
      case 4: // Record Over top
      {
        long t = millis() - tapLast;
        if(t>500) // debounce to 0.5 sec.
        {
          recOvertop();  
          //Serial.print("Record Over Top  State: ");Serial.println(recordOvertop);                 
          ledRefresh(); 
        }
      }
      break; 
      case 6: // Loop Button    
      {  
        if(settings)
        {
        }
        else
        {
          if(seqMode || !loopLoad)
          {
            seqMode = false;  
            noteVelo = false;    // we know that by whatever action this must be turned off if it was on before.
            ledDisplayMode = 0;  
          }
          else
          {                   
            for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   // Clear Array 
            ledArray[loopNum][0] = 1;                                               // Mark current loop
            ledDisplayMode = 1;                                                     // Screen to show Notes.  
            tapActive = 6;    
          } 
          loopLoad = true;  
          ledRefresh();      
        }
      }
      break;                 
      case 9: // Pattern/Seq Button    
      {
        if(loopLoad)
        {
          loopLoad = false; seqMode = true;          
          noteVelo = false;                                 // we know that by whatever action this must be turned off if it was on before.
          uint8_t b[1];                          
          readEEPROM(seqMemStart+254,b,1);                 // Find the last step in the saved sequence 
          seqArray[0] = b[0];
          if(seqArray[254] <254)
            {
              uint8_t b[32]; 
              readEEPROM(seqMemStart,b,32);
              for(int x=0; x<32; x++) { if(b[x] < 14){seqArray[x] = b[x];}  }
            }          
          else if(seqArray[254] > 254)    // In case of memory error.
          {
            byte b[1]; 
            b[0] = 0;
            writeEEPROM(seqMemStart+254,b,1);   
            seqArray[254] = 0;
          }        
          // load into led array.
          for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}               // Clear Array 
          for(int x=0; x<32; x++)                                                              // Load Array with seqencer plot
          {
            byte loopNum = seqArray[x];
            if(loopNum < 14){ledArray[loopNum][x] = 1;}                                     // If valid loop number (<128)
            else{ledArray[0][x] = 0;}                                                        // Otherwise put a blank in there
          }       
          seqStep = 0; loopLoadActive = true;                                                // Load up loop to start playing. 
          ledDisplayMode = 2;                                                                // Tell LED refresh to load seq plot
          ledRefresh();
        }
      }
      break;         
      case 11: // Note/Velocity Button   
      {
        noteVelo = !noteVelo;
        if(seqMode)
        { 
        }
        else
        {
          if(noteVelo)
          {
            ledDisplay(5, 0); // send zero to indicate no change to velocity.
            ledDisplayMode = 3; // clears screen keeps the note/key lanes (ledDisplayMode 1 gets rid of lanes)
            drawActive = 11;
          }
          else
          {
            drawActive = 0;
            ledDisplayMode = 0; 
            ledRefreshNotes(); ledRefresh();
          }
        }
        ledRefresh();      
      }
      break;  
      case 15: // notesPerBeat Button    
      {
        if(stepViewActive)
        {
          stepViewActive = false;
          swipeActive = 0; 
          ledDisplayMode = 0; ledRefresh();ledRefreshNotes();
        }
        else
        {
          stepViewActive = true;
          swipeActive = 15;        // If the next touch event is a finger drag go to this function          
          ledDisplay(2, notesPerBeat);
        }
      } 
      break;                                             
      case 16: // StepStart Button    
      {
        setStartEnd = true;
        ledRefresh();  
        if(seqMode) {ledDisplayMode = 2;}                                       
        else {}
        drawActive = 16;        // If the next touch event is a finger drag go to this function            
      } 
      break;  
      case 18: // transposeActive
      {
        if(settings)
        {
          chordEdit = true;
          swipeActive = 0; tapActive = 18;  // go to tapProcess section to enter chord notes    
          ledDisplayMode = 3;  
          ledDisplay(7, 0);      
        } 
        else
        { 
          transposeState();
        }
      }
      break;   
      case 20: // Phasing Active
      {
        if(settings)
        {
          phaseMod = true;
          ledDisplay(6, 0); 
          ledDisplayMode = 1; 
          swipeActive = 20;  
        }        
        else if(!phaseActive) {phaseActive = true;}   // is off
        else if (phaseActive) {phaseActive = false;}  // in on already
        ledRefresh();
      }
      break;   
      case 22: // Canon Active Button (finger to move notes X or Y
      {
        if(canonActive)
        {
          canonActive = false;
          swipeActive = 0; 
          drawActive = 0;
        }
        else
        {
          canonActive = true;
          swipeActive = 22;
          ledDisplayMode = 0; 
        } 
        ledRefresh();             // So you can see the button has been pressed 
      }            
      break;   
      case 25: // BPM / SYNC  Button    
      {      
        if(settings)
        {   
        }
        else
        {
          long t = millis() - tapLast;
          if(buttonLastArray[0] == xPos && buttonLastArray[1] == yPos && t<1000) // same button pushed 
            {
              if(!clkSync)  // User was in Non-CLKsync mode.  Is double tapping to go into CLKsync
              {
                clkSync = true; ledDisplayMode = 0; ledRefresh(); swipeActive = 0;
              }
              else  // User was in CLKsync mode.  Is double tapping to go to internal CLK
              {
                clkSync = false; ledDisplayMode = 0; ledRefresh(); swipeActive = 0;
              }    
            }
          else if(!clkSync && swipeActive == 0)
            {ledDisplay(1, bpm); ledDisplayMode = 1; swipeActive = 25; } 
          else if (!clkSync)
          {
            swipeActive = 0; //clkSync = false;
            if(seqMode){ledDisplayMode = 2;} 
              else{ledDisplayMode = 0;} 
            ledRefresh();      
          }
        }
      } 
      break;        
      case 27: // Save Button    
      {
        if(saveLoop)  // user hits save twice
        {
          saveLoop = false;
          tapActive = 0;
          ledDisplayMode = 0; 
          ledRefresh();                            
        }
        else
        {
          if(seqMode)
          {
            byte b[1]; 
            for(int y=0;y<254;y++)
            {
              b[0] = seqArray[y];
              writeEEPROM(seqMemStart+y,b,1); 
            }
            b[0] = seqArray[254];
            writeEEPROM(seqMemStart+254,b,1);  // save the loop end point.
          }
          else if(loopLoad) // loop mode active
          {
            for(int x=0; x<32; x++){for(int y=0; y<13; y++){ledArray[y][x] = 0;}}   // Clear Array 
            ledArray[13][27] = 1;                                                   // Show the save select as active          
            ledArray[loopNum][0] = 1;                                              // Place led of current pattern.           
            saveLoop = true;
            ledDisplayMode = 1;                                                     // Screen to show active selection        
            ledRefresh(); 
            tapActive = 27;
          }
        }  
      }   
      break;       
      case 29: // Settings
      {
        if(settings)
        {
          settings = false;  
          if(channelMod)
          {
            channelMod = false;
            tapActive = 0;
            byte b[2]; 
            b[0] = channelOut; b[1] = channelIn;
            writeEEPROM(globalMemStart+1,b,2);       
            
            conductor = false;
            for(int i=0; i<16; i++){if(conductorOut[i][0] > 0){conductor = true;}}
            conductorMidiOutEdit = -1;  // resets for next time.          
          }
          if(phaseMod)
          {
            phaseMod = false;
            swipeActive = 0; 
            byte b[1]; 
            b[0] = phaseAmount + 128;   //Serial.print("phaseAmount[0]: ");Serial.print(b[0]);
            writeEEPROM(globalMemStart+5,b,1);               
          }   
          if(chordEdit)
          {
            chordEdit = false;
            tapActive = 0;
            swipeActive = 0;
            drawActive = 0;
            transposeSaveChord();  // save to memory   
          }   
          ledDisplayMode = 0; 
        }     
        else {settings = true;}
        
        ledRefresh();          
      }
      break;  
      case 31: //  Clear Button
      {
        if(clearNotes)
        {
          clearNotes = false;
        }
        else
        {
          if(seqMode)
          {
          }
          else
          { 
            initialiseNoteArray(); 
            for(int y=0;y<13;y++){for(int x=0;x<32;x++){ledNoteArray[y][x] = 0;}}
            notesOffAll();
            clearNotes = false;
          }
        }
        ledRefresh();          
      }
      break;                   
    }
  }
}

void processTap(int item, int xPos, int yPos)
{
  switch (item)
  {
    case 1:
    {
      if(xPos < 8 && yPos > 4)  // User adjusting input Channel  
      {
        if(yPos < 9) // down
        {
          int c = channelIn - 1;
          if(c< 0){channelIn = 0;}  else{channelIn = c;}      
        }
        else  // up
        {
          channelIn ++;
          if(channelIn >16){channelIn = 16;}    
        }
        Serial.print(" ChanIn: ");Serial.println(channelIn);
      }
      if(xPos > 23 && yPos > 4)  // User adjusting output Channel  
      {
        if(yPos < 9) // down
        {
          channelOut --;
          if(channelOut < 0){channelOut = 0;}         
        }
        else  // up
        {
          channelOut ++;
          if(channelOut >16){channelOut = 16;}    
        }
        Serial.print(" ChanOut: ");Serial.println(channelOut);
      } 
      if(yPos < 3 && xPos > 15) // User adjusting conductor matrix.
      {
        conductorAdjustMatrix (xPos, yPos);
      }
      
      if(yPos < 7 && xPos > 8 && xPos < 15) // User adjusting conductor Note Offset.
      {  
        conductorAdjustOffset (yPos);
      }  
                     
      ledDisplay(3, 0);   
      ledDisplayMode = 4;   
    }
    break;
    case 6:   // LoopLoad
    {
      if(yPos < 13)
      {
        loopNum = yPos;
        loadTheLoop(loopNum);
        ledDisplayMode = 0;         
        nextEvent(0);   
        ledRefreshNotes(); 
        tapActive = 0;
      }
    }
    break;
    case 18:   // Canon - Edit Chord NOtes.
    {
      if(yPos < 13 && xPos < 2)
      {
        //Serial.println("Edit Chord Notes");
        byte note = (winYNoteStart + yPos);  
        transposeEditChord(note);
        for(int y=0; y<13; y++){ledArray[y][0] = chordArray[winYNoteStart+y];}  // show on screen
      }   
    }
    break;    
    case 27:   // SaveLoop
    {
      if(seqMode){}
      else if(yPos < 13)
      {
        loopNum = yPos;  
        saveTheLoop(loopNum);                  
      }
      saveLoop = false;
      ledDisplayMode = 0; 
      ledRefreshNotes();ledRefresh();   
      tapActive = 0;    
    }
    break;           
  }
}


void processDraw(int item, int xPos, int yPos)
{
  switch (item)
  { 
    case 1:   // User is continuing to change Single note Velocity
    {

    }
    break; 
    case 2:  // scroll Y
    {
        int amount = yPos - buttonLastArray[1];
        winYNoteStart -= amount;  
        if(winYNoteStart < 21){winYNoteStart = 21;}                               // Make sure does not go out of bounds
        if(winYNoteStart > 95){winYNoteStart = 95;}                             // Make sure does not go out of bounds
        ledRefreshNotes();  
    }
    break;
    case 3:  // scroll X
    {
      
    }
    break;   
    case 4:  // Tie Notes
    {      
      // Rest is done when draw exits.
    }
    break;
    case 11:   // Draw Velocity.
    {
      int stepsPerLed = 24 / notesPerBeat; 
      drawVel = (yPos & 0xFF) * 10;               // x10 = to get to 127.  ie: row 6*10 =60 velocity.  row 12=120vel
      if(drawVel >127){drawVel=127;}
      drawVelStep = (winXStepStart + xPos) * stepsPerLed; 
      ledDisplay(5, 1);   
      ledDisplayMode = 3;                               // clears screen keeps the note/key lanes (ledDisplayMode 1 gets rid of lanes)
    }
    break;     
    case 16:   // StepStart Swipe active
    {
      if(seqMode)
      {
        seqArray[254] = xPos;    
        ledDisplayMode = 2;            // Screen to show Notes.           
      }
      else
      {
        int stepsPerLed = 24 / notesPerBeat; 
        int relXPos = winXStepStart + xPos;
        int s = relXPos * stepsPerLed;                                       // Windowed Notes.  Which step count is on the left (typcially step 0)
        // find which the step is closer to: start or finish of loop
        int L = stepStart;
        if(stepStart < winXStepStart){L = winXStepStart;}
        int left = s - L;
        int halfway = ((stepEnd - L) / 2) + L;
        if(s < halfway)
        {
          if(s < 0){s = 0;}
          if(s >= stepEnd){s = stepEnd - stepsPerLed;}
          if(s != stepStart)
          {  
            stepStart = s; 
                        //Serial.print("stepStart  ");Serial.println(stepStart); 
          }          
        }
        else
        {
          int newEnd = s + stepsPerLed - 1;
          if(newEnd > stepsInLoop){stepsInLoop = newEnd;}
          if(newEnd <= stepStart){newEnd = stepStart + stepsPerLed;}
          if(newEnd != stepEnd)
          {  
            stepEnd = newEnd;
                        //Serial.print("stepEnd  ");Serial.println(stepEnd);
          }                  
        }
        ledRefreshNotes();   
      }
    }
    break;  
    case 19:   
    {
      
    }
    break;    
    case 22:  // Canon: Vertical Swipe = Transpose
    {    
      if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving (debouncing)
      { 
          int pitch = yPos + winYNoteStart;
          offsetNotes(pitch); 
          ledRefreshNotes(); 
          eventLast = millis();    
      }    
    }
    break;   
    case 23:  // Canon: Horizontal Swipe = Phase
    {    
      if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving (debouncing)
      { 
          int amount = xPos - buttonLastArray[0];
          canonPhaseNotes(amount); // yPos is actually a swipe amount: either -1 or +1
          ledRefreshNotes(); 
          eventLast = millis();    
      }    
    }
    break;                    
  }
}


void processSwipe(int item, int xMoved, int yMoved, byte xAbs, byte yAbs)
{  
  switch (item)
  {
    case 0:   // Scroll Screen and TIE note Start
    {
      if(!swipeActive && (millis() - eventLast > 150) )  // debounce                           
      {  
        eventLast = millis();
        drawActive = 0;               
        if(yMoved != 0)
        {            
          winYNoteStart -= yMoved; 
          if(winYNoteStart < 21){winYNoteStart = 21;}                               // Make sure does not go out of bounds
          if(winYNoteStart > 95){winYNoteStart = 95;}                             // Make sure does not go out of bounds
          ledRefreshNotes(); 
          swipeActive = 0;
          drawActive = 2;             
        }
        else if(xMoved != 0)   // Tie notes move
        { 
          int stepsPerLed = 24 / notesPerBeat; 

          tieNotePSE[1] = (winXStepStart + buttonLastArray[0]) * stepsPerLed;           // *2 for actual steps.  Windowed Notes.  Which note pitch is on the y axis (typcially note 59 - middleB)  
          tieNotePSE[0] = buttonLastArray[1] + winYNoteStart;       
          swipeActive = 0;
          drawActive = 4;      
        } 
      }  
    }
    break;  
    case 15:   // Steps Per Beat Swipe active
    {
      if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving  (debouncing)
      { 
        eventLast = millis();
        int s = notesPerBeat;
        if(yMoved > 0){s = s * 2;}
        else{s = s / 2;}  // *2 so goes in increments of 1,2,4,8
        if(s > 8){s = 8;}
        if(s < 1){s = 1;}  
        notesPerBeat = s;
        ledRefreshNotes(); 
        ledDisplay(2, notesPerBeat);
      } 
    }
    break;   
    case 20:   // Phase amount
    {
      if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving (debouncing)
      { 
        eventLast = millis();
        if(xAbs < 16) // is the input channel being selected
        {
          phaseAmount = phaseAmount + yMoved;  // yMoved is actually a swipe amount: either -1 or +1
          if(phaseAmount < -99){phaseAmount = -99;}
          if(phaseAmount > 99){phaseAmount = 99;}        
        }
        if(xAbs > 15) // is the output channel being selected
        {
          phaseAmount = phaseAmount + yMoved;  // yMoved is actually a swipe amount: either -1 or +1
          if(phaseAmount < -99){phaseAmount = -99;}
          if(phaseAmount > 99){phaseAmount = 99;}             
        }
        ledDisplay(6, 0);
        ledDisplayMode = 1;  
      }
    }
    break;            
    case 22:   // canonPhase active
    {      
      int xNow = buttonLastArray[0] + xMoved;
      int yNow = buttonLastArray[1] + yMoved;  
      if(yNow != swipeStart[1])   // Vertical Swipe = Transpose
      {
        int pitch = yAbs + winYNoteStart;
        offsetNotes(pitch);
        ledRefreshNotes(); 
        eventLast = millis();    
        drawActive = 22; swipeActive = 0;
      }
      else if (xNow != swipeStart[0])   // Horizontal Swipe = PhaseShift
      {  
        canonPhaseNotes(xMoved); // xMoved is actually a swipe amount: either -1 or +1
        ledRefreshNotes(); 
        eventLast = millis();   
        drawActive = 23; swipeActive = 0;
      }
    }
    break;  
    case 25:   // BPM swipe active
    {
      if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving (debouncing)
      { 
      eventLast = millis();
      bpm = bpm + yMoved;  // yMoved is actually a swipe amount: either -1 or +1
      intClockuS = (60000000 /(long)bpm) / 24; 
      ledDisplay(1, bpm);
      ledDisplayMode = 1;   
      }
    }
    break;                       
  }     
}

void scrollScreen(int xMoved, int yMoved)  // for 2 finger scroll only
{
//  Serial.println(" two fingers.. ");
//  Serial.print(" xMoved = ");Serial.print(xMoved);Serial.print(" yMoved = ");Serial.println(yMoved);
}
void zoomScreen(int yMoved)  // for 2 finger scroll only
{
  if(millis() - eventLast > 100) // stops multiple actions faster than one is reasonably moving  (debouncing)
  { 
    eventLast = millis();
    int s = notesPerBeat;
    if(yMoved > 0){s = s * 2;}
    else{s = s / 2;}  // *2 so goes in increments of 1,2,4,8
    if(s > 8){s = 8;}
    if(s < 1){s = 1;}  
    notesPerBeat = s;
    ledRefreshNotes(); 
    //Serial.print(" zoom.. ");Serial.print(" yMoved = ");Serial.println(yMoved);
  }
}
