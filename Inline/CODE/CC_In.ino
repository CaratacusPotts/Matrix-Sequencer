void ccIn (byte channel, byte num, byte value)
{
  switch(num) 
  {
    // Touch Buttons
    case 20:  { ledState[0] = bitRead(value,0); ledState[1] = bitRead(value,1); ledState[2] = bitRead(value,2); } break;
    case 21:  { ledState[3] = bitRead(value,0); ledState[4] = bitRead(value,1); ledState[5] = bitRead(value,2); } break;
    case 22:  { ledState[6] = bitRead(value,0); ledState[7] = bitRead(value,1); ledState[8] = bitRead(value,2); } break;
    case 23:  { ledState[9] = bitRead(value,0); ledState[10] = bitRead(value,1); ledState[11] = bitRead(value,2); } break;
    case 24:  { ledState[12] = bitRead(value,0); ledState[13] = bitRead(value,1); ledState[14] = bitRead(value,2); } break;
    case 25:  { ledState[15] = bitRead(value,0); ledState[16] = bitRead(value,1); ledState[17] = bitRead(value,2); } break;
    case 26:  { ledState[18] = bitRead(value,0); ledState[19] = bitRead(value,1); ledState[20] = bitRead(value,2); } break;
    case 27:  { ledState[21] = bitRead(value,0); ledState[22] = bitRead(value,1); ledState[23] = bitRead(value,2); } break;
    case 28:  { ledState[24] = bitRead(value,0); ledState[25] = bitRead(value,1); ledState[26] = bitRead(value,2); } break;
    case 29:  { ledState[27] = bitRead(value,0); ledState[28] = bitRead(value,1); ledState[29] = bitRead(value,2); } break;
    case 30:  { ledState[30] = bitRead(value,0); ledState[31] = bitRead(value,1); ledState[32] = bitRead(value,2); } break;
    case 31:  { ledState[33] = bitRead(value,0); ledState[34] = bitRead(value,1); ledState[35] = bitRead(value,2); } break;
    
    case 32: {if(value){ledState[36] = 1;}else{ledState[36] = 0;}} break;
    case 33: {if(value){ledState[37] = 1;}else{ledState[37] = 0;}} break;
    case 34: {if(value){ledState[38] = 1;}else{ledState[38] = 0;}} break;
    case 35: {if(value){ledState[39] = 1;}else{ledState[39] = 0;}} break;
    case 36:  // MODE
    {
     if(value == 1)       {ledState[40] = 1; ledState[41] = 0; ledState[42] = 0;}
     else if(value == 2)  {ledState[40] = 0; ledState[41] = 1; ledState[42] = 0;}
     else if(value == 3)  {ledState[40] = 1; ledState[41] = 1; ledState[42] = 0;}
     else if(value == 4)  {ledState[40] = 0; ledState[41] = 1; ledState[42] = 1;}
    }        
    case 37: {if(value){ledState[43] = 1;}else{ledState[43] = 0;}} break;
    case 38: {if(value){ledState[44] = 1;}else{ledState[44] = 0;}} break;
    case 39: {if(value){ledState[45] = 1;}else{ledState[45] = 0;}} break;
    case 40: {if(value){ledState[46] = 1;}else{ledState[46] = 0;}} break;
    case 41: {if(value){ledState[47] = 1;}else{ledState[47] = 0;}} break;           
    case 102:
    {
     if(value < 17) // Is MIDI Channel
     {
      channelOut = value;
      Serial.println("Martix Present");
      output(4, 102, 1);  // send back a one to indicate receipt.
      Serial.print("Channel Out: ");Serial.println(channelOut);     
     }
    }
    break;
  }
}
