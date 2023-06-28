// ledState is Array holding Output for LED Shift Register
// Numbering is based on schematic.  3 x 16bit shift registers = 48 states
// 0=BUT_LED_1R, 1=BUT_LED_1B, 2=BUT_LED_1G, 3=BUT_LED_2R....... 36= EN_LED_01, 37=EN_LED_02......40=EN_LED_05R, 41=EN_LED_05G, 42=EN_LED_05G, 43=EN_LED_06..
 

void outputLed()
{
  digitalWrite(latchPin, LOW);                                  // Start the shift out sequence  x-axis first, y-axis last        
  for(int i=47; i>-1; i--)
  {
    digitalWrite(dataPin, ledState[i]);
    digitalWrite(clockPin, HIGH);
    //delayMicroseconds(3);
    digitalWrite(clockPin, LOW); 
  }
  digitalWrite(latchPin, HIGH);                                 // End the shift out sequence        
} 

void output(int obj, int num, int value)  // 0-11 tb, 12-17 enTb 
{
  unsigned long timePast = millis() - ccOutputLast[num];
  if(timePast > 50) // debounce 
  {
    ccOutputLast[num] = millis();
    
    Serial.print(" obj: ");Serial.print(obj);
    Serial.print(" num: ");Serial.print(num);
    Serial.print(" value: ");Serial.println(value);
     
    // send a byte with the controller value for Channel 16 (binary 1011,1111)( 1011 being continuous controller function , 
    // 1111 being midi channel 16) 
  
    byte messageNibble = 0xB0; // CC = 176 or 1011 0000
    byte statusByte = messageNibble + (channelOut - 1);
    Serial1.write(statusByte);  
  //  Serial1.write(98); Serial1.write(obj); // Channel , CC, Value
  //  Serial1.write(99); Serial1.write(num); // Channel , CC, Value
  //  Serial1.write(100); Serial1.write(value); // Channel , CC, Value
    
    switch(obj)
    {
      case 1:   // touchButton
      {
        switch (num)
        {
          case 1:{ Serial1.write(20); Serial1.write(value);/* ledState[0] = value; */} break;
          case 2:{ Serial1.write(21); Serial1.write(value);/* ledState[3] = value; */} break;
          case 3:{ Serial1.write(22); Serial1.write(value);/* ledState[6] = value; */} break;
          case 4:{ Serial1.write(23); Serial1.write(value);/* ledState[9] = value; */} break;
          case 5:{ Serial1.write(24); Serial1.write(value);/* ledState[12] = value; */} break;
          case 6:{ Serial1.write(25); Serial1.write(value);/* ledState[15] = value; */} break;
          case 7:{ Serial1.write(26); Serial1.write(value);/* ledState[18] = value; */} break;
          case 8:{ Serial1.write(27); Serial1.write(value);/* ledState[21] = value; */} break;
          case 9:{ Serial1.write(28); Serial1.write(value);/* ledState[24] = value; */} break;
          case 10:{ Serial1.write(29); Serial1.write(value);/* ledState[27] = value; */} break;
          case 11:{ Serial1.write(30); Serial1.write(value);/* ledState[30] = value; */} break;
          case 12:{ Serial1.write(31); Serial1.write(value);/* ledState[33] = value; */} break;     
        }
      }
      break;
      case 2:   // Encoder Button
      { 
        switch (num)
        {
          case 1:{ Serial1.write(32); Serial1.write(value); /*ledState[36] = value;*/ } break;
          case 2:{ Serial1.write(33); Serial1.write(value); /*ledState[37] = value;*/ } break;
          case 3:{ Serial1.write(34); Serial1.write(value); /*ledState[38] = value;*/ } break;
          case 4:{ Serial1.write(35); Serial1.write(value); /*ledState[39] = value;*/ } break;
          case 5:{ Serial1.write(36); Serial1.write(value); /*ledState[40] = value;*/ } break;
          case 6:{ Serial1.write(37); Serial1.write(value); /*ledState[43] = value;*/ } break;
          case 7:{ Serial1.write(38); Serial1.write(value); /*ledState[44] = value;*/ } break;
          case 8:{ Serial1.write(39); Serial1.write(value); /*ledState[45] = value;*/ } break;
          case 9:{ Serial1.write(40); Serial1.write(value); /*ledState[46] = value;*/ } break;
          case 10:{ Serial1.write(41); Serial1.write(value); /*ledState[47] = value;*/ } break;
        }
      }
      break;    
      case 3:   // Encoder Turn
      {
        switch (num)
        {
          case 1:{ Serial1.write(42); Serial1.write(value); } break;
          case 2:{ Serial1.write(43); Serial1.write(value); } break;
          case 3:{ Serial1.write(44); Serial1.write(value); } break;
          case 4:{ Serial1.write(45); Serial1.write(value); } break;
          case 5:{ Serial1.write(46); Serial1.write(value); } break;
          case 6:{ Serial1.write(47); Serial1.write(value); } break;
          case 7:{ Serial1.write(48); Serial1.write(value); } break;
          case 8:{ Serial1.write(49); Serial1.write(value); } break;
          case 9:{ Serial1.write(50); Serial1.write(value); } break;
          case 10:{ Serial1.write(51); Serial1.write(value); } break;
        }      
      }
      break; 
      case 4: // Other
      {
        switch (num)
        {
          case 102:{Serial1.write(102); Serial1.write(value); } break;
        }
      }
    }
  }
}


