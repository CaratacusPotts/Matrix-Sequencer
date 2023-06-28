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
  //Serial.print(" chan: ");Serial.print(channel);Serial.print(" cc: ");Serial.print(number); Serial.print(" , ");Serial.println(value);
  
  if(channel == channelIn) // if the controller is sending a message (sends controllerCC=true first, and then after controllerCC=false
  {
    if(number == ccNum)  // ccNum = 32
    {
      int enNum = (value /4);
      int enVal = value%4;
      unsigned long timeNow = millis();
      if(enNum < 15) // is an encoder button or knob.
      {  
        // ENCODER TURNED
        if(enVal < 2) 
        {
          Serial.print("EnButTurn: ");Serial.println(bitRead(encoderButtonsActive,enNum));
          if(bitRead(encoderButtonsActive,enNum))                       // if button is being held down
          {
            bitWrite(encoderButtonEdit,enNum,1);                        // log you have been here
            Serial.print("EnButtDownTurn: ");Serial.println(enNum);
            switch(enNum)
            {
              case 0:   {                                           }break;   
              case 1:   {   }break;
              case 2:   {   }break;
              case 3:   { settingChangeStepPerBeat(enVal);          }break;
              case 4:   {   }break;
              case 5:   {   }break;
              case 6:   { phaseStepAmount(enVal);                   }break;
              case 7:   {   }break;
              case 8:   { canonInvertSetMiddle(enVal);              }break;
              case 9:   {   }break;
              case 10:  {   }break;
              case 11:  {   }break;
              case 12:  {   }break;
              case 13:  {   }break;
              case 14:  {   }break;            
            }     
          }
          else
          {
            Serial.print("EnTurn: ");Serial.print(enNum);Serial.print(" Val: ");Serial.println(enVal);
            switch(enNum)
            {
              case 0:   { settingChangeBPM(enVal);                  }break;     
              case 1:   { settingChangeLoopEndPoint(enVal);         }break;
              case 2:   { settingChangeLoopStartPoint(enVal);       }break;
              case 3:   { ledPanLeftRight(enVal);                   }break;
              case 4:   { if(editSettings !=0){settingsEdit(enVal);} else{settingModeSelect(enVal);} }break;
              case 5:   { transposeEncoder(enVal);                  }break;
              case 6:   { phaseNotes(enVal);                        }break;
              case 7:   { canonFreqAdjust(enVal);                   }break;
              case 8:   { canonInvert(enVal);                       }break;
              case 9:   {   }break;
              case 10:  { notesMutePitchEncoder(1,enVal);           }break;
              case 11:  { notesMutePitchEncoder(0,enVal);           }break;
              case 12:  { notesEncoderVelocity(enVal);              }break;
              case 13:  { notesSustainChange(enVal);                }break;
              case 14:  {   }break;            
            }
          } 
        // BUTTON PRESSED    
        }
        else if(enVal == 2 && (timeNow - knobActionTimeLast) > 100)   // button on and debounced
        {
          bitWrite(encoderButtonsActive,enNum,1);
          Serial.print("EnButtOn: ");Serial.println(enNum);
          switch(enNum)
          {
            case 0:   {   }break;
            case 1:   {   }break;
            case 2:   { leds[2] = CRGB(255,0,0); FastLED.show();    }break;
            case 3:   {   }break;
            case 4:   {   }break;
            case 5:   { transposeState(1);                           }break;  // show transpose notes
            case 6:   {   }break;
            case 7:   {   }break;
            case 8:   { ledShowInvertLine(1);                        }break;
            case 9:   {   }break;
            case 10:  {   }break;
            case 11:  {   }break;
            case 12:  {   }break;
            case 13:  {   }break;
            case 14:  {   }break;            
          }
        }
        else if(enVal == 3 && (timeNow - knobActionTimeLast) > 50)   // button off and debounced
        {
          bitWrite(encoderButtonsActive,enNum,0);
          if(bitRead(encoderButtonEdit,enNum))                          // the button was pushed and encoder used while down..
          {
            bitWrite(encoderButtonEdit,enNum,0);                        // reset
            Serial.print("EnButtOffEnDown: ");Serial.println(enNum);
            switch(enNum)
            {
              case 0:   {   }break;
              case 1:   {   }break;
              case 2:   {   }break;
              case 3:   {   }break;
              case 4:   {   }break;
              case 5:   { transposeState(2);  }break;
              case 6:   {   }break;
              case 7:   {   }break;
              case 8:   { ledShowInvertLine(0);                         }break;
              case 9:   {   }break;
              case 10:  {   }break;
              case 11:  {   }break;
              case 12:  {   }break;
              case 13:  {   }break;
              case 14:  {   }break;            
            }
          }                
          else                                                          // Was a normal button press with no action on the encoder.
          {
            Serial.print("EnButtOff: ");Serial.println(enNum);
            switch(enNum)
            {
              case 0:   { settingStartStop();                 }break;
              case 1:   { recOvertop();                       }break;
              case 2:   { settingClearNotes(); leds[2] = CRGB(0,0,0); FastLED.show();  }break;
              case 3:   {   }break;
              case 4:   { settingsEdit(-1);                   }break;
              case 5:   { transposeState(2);                   }break;
              case 6:   { phaseOnOff();                       }break;
              case 7:   {   }break;
              case 8:   { ledShowInvertLine(0);               }break;
              case 9:   {   }break;
              case 10:  { noteMutePitchButton(1);             }break;
              case 11:  { noteMutePitchButton(0);             }break;
              case 12:  { notesEnButtDrawVelocity();          }break;
              case 13:  {   }break;
              case 14:  {   }break;            
            }
          }
        }
        knobActionTimeLast = timeNow;
      }
      else // is a touch button
      {
        enVal = !enVal;
        Serial.print("TouchButt: ");Serial.print(enNum-15);Serial.print(" Val: ");Serial.println(enVal);
        if((timeNow - buttActionTimeLast) > 50)  // debounce
        {
          int buttNum = enNum - 15;
          if(editSettingsChord) { if(enVal){settingChangeScaleNotes(buttNum);} } // finger on
          else
          {
            switch(Mode)
            {        
              case 0:  // Loop Mode
              {
                if(enVal)                          //  Raise Flag for long press save (main loop).  Using button number.
                  {buttHoldFlag = buttNum; keyboardRefresh(11); Serial.print(" buttHold ");Serial.println(millis());}          
                else 
                {
  //                if(timeNow - buttActionTimeLast < 1500)  // so a long press (Save) is not actioned as something else as well.
  //                {
                    buttHoldFlag = -1;
                    Serial.print(" off ");Serial.println(millis());
                    if(buttNum == loopNum && timeNow - buttActionTimeLast < 1000)            // mute selecting same loop - MUTE/unMUTE the loop
                      {mute = !mute; keyboardRefresh(0); Serial.println(" Mute");} 
                    else                              // user selecting new loop
                      {loadTheLoop(buttNum);Serial.print("Load: ");Serial.println(buttNum);} 
   //               }   
                }              
              } 
              break;   
              case 1:  // Song Mode
              {
                if(enVal)
                  { }
                else
                  { }              
              } 
              break;  
              case -1: // Keyboard Mode
              {
                if(enVal)
                  { noteIn(channelIn, buttNum + 48, 100);}  // C3 = 48
                else
                {
                  if(transposeActive)
                    {if(buttNum < 5){transposeByInterval(buttNum-5);}  else{transposeByInterval(buttNum-4);} }
                  else
                    {noteOffIn(channelIn, buttNum + 48, 0);}  // C3 = 48
                }              
              } 
              break;  
              case -2:  // Conductor Mode
              {
                if(enVal)
                {                  
                }
                else
                  {if(buttNum < 5){transposeByInterval(buttNum-5);}else{transposeByInterval(buttNum-4);} }              
              } 
              break;     
            } 
          }
        }
        buttActionTimeLast = timeNow;      
      }
    }
    else
    {
      switch(number) // cc IN
      {   
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
                else if(!on && recordOvertop)     {recordFlag = false; recCCNextPattern = false; recOvertop(); transposeState(2);}
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
            if(Mode == -1){keyboardRefresh(0);} else{keyboardRefresh(0);}
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
        } 
        break;          
      }      
    }
  }
}
