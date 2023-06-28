    void readEncoders(int expander)   //Read all input pins -  see Fig 17 in datasheet
{
  const byte enList[] ={0,3,6,9,12,1,4,7,10,13,2,5,8,11,14};
  
  byte b[4]; 
  if(expander == 1)
  {
    Wire.beginTransmission(Expander1_ADR);     // slave address
    Wire.write(0x00);                       // command byte
    Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
    Wire.requestFrom(Expander1_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
    while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
    {
      b[0] = Wire.read(); b[1] = Wire.read(); b[2] = Wire.read();
      for(int y=0; y<3; y++)
      {
        for(int i=0; i<4; i++)
        {
          int p = i*2;
          int state = bitRead(b[y],p) + bitRead(b[y],p+1);
          int lastState = bitRead(en1Last[y],p) + bitRead(en1Last[y],p+1);
          int en = enList[(y*4) + i];
          if(state == 2 && lastState == 1) 
            {byte value = (en*4) + bitRead(en1Last[y],p+1); ccIn(channelIn, ccNum, value);}
          if(state == 0 && lastState == 1)
            {byte value = (en*4) + bitRead(en1Last[y],p); ccIn(channelIn, ccNum, value);}
        }
        en1Last[y] = b[y];
      }
    }  
  }
  else
  {
    Wire.beginTransmission(Expander2_ADR);     // slave address
    Wire.write(0x00);                       // command byte
    Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
    Wire.requestFrom(Expander2_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
    while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
    {
      b[0] = Wire.read(); b[1] = Wire.read(); b[2] = Wire.read();
      //Serial.print(b[0],BIN);Serial.print(" ");  Serial.print(b[1],BIN);Serial.print(" ");  Serial.print(b[2],BIN);Serial.println(" ");  
      for(int i=0; i<3; i++)
      {
        int p = i*2;
        int state = bitRead(b[0],p) + bitRead(b[0],p+1);
        int lastState = bitRead(en2Last[0],p) + bitRead(en2Last[0],p+1);
        int en = enList[12+i];
        if(state == 2 && lastState == 1) 
          {byte value = (en*4) + bitRead(en2Last[0],p+1); ccIn(channelIn, ccNum, value);}
        if(state == 0 && lastState == 1) 
          {byte value = (en*4) + bitRead(en2Last[0],p); ccIn(channelIn, ccNum, value);} 
      }
      if( bitRead(b[0],6) != bitRead(en2Last[0],6) ) {ccIn(channelIn, ccNum, 2+bitRead(b[0],6));}  //0
      if( bitRead(b[0],7) != bitRead(en2Last[0],7) ) {ccIn(channelIn, ccNum, 14+bitRead(b[0],7));} //3
      en2Last[0] = b[0];
      
      if(b[1] != en2Last[1])
      {
        if( bitRead(b[1],0) != bitRead(en2Last[1],0) ) {ccIn(channelIn, ccNum, 26+bitRead(b[1],0));} //6
        if( bitRead(b[1],1) != bitRead(en2Last[1],1) ) {ccIn(channelIn, ccNum, 38+bitRead(b[1],1));}   // 9    
        if( bitRead(b[1],2) != bitRead(en2Last[1],2) ) {ccIn(channelIn, ccNum, 50+bitRead(b[1],2));} //12
        if( bitRead(b[1],3) != bitRead(en2Last[1],3) ) {ccIn(channelIn, ccNum, 6+bitRead(b[1],3));}   //1
        if( bitRead(b[1],4) != bitRead(en2Last[1],4) ) {ccIn(channelIn, ccNum, 18+bitRead(b[1],4));}  //4
        if( bitRead(b[1],5) != bitRead(en2Last[1],5) ) {ccIn(channelIn, ccNum, 30+bitRead(b[1],5));}   // 7
        if( bitRead(b[1],6) != bitRead(en2Last[1],6) ) {ccIn(channelIn, ccNum, 42+bitRead(b[1],6));} //10
        if( bitRead(b[1],7) != bitRead(en2Last[1],7) ) {ccIn(channelIn, ccNum, 54+bitRead(b[1],7));}    //13
      }
      en2Last[1] = b[1];
      if(b[2] != en2Last[2])
      {
        if( bitRead(b[2],0) != bitRead(en2Last[2],0) ) {ccIn(channelIn, ccNum, 10+bitRead(b[2],0));} // 2
        if( bitRead(b[2],1) != bitRead(en2Last[2],1) ) {ccIn(channelIn, ccNum, 22+bitRead(b[2],1));}  // 5     
        if( bitRead(b[2],2) != bitRead(en2Last[2],2) ) {ccIn(channelIn, ccNum, 34+bitRead(b[2],2));} //8
        if( bitRead(b[2],3) != bitRead(en2Last[2],3) ) {ccIn(channelIn, ccNum, 46+bitRead(b[2],3));}   // 11
        if( bitRead(b[2],4) != bitRead(en2Last[2],4) ) {ccIn(channelIn, ccNum, 58+bitRead(b[2],4));}  // 14
        if( bitRead(b[2],5) != bitRead(en2Last[2],5) ) {ccIn(channelIn, ccNum, 60+bitRead(b[2],5));}   
        if( bitRead(b[2],6) != bitRead(en2Last[2],6) ) {ccIn(channelIn, ccNum, 64+bitRead(b[2],6));}
        if( bitRead(b[2],7) != bitRead(en2Last[2],7) ) {ccIn(channelIn, ccNum, 68+bitRead(b[2],7));}            
      }
      en2Last[2] = b[2];   
    }  
  } 
}


void portExpanderConfig()
{
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x0C);                       // command byte  CONFIG_REG_PORT   0x0C 
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 0 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 1 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 2 as input
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 2) Configure all the pins as Pull UP/Down as on
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x4C);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 3) Configure all the pins as Pull UP.
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x50);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);  
  // 4) Configure the interrupt triggering mode (i.e. interrupt on both edges) of all input pins
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x60);            // command byte  CONFIG_INT_EDGE   0x60
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 3 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 3 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus) 
  delay(5);
  // 5) Configure the interrupt mask to enable all input pin's interrupt
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x54);                       // command byte  ENABLE_INT        0x54
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 0
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 1
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 2
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)   
  delay(5);

  // EXPANDER No 2
  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x0C);                       // command byte  CONFIG_REG_PORT   0x0C 
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 0 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 1 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 2 as input
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 2) Configure all the pins as Pull UP/Down as on
  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x4C);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 3) Configure all the pins as Pull UP.
  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x50);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);  
  // 4) Configure the interrupt triggering mode (i.e. interrupt on both edges) of all input pins
  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x60);            // command byte  CONFIG_INT_EDGE   0x60
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 0,1,2,3 of Port 3 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 3 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus) 
  delay(5);
  // 5) Configure the interrupt mask to enable all input pin's interrupt
  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x54);                       // command byte  ENABLE_INT        0x54
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 0
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 1
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 2
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)   
  delay(5);  
}

void getCurrentEncoderState()
{
  Wire.beginTransmission(Expander1_ADR);     // slave address
  Wire.write(0x00);                       // command byte
  Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
  Wire.requestFrom(Expander1_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
  
  while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
  {
    en1Last[0] = Wire.read();  //Serial.print(en1Last[0],BIN);Serial.print(" ");              
    en1Last[1] = Wire.read();  //Serial.print(en1Last[0],BIN);Serial.print(" ");
    en1Last[2] = Wire.read();  //Serial.print(en1Last[0],BIN);Serial.print(" "); 
  }  

  Wire.beginTransmission(Expander2_ADR);     // slave address
  Wire.write(0x00);                       // command byte
  Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
  Wire.requestFrom(Expander2_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
  
  while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
  {
    en2Last[0] = Wire.read();  //Serial.print(en2Last[0],BIN);Serial.print(" ");              
    en2Last[1] = Wire.read();  //Serial.print(en2Last[1],BIN);Serial.print(" ");
    en2Last[2] = Wire.read();  //Serial.print(en2Last[2],BIN);Serial.println(" "); 
  }  
}