void calcEncoder(byte port, byte b)
{ 
  //Serial.print(" Port: ");Serial.print(port);Serial.print(" b: ");Serial.println(b,BIN);
  switch (port)
  {
    case 1:
    {
      switch (b) 
      {
        case(1):    { if(bitRead(enP1Last,0) == 1 && bitRead(enP1Last,1) == 0){ output(3, 1, 1);}  }  break;   // Right
        case(2):    { if(bitRead(enP1Last,1) == 1 && bitRead(enP1Last,0) == 0){ output(3, 1, 0);}  }  break;   // Left
        case(4):    { if(bitRead(enP1Last,2) == 1 && bitRead(enP1Last,3) == 0){ output(3, 2, 1);}  }  break;   // 0Right
        case(8):    { if(bitRead(enP1Last,3) == 1 && bitRead(enP1Last,2) == 0){ output(3, 2, 0);}  }  break;   // Left
        case(16):   { if(bitRead(enP1Last,4) == 1 && bitRead(enP1Last,5) == 0){ output(3, 3, 1);}  }  break;   // Right
        case(32):   { if(bitRead(enP1Last,5) == 1 && bitRead(enP1Last,4) == 0){ output(3, 3, 0);}  }  break;   // Left
        case(64):   { if(bitRead(enP1Last,6) == 1 && bitRead(enP1Last,7) == 0){ output(3, 4, 1);}  }  break;   // Right
        case(128):  { if(bitRead(enP1Last,7) == 1 && bitRead(enP1Last,6) == 0){ output(3, 4, 0);}  }  break;   // Left 
      }    
    }
    break;
    case 2:
    {
      switch (b) 
      {
        case(1):    { if(bitRead(enP2Last,0) == 1 && bitRead(enP2Last,1) == 0){ output(3, 5, 1);}  }  break;   // Right
        case(2):    { if(bitRead(enP2Last,1) == 1 && bitRead(enP2Last,0) == 0){ output(3, 5, 0);}  }  break;   // Left
        case(4):    { if(bitRead(enP2Last,2) == 1 && bitRead(enP2Last,3) == 0){ output(3, 6, 1);}  }  break;   // 0Right
        case(8):    { if(bitRead(enP2Last,3) == 1 && bitRead(enP2Last,2) == 0){ output(3, 6, 0);}  }  break;   // Left
        case(16):   { if(bitRead(enP2Last,4) == 1 && bitRead(enP2Last,5) == 0){ output(3, 7, 1);}  }  break;   // Right
        case(32):   { if(bitRead(enP2Last,5) == 1 && bitRead(enP2Last,4) == 0){ output(3, 7, 0);}  }  break;   // Left
        case(64):   { if(bitRead(enP2Last,6) == 1 && bitRead(enP2Last,7) == 0){ output(3, 8, 1);}  }  break;   // Right
        case(128):  { if(bitRead(enP2Last,7) == 1 && bitRead(enP2Last,6) == 0){ output(3, 8, 0);}  }  break;   // Left 
      }
    }
    break;
    case 3:
    {
      switch (b) 
      {
        case(1):  { if(bitRead(enP3Last,0) == 1 && bitRead(enP3Last,1) == 0){ output(3, 9, 1);}  }  break;   // Right
        case(2):  { if(bitRead(enP3Last,1) == 1 && bitRead(enP3Last,0) == 0){ output(3, 9, 0);}  }  break;   // Left
        case(4):  { if(bitRead(enP3Last,2) == 1 && bitRead(enP3Last,3) == 0){ output(3, 10, 1);}  }  break;   // 0Right
        case(8):  { if(bitRead(enP3Last,3) == 1 && bitRead(enP3Last,2) == 0){ output(3, 10, 0);}  }  break;   // Left
       
        //  BUTTONS
        case(16):  // 0001 0000
          { buttonState[12] = !buttonState[12]; output(2, 1, buttonState[12]);}
        break;
        case(32):  // 0010 0000
          { buttonState[13] = !buttonState[13]; output(2, 2, buttonState[13]); }
        break;
        case(64):  // 0100 0000
          { buttonState[14] = !buttonState[14]; output(2, 3, buttonState[14]); }
        break;
        case(128):  // 1000 0000
          { buttonState[15] = !buttonState[15]; output(2, 4, buttonState[15]);}
        break;
      }
    }
    break;
  }
}


void calcButtons()
{
  const int tbMap[] = {18,9,4,3,29,30,28,2,5,12,7,19};      // 12 items.  touch button TB01-TB12
  const int enTbMap[] = {40,41,14,42,16,17};                // encoder buttion 05-10
  for(int i=0; i<12; i++)
  {
    bool s = digitalRead(tbMap[i]);
    if(buttonState[i] != s) { buttonState[i] = s; output(1, i+1, !s); }
  }  
  for(int i=0; i<6; i++)    
  { 
    bool s = digitalRead(enTbMap[i]);
    if(buttonState[i+16] != s) { buttonState[i+16] = s; output(2, i+5, !s); }
  }  
}
