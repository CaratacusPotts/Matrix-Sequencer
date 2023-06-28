void ccOutSerial3 (byte ccNum, byte value)
{
//  byte messageNibble = 0xB0; // CC = 176 or 1011 0000
//  byte statusByte = messageNibble + 0; // channel 0;
//  Serial3.write(0xB0);Serial3.write(ccNum); Serial3.write(value);  
}

void ccOutMidi1 (byte ccNum, byte value, byte channel)
{
  byte messageNibble = 0xB0; // CC = 176 or 1011 0000
  byte statusByte = messageNibble + (channel - 1); 
  Serial1.write(statusByte);Serial1.write(ccNum); Serial1.write(value);  
  Serial.print(" ccOUT  Chan: ");Serial.print(channel);Serial.print(" cc: ");Serial.print(ccNum); Serial.print(" , ");Serial.println(value);
}

void ccIn (byte channel, byte number, byte value)
{ 
  Serial.print(" chan: ");Serial.print(channel);Serial.print(" cc: ");Serial.print(number); Serial.print(" , ");Serial.println(value);
  
  if(channel == channelIn) // if the controller is sending a message (sends controllerCC=true first, and then after controllerCC=false
  {
    switch(number)
    { 
      //  TOUCH BUTTONS ***********************************************       
      case 20: {touchCCAction(number, value);} break;   // TB01
      case 21: {touchCCAction(number, value);} break;   // TB02
      case 22: {touchCCAction(number, value);} break;   // TB03
      case 23: {touchCCAction(number, value);} break;   // TB04
      case 24: {touchCCAction(number, value);} break;   // TB05
      case 25: {touchCCAction(number, value);} break;   // TB06
      case 26: {touchCCAction(number, value);} break;   // TB07
      case 27: {touchCCAction(number, value);} break;   // TB08
      case 28: {touchCCAction(number, value);} break;   // TB09
      case 29: {touchCCAction(number, value);} break;   // TB10
      case 30: {touchCCAction(number, value);} break;   // TB11
      case 31: {touchCCAction(number, value);} break;   // TB12
      
      //  ENCODER BUTTONS ***********************************************
      
      case 32: //  Start / Stop
      { 
        if(value == 1)
        {  
          bitWrite(encoderButtonsActive,0,1);
        }
        else  // finger off
        {
          bitWrite(encoderButtonsActive,0,0);
          if(bitRead(encoderButtonEdit,0))                     // the button was pushed and encoder used while down..
          {
            bitWrite(encoderButtonEdit,0,0);                                 // reset
          }                
          else                                                // Was a normal button press with no action on the encoder.
          {
            startStop = !startStop;                                       // toggle state
            if(startStop && clkSync){loopStartFlag = true;}  
            else if(startStop && !clkSync){ loopStart(); }               
            else{ loopStop(); } 
            if(seqMode){ledDisplayMode = 2;}else{ledDisplayMode = 0;}
            ledRefresh();     
            Serial.print("Play: ");Serial.print(startStop);     
          } 
          encoderRefresh();  
        }
      }  
      break;  
      case 33: //  MUTE
      { 
        if(value == 1)
        {                    
        }
        else          // finger off.
        {
          mutePitchActive = !mutePitchActive;   
          if(mutePitchActive){notesMutePitchCalibrate();}
          else{mutePitch[1]=23; mutePitch[0] = 109;} // reset so all notes play 
          encoderRefresh();      
        }  
      }  
      break;           
      case 34: //  Record Over Top
      { 
        if(value == 1)
        {    
          bitWrite(encoderButtonsActive,2,1);
        }
        else
        {
          bitWrite(encoderButtonsActive,2,0);
          recOvertop();  
          encoderRefresh();
          Serial.print("Record Over Top  State: ");Serial.println(recordOvertop);                 
          ledRefresh();           
          transposeNoteTonic = 127;  // reset, as a new note will not get register on the transpose without this reset.
        }
      }  
      break;  
      case 35: //  CLEAR
      { 
        ccOutSerial3(number,value);
        if(value == 1)
        {    
          bitWrite(encoderButtonsActive,3,1);
          initialiseNoteArray(); 
          for(int y=0;y<13;y++){for(int x=0;x<32;x++){ledNoteArray[y][x] = 0;}}
          notesOffAll();
          clearNotes = false;
          ccOutSerial3(33,1);
          Serial.println("Clear");
        }
        else{bitWrite(encoderButtonsActive,3,0);encoderRefresh();}
      }  
      break; 
      case 36: //  MODE BUTTON
      { 
        if(value == 1)
        {  
          bitWrite(encoderButtonsActive,4,1);
          if(modeSelectState == 0) // Changing Channel Out as user is in Loop Mode
            {tempChanChange = channelOut; keyboardRefresh(4);}
          else if(modeSelectState == -1) // Changing Channel In as user is in Keyboard Mode
            {tempChanChange = channelIn; keyboardRefresh(4);}  
          else if(modeSelectState == -2) // Conductor Setting - Midi Chan
            {touchButtonFlag = 0; keyboardRefresh(6); keyboardPos = 0; Serial.println("EditConductorOn");} 
        }
        else
        {
          bitWrite(encoderButtonsActive,4,0);
          if(bitRead(encoderButtonEdit,4))                    // the button was pushed and encoder used while down..
          {
            bitWrite(encoderButtonEdit,4,0);                  // reset 
            if(modeSelectState == 0)                          // is in Loop Mode
            {
              channelOut = tempChanChange;
              byte ch[] = {channelOut}; writeEEPROM(globalMemStart+1,ch,1);Serial.print("Chan Out:");Serial.println(channelOut);
              keyboardRefresh(0);
            }  
            else if(modeSelectState == -1)  // is in KeyBoard Mode
            {
              channelIn = tempChanChange;
              byte ch[] = {channelIn}; writeEEPROM(globalMemStart+2,ch,1);Serial.print("Chan In:");Serial.println(channelIn);
              ccOutSerial3(102,channelIn);
              keyboardRefresh(2);
            }
            else if(modeSelectState == -2)  // is in Conductor Edit Mode
            {
              keyboardRefresh(3);
              touchButtonFlag = 0;
              for(int i=1; i<17; i++)
              {
                Serial.print(conductorOut[i][0]);Serial.print(" ");
              }Serial.println();
            } 
          }
          else if(modeSelectState == 0){keyboardRefresh(0);}  // is in Keyboard Mode
          else if(modeSelectState == -1){keyboardRefresh(2);}  // is in Keyboard Mode          
          else if(modeSelectState == -2)
            { keyboardRefresh(3);touchButtonFlag = 0; }                               // is in Conductor Edit Mode
          else if(modeSelectState == 1){keyboardRefresh(1);}  // is in Sequence Mode
          else                                                // Was a normal button press with no action on the encoder.
          {
          }
          encoderRefresh();  
        }        
      }  
      break;            
      case 37: //  Transpose Active
      { 
        if(value == 1)
        {
          bitWrite(encoderButtonsActive,5,1);  
          bitWrite(encoderButtonEdit,5,0);
          keyboardRefresh(5); 
        }
        else // button off
        {
          bitWrite(encoderButtonsActive,5,0);
          if(bitRead(encoderButtonEdit,5))                     // the button was pushed and encoder used while down..
          {
           bitWrite(encoderButtonEdit,5,0);                    // reset
           transposeSaveChord();
          }             
          else                                                 // Was a normal button press with no action on the encoder.
          {            
            transposeState(); 
          }
          if(modeSelectState == 0){keyboardRefresh(0);}  // is in Loop Mode
          else if(modeSelectState == 1){keyboardRefresh(1);}  // is in Sequence Mode
          else if(modeSelectState == -1){if(transposeActive){keyboardRefresh(3);}else{keyboardRefresh(2);}}  // is in Keyboard Mode
          else if(modeSelectState == -2){keyboardRefresh(3);}  // is in Keyboard Mode
          encoderRefresh();
        }
      }  
      break;   
      case 38: //  PHASE
      { 
        if(value == 1)  // button down (on)
        {
          bitWrite(encoderButtonsActive,6,1);   
          bitWrite(encoderButtonEdit,6,0); 
        }
        else   // button up (off)
        {
          bitWrite(encoderButtonsActive,6,0);
          if(bitRead(encoderButtonEdit,6))                     // the button was pushed and encoder used while down..
          {
            bitWrite(encoderButtonEdit,6,0); 
            if(modeSelectState == 0){keyboardRefresh(0);}  // is in Loop Mode
            else if(modeSelectState == 1){keyboardRefresh(1);}  // is in Sequence Mode
            else if(modeSelectState == -1){if(transposeActive){keyboardRefresh(3);}else{keyboardRefresh(2);}}  // is in Keyboard Mode
            else if(modeSelectState == -2){keyboardRefresh(3);}  // is in Keyboard Mode  
          }
          else
          {
            if(enCanonMode)
            {
              phaseActive = !phaseActive;
              ledRefresh();   
            }
            else
            {
                         
            }
            encoderRefresh();
          }
        }
      }  
      break;        
      case 39: //  
      { 
        if(value == 1)
        {  
          bitWrite(encoderButtonsActive,7,1);
          bitWrite(encoderButtonEdit,7,0);
        }
        else
        {
          bitWrite(encoderButtonsActive,7,0);
          
          if(bitRead(encoderButtonEdit,7))                     // the button was pushed and encoder used while down..
            {bitWrite(encoderButtonEdit,7,0);}                // reset
          else                                                // Was a normal button press with no action on the encoder.
          {
          }
          encoderRefresh();   
        }
      }  
      break;       
      case 40: //  
      { 
        if(value == 1)
        {  
          bitWrite(encoderButtonsActive,8,1);
          bitWrite(encoderButtonEdit,8,0);
        }
        else
        {
          bitWrite(encoderButtonsActive,8,0);
          
          if(bitRead(encoderButtonEdit,8))                     // the button was pushed and encoder used while down..
            {bitWrite(encoderButtonEdit,8,0);}                // reset
          else                                                // Was a normal button press with no action on the encoder.
          {
                   
          }  
          encoderRefresh(); 
        }
      }  
      break;  
      case 41: //  
      { 
        if(value == 1)  // button pushed down
        {  
          bitWrite(encoderButtonsActive,9,1);
        }
        else            // button released
        {
          bitWrite(encoderButtonsActive,9,0);    
          if(bitRead(encoderButtonEdit,9))                    // the button was pushed and encoder used while down..
            {bitWrite(encoderButtonEdit,9,0);}                // reset
          else                                                // Was a normal button press with no action on the encoder.
          {
            enCanonMode = !enCanonMode;
            ledRefresh();  
          } 
          encoderRefresh();  
        }
      }  
      break;        
 

      //  ENCODER KNOBS ***********************************************
      
      case 42: //  BPM
      { 
        if(bitRead(encoderButtonsActive,0))                         // if button pressed
        {
          bitWrite(encoderButtonEdit,0,1);                            // log you have been here
          if(value == 1){clkSync = true; ccOutSerial3(32,1);}   
          else if(value == 0){clkSync = false; ccOutSerial3(32,0);}  
          Serial.print(" clkSync: ");Serial.println(clkSync);
        }
        else
        {
          if(value == 1){bpm ++;} else if(value == 0){bpm --;}
          intClockuS = (60000000 /(long)bpm) / 24; 
          Serial.print(" bpm");Serial.println(bpm);
        }
      }  
      break;     
      case 43: //  MUTE PITCH
      { 
        if(mutePitchActive)
        {
          notesMutePitch(value);
        }
        else
        {
          int s = notesPerBeat;
          if(value > 0){s = s * 2;}
          else{s = s / 2;}  // *2 so goes in increments of 1,2,4,8
          if(s > 8){s = 8;}
          if(s < 1){s = 1;}  
          notesPerBeat = s;
          Serial.print("NperBeat: ");Serial.println(notesPerBeat);
          ledRefreshNotes(); 
        }
      }  
      break;         
      case 44: //  LOOP START POINT
      { 
        int newStart = stepStart;
        int s = 24 / notesPerBeat; 
        if(value == 1){newStart = newStart + s;} else{newStart = newStart - s;}        
        
        if(newStart >= stepEnd){newStart = stepEnd - s;}
        if(newStart < 0){newStart = 0;}
        stepStart = newStart;             
         
        ledRefreshNotes();   
        Serial.print("stepStart ");Serial.println(stepStart); 
      }  
      break;  
      case 45: //  LOOP END POINT
      { 
        int newEnd = stepEnd;
        int s = 24 / notesPerBeat; 
        if(value == 1){newEnd = newEnd + s;} else{newEnd = newEnd - s;}   
        //if(newEnd > stepsInLoop){newEnd = stepsInLoop;}
        if(newEnd <= stepStart){newEnd = stepStart + s;}
        stepEnd = newEnd;
        
        Serial.print("stepEnd ");Serial.println(stepEnd);  
        ledRefreshNotes();   
      }  
      break;          
      case 46: //  MODE 
      { 
        if(bitRead(encoderButtonsActive,4))                                     // button is pressed, and encoder is turning.
        {
          bitWrite(encoderButtonEdit,4,1);                                      // log that this has happened.
          if(modeSelectState == 0 || modeSelectState == -1)                     // if in LoopMode or Keyboard Mode
          {
            if(value == 1){tempChanChange ++;} else{tempChanChange --;}         // channel out select.
            if(tempChanChange >16){tempChanChange = 16;} 
            if(tempChanChange < 1){tempChanChange = 1;}  
            keyboardRefresh(4);
            Serial.print(" Chan: ");Serial.println(tempChanChange);  
          }   
          if(modeSelectState == -2)                     // if in Conductor Mode
          {
            if(touchButtonFlag == 0)  // user has not yet hit a button, and user turned knob to shift midi scale
            {
              keyboardPos = value;  // 0= midi 1-12,  1= midi 13-16,
              keyboardRefresh(6);
            }
            if(touchButtonFlag > 0)  // user has hit a button, and user turned knob to shift octave
            {
              
            }
          }
        }
        else
        {
          int v = 0; if(value == 1){v = 1;} else{v = -1;}  
          modeSelectState = modeSelectState + v;
          if(modeSelectState < -2){modeSelectState = -2;}
          if(modeSelectState > 1){modeSelectState = 1;}
          switch (modeSelectState)
          {
            case 0:   // LOOP MODE
            {
              seqMode = false;
              keyboardActive = false;
              loopLoad = true;  
              ledResetStateDefault();
              keyboardRefresh(0); 
              Serial.println("loopMode");                
            }
            break;
            case -1:  // KEYBOARD MODE
            {
              keyboardActive = true; conductor = false;
              keyboardRefresh(2);           // Show black and white notes
              Serial.println("keysActiv");              
            }
            break;
            case -2:  // CONDUCTOR MODE
            {
              keyboardActive = false; conductor = true;
              keyboardRefresh(3);           // Show black and white notes
              Serial.println("ConductorActiv");  
              // send Chords if channel outs set
              for(int i=0; i<16; i++)
              {if(conductorOut[i][0] > 0){conductorCCSendChord(i+1);}}                    
            }
            break;  
            case 1:   // SONG MODE
            {
              loopLoad = false; seqMode = true; noteVelo = false;    // we know that by whatever action this must be turned off if it was on before.
         
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
                seqArray[254] = 0; 
                b[0] = seqArray[254];
                writeEEPROM(seqMemStart+254,b,1);   
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
              Serial.println("seqMode");               
            }
            break;                            
          }
          encoderRefresh(); 
        }
      }  
      break;   
      case 47: //  TRANSPOSE
      {
        if(bitRead(encoderButtonsActive,5))  // if transpose encoder button active (held down)
        {
          encoderButtonEdit = bitWrite(encoderButtonsActive,5,1);
        }
        else
        {  
          int v = 0;
          if(value == 1){v = 1;} else{v = -1;}            
        
          if(enCanonMode)
          {
            byte p = getTransposeTonic();
            Serial.print("tranpose: ");Serial.print(p);
            transposePitch = p + v;
            Serial.print(" to: ");Serial.println(transposePitch);
            transposeFlag = true;
          }        
          else  // Change VELOCITY
          {
            int vNew = enRecLoopValue;
            if(vNew == 0)
            {
              vNew = nextVelocity;    
              if(vNew == 0) { vNew = noteArray[4][noteArray[0][0]]; }   
            }
            Serial.print(" nextVel: ");Serial.println(vNew);   
            vNew = vNew + (v*5);
            if(vNew > 127){vNew=127;} 
            if(vNew < 1){vNew=1;}
            enRecLoopValue = vNew;
            bitWrite(enRecLoopFlag,5,1);
            enRecLoopStartStep = stepCount - 1;  // get current loop point so can reset when pass this next loop around.
            Serial.print(" NewVelocity: ");Serial.println(enRecLoopValue);      
          } 
        }               
      }  
      break;  
      case 48: //  PHASE
      { 
        if(bitRead(encoderButtonsActive,6))  // if transpose encoder button active (held down)
        {
          encoderButtonEdit = bitWrite(encoderButtonsActive,6,1);  
          if(value == 1){phaseAmount++;} else{phaseAmount--;}   
          keyboardRefresh(8); 
          Serial.print(" PhaseAmount: ");Serial.println(phaseAmount);
        }
        else  // Just do Encoder normal
        {
          int d = 0;
          if(value == 1){d = 1;} else{d = -1;}   
          
          if(enCanonMode)  // Phase Notes
          {
            canonPhaseNotes(d); // yPos is actually a swipe amount: either -1 or +1
            ledRefreshNotes();      
          } 
          else  // Sustain Mod 
          {
            enRecLoopValue += d;
            bitWrite(enRecLoopFlag,6,1);
            enRecLoopStartStep = stepCount - 1;  // get current loop point so can reset when pass this next loop around.
            Serial.print(" Sustain: ");Serial.println(enRecLoopValue);                      
          }
        }
      }  
      break;        
      case 49: //  HORIZONTAL FLIP
      { 
        if(bitRead(encoderButtonsActive,7))  // if mode is canon functions (tranpose, phase etc.
        {
          encoderButtonEdit = bitWrite(encoderButtonsActive,7,1);  
        }
        else    // else note mod
        {
     
        }
      }  
      break;  
      case 50: //  VERTICAL FLIP
      {
        if(bitRead(encoderButtonsActive,8))  // if mode is canon functions  (tranpose, phase etc.
        {
          encoderButtonEdit = bitWrite(encoderButtonsActive,8,1); 
        }
        else    // else note mod
        {
     
        }
      }  
      break;   
      case 51: //  Switch 2 / FUNCTION
      { 
//          int d = 0;
//          if(value == 1){d = 1;} else{d = -1;}  
//          
//          winYNoteStart += value;  
//          if(winYNoteStart < 20){winYNoteStart = 20;}                               // Make sure does not go out of bounds
//          if(winYNoteStart > 107){winYNoteStart = 107;}                             // Make sure does not go out of bounds
//          ledRefreshNotes();     
      }  
      break;    

      case 64:
      {
        bool on = false;
        if(value == 127){on = true;} else{on = false;} 
        if(on != recordPedalLast)  // state has changed
        {
          unsigned long timeNow = micros();
          unsigned long timePast = timeNow - recordPedalTimeLast;  
          if(timePast > 80000)  // debounceing
          {
            if(timePast > 750000)  // 0.75Sec is longer than exceptable for double-tap time - so do normal record actions
            {  
              recordPedalTimeLast = timeNow;  
              if(on && !recording && !transposeActive) {recordFlag = true;}
              else if (on && !recording && transposeActive) {transposeChordFlag = 1; Serial.println("Transp Chord");}
              else if (!on && recording) {recordingStop = true; Serial.println("Rec Stop");} // if low, then stop the recording (foot is off pedal)
              else if (!on && recordOvertop) {recOvertop(); if(!recordOvertop){saveTheLoop(loopNum);}}    
              else if (!on && !recording && transposeActive) {transposeChordFlag = 0;Serial.println("Transp Chord Off");}   
            }
            else //  if timelast is greater than 80mS and less than 750mS
            {
              if     (!on && recordFlag)        {recordFlag = false; recCCNextPattern = true;}
              else if(on && recCCNextPattern)   {recordFlag = false; recCCNextPattern = false; recOvertop();}
              else if(!on && recordOvertop)     {recordFlag = false; recCCNextPattern = false; recOvertop(); transposeState();}
            } 
          }  
          recordPedalLast = on;      
        }  
      }
      break;     
      case 102:
      {
        if(value == 1) // Is MIDI Channel Change (and startup)
        {          
          Serial.println("Control Pres");
          controllerAttached = true;
          encoderButtonsActive = 0; encoderButtonEdit = 0;
          if(modeSelectState == -1){keyboardRefresh(2);} else{keyboardRefresh(0);}
          encoderRefresh();
        }
        if(value == 127) // next note is a conductor tranpose
        {
          conductorTransposeFlag = true;
          //Serial.print(" ConTransFlag: ");Serial.println(conductorTransposeFlag);
        }
        if(value == 126) // next note is a conductor tranpose off
        {
          conductorTransposeOffFlag = true;
        } 
        if(value == 103){ccByte1Flag=true;}       
      }
      break;  
      case 103:
      {
        ccByte1 = value;
      }
      break;
      case 104:
      {  
 //       if(ccByte1Flag)
 //       {
          ccByte1Flag=false;
          uint16_t scaleKeys = (ccByte1 << 8) + value;
          int count = 0;
          for(int i=0; i<9; i++) // update chordArray
          {
            for(int c=0; c<12; c++)
            {
              chordArray[count] = (bitRead(scaleKeys,c));
              count ++;
            }
          }
          conductorTransposeOffset = 127;
          Serial.println(" scaleUpdated");
//        }
      } 
      break;          
    }
  }
}


void touchCCAction(byte button, byte value) // touch button actions
{
  if(value == 1)
  {  
    if(bitRead(encoderButtonsActive,5))                  // If encoder button 5 is down (transpose button)  Do transpose thing.
    {
      bitWrite(encoderButtonEdit,5,1);                   // log that you have done something - so if you lift finger off encoder button the default state is not activated.
      transposeEditChord(button + 28);
      keyboardRefresh(5);    
    }
    else if(modeSelectState == 0)  // Loop Mode
    {
      ccButtonLast = millis();
      touchButtonFlag = button; 
      ccOutSerial3(button,5);
    } 
    else if(modeSelectState == -1)  // Keyboard Play Note
    {
      if(transposeActive)
      {
        int v;
        if(button < 25){v= button - 25;}else{v=button-24;}
        transposeByInterval(v);
      }
      else
      { 
        noteIn (channelIn, button + 28, 100);
        ccOutSerial3(button,5);
      }
    }
    else if(modeSelectState == -2)    // Conductor Mode and EncoderButton Pressed
    {
      if(bitRead(encoderButtonsActive,4))  //  User selecting Conductor Midi Outs
      {
        bitWrite(encoderButtonEdit,4,1);
        if(touchButtonFlag == 0)   // first time press - Select Midi Chan
        {
          int midiChan = (button-19) + (keyboardPos * 12);
          if(conductorOut[midiChan][0] > 0) // already had a value - reset
          {
            touchButtonFlag = 0;
            conductorOut[midiChan][0] = 0; // 0= no action.     
            conductorOut[midiChan][1] = 0;
            keyboardRefresh(6);
          }   
          else  // nothing there - set to 1
          {
            touchButtonFlag = midiChan;
            conductorOut[midiChan][0] = 0; // 0= no action.   
            conductorOut[midiChan][1] = 0;  
            keyboardRefresh(7);
          }                                   
        }
        else if(touchButtonFlag > 0)  // second time press - Select Notes
        {
          int midiChan = touchButtonFlag;  touchButtonFlag = 0;
          int finger = 1;
          if(button == 22){finger = 2;}
          if(button == 24){finger = 3;}
          if(button == 31){finger = 5;}
          conductorOut[midiChan][0] = finger;  // 0= no action.
          keyboardRefresh(6);  // reset (show Midi Out)
          conductorCCSendChord(midiChan + 1);              
        } 
        //keyboardPos = 0;        
      }
      else // Transpose 
      {
        int v;
        if(button < 25){v= button - 25;}else{v=button-24;}
        transposeByInterval(v);
      }
    }
  }
  if(value == 0)
  {
    if(bitRead(encoderButtonsActive,5))                  // If encoder button 5 is down (transpose button)  Do transpose thing.
    {
    }    
    else if(modeSelectState == 0)  // Loop Mode
    {
      touchButtonFlag = 0;
      if(millis() - ccButtonLast > 1500) // long press
      {
        saveTheLoop(button-20);
        Serial.print("SavingLoopTo: ");Serial.println(button-20);
        loopNum = button-20;
        keyboardRefresh(0); 
      }
      else
      {
        if(loopNum != button - 20)   // user selecting new loop
        {
          loopNum = button-20; 
          loadTheLoop(loopNum); 
          Serial.print("load Loop: ");Serial.println(loopNum);
          nextEvent(0); 
          ledRefreshNotes();    
        }
        else // mute selecting same loop - there MUTE the loop
        {
          mute = !mute;                                // toggle state
          Serial.print("mute: ");Serial.println(mute);
        }          
        keyboardRefresh(0); 
      }     
    }
    else if(modeSelectState == -1)  // Keyboard Play Note
    {
      if(transposeActive)
        {keyboardRefresh(3);}
      else
      {
        noteOffIn (channelIn, button + 33, 0);                   // button 20 = 53 = Note F3
        keyboardRefresh(2);
      }
    }
//    else if(modeSelectState == -2 && bitRead(encoderButtonsActive,4))    // Conductor Mode and EncoderButton Pressed
//    {
//      if(touchButtonFlag > 1)  // Midi CHan Selected.   Show Fingeer Pos
//      {
//        
//        //ccOutSerial3(25,5);  // highlight middle to show where on keyboard octave you are.
//      }    
//    }
  }
}


 
void keyboardRefresh(byte state)
{
  switch(state)
  {
    case 0:  // Loop Mode
    {
      ccOutSerial3(20,0);
      ccOutSerial3(21,0);
      ccOutSerial3(22,0);
      ccOutSerial3(23,0);
      ccOutSerial3(24,0);
      ccOutSerial3(25,0);
      ccOutSerial3(26,0);
      ccOutSerial3(27,0);
      ccOutSerial3(28,0);
      ccOutSerial3(29,0);
      ccOutSerial3(30,0);
      ccOutSerial3(31,0);  
      if(loopNum < 12){byte v=1; if(mute){v=3;} ccOutSerial3(20+loopNum,v);} // Set loopnum LED to On       
    }
    break;
    case 2: // Keyboard Mode
    {
      ccOutSerial3(20,2);
      ccOutSerial3(21,2);
      ccOutSerial3(22,2);
      ccOutSerial3(23,2);
      ccOutSerial3(24,2);
      ccOutSerial3(25,2);
      ccOutSerial3(26,2);
      ccOutSerial3(27,2);
      ccOutSerial3(28,2);
      ccOutSerial3(29,2);
      ccOutSerial3(30,2);
      ccOutSerial3(31,2);        
    }
    break;    
    case 3: // Conductor Mode
    {
      ccOutSerial3(20,4);
      ccOutSerial3(21,4);
      ccOutSerial3(22,2);
      ccOutSerial3(23,2);
      ccOutSerial3(24,1);
      ccOutSerial3(25,1);
      ccOutSerial3(26,2);
      ccOutSerial3(27,2);
      ccOutSerial3(28,4);
      ccOutSerial3(29,4);
      ccOutSerial3(30,0);
      ccOutSerial3(31,0);        
    }
    break;  
    case 4:  // Channel In/Out
    {
      if(tempChanChange < 13) // Set LEDS
      {
        for(int i=20;i<32;i++){ccOutSerial3(i,2);} 
        ccOutSerial3(tempChanChange+19,1);
      }
      else 
      {
        for(int i=20;i<24;i++){ccOutSerial3(i,2);} 
        for(int i=24;i<32;i++){ccOutSerial3(i,0);} 
        ccOutSerial3(tempChanChange + 7,1);           
      }         
    }
    break;
    case 5: // Transpose - Show Scale Notes
    {
      for(byte i=0; i<12; i++) 
      {
        Serial.print(chordArray[i+48]);Serial.print(",");
        if(chordArray[48+i] == 1){ccOutSerial3(20+i,4);}
        else{ccOutSerial3(20+i,0);}
      }    
    }
    break;   
    case 6: // Show Conductor Midi Out Midi 1-12
    {
      if(keyboardPos == 0) 
      {
        for(int i=1;i<13;i++)
        {
          if(conductorOut[i][0] > 0 && conductorOut[i][0] < 5){ccOutSerial3(i+19,1);}
          else if(conductorOut[i][0] == 5){ccOutSerial3(i+19,5);}
          else{ccOutSerial3(i+19,2);} // set blue
        } 
      }
      else 
      {
        for(int i=13;i<17;i++)
        {
          if(conductorOut[i][0] > 0){ccOutSerial3(i+7,1);}
          else{ccOutSerial3(i+7,2);} // set blue
        } 
        for(int i=24;i<32;i++){ccOutSerial3(i,0);}          
      }       
    }
    break;    
    case 7: // Show Conductor Finger Options
    {
      ccOutSerial3(20,1);
      ccOutSerial3(21,0);
      ccOutSerial3(22,1);
      ccOutSerial3(23,0);
      ccOutSerial3(24,1);
      ccOutSerial3(25,0);
      ccOutSerial3(26,0);
      ccOutSerial3(27,0);
      ccOutSerial3(28,0);
      ccOutSerial3(29,0);
      ccOutSerial3(30,0);
      ccOutSerial3(31,5);         
    }
    break;     
    case 8:  // Phase Amount
    {
      for(int i=20;i<32;i++){ccOutSerial3(i,0);} 
      if(phaseAmount < 0 && phaseAmount > -13){ccOutSerial3(abs(phaseAmount)+19,1);}
      if(phaseAmount > 0 && phaseAmount < 13){ccOutSerial3(phaseAmount+19,2);}         
    } 
    break; 
  }
}

void encoderRefresh()
{
  ccOutSerial3(32,startStop);
  ccOutSerial3(33,mutePitchActive);
  ccOutSerial3(34,recordOvertop);
  ccOutSerial3(35,0);
  switch (modeSelectState)
  {
    case 0: {ccOutSerial3(36,1);}break;
    case 1: {ccOutSerial3(36,2);}break;
    case -1: {ccOutSerial3(36,3);}break;
    case -2: {ccOutSerial3(36,4); }break;
  }  
  ccOutSerial3(37,transposeActive);
  ccOutSerial3(38,phaseActive);
  ccOutSerial3(39,0); 
  ccOutSerial3(40,0); 
  ccOutSerial3(41,!enCanonMode);
}
