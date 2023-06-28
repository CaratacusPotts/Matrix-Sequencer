
void readEncoders(void)
{
   //Read all input pins -  see Fig 17 in datasheet
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x00);                       // command byte
  Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
  Wire.requestFrom(DEVICE_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
  
  while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
  {
    byte a = Wire.read();  
    byte b = Wire.read(); 
    byte c = Wire.read(); 
    byte n;
    n = a ^ enP1Last; if(n != 0){calcEncoder(1, n);} enP1Last = a;
    n = b ^ enP2Last; if(n != 0){calcEncoder(2, n);} enP2Last = b;
    n = c ^ enP3Last; if(n != 0){calcEncoder(3, n);} enP3Last = c;      
  }    
}



void portExpanderConfig()
{
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x0C);                       // command byte  CONFIG_REG_PORT   0x0C 
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 0 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 1 as input
  Wire.write(0xFF);                       // set pin 1,3,5,7 of port 2 as input
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 2) Configure all the pins as Pull UP/Down as on
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x4C);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);
  // 3) Configure all the pins as Pull UP.
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x50);                       // command byte  Enable Pull up
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.write(0xFF);                       // set on
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)
  delay(5);  
  // 4) Configure the interrupt triggering mode (i.e. interrupt on both edges) of all input pins
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x60);            // command byte  CONFIG_INT_EDGE   0x60
  Wire.write(0xAA);                       // set pin 0,1,2,3 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xAA);                       // set pin 4,5,6,7 of Port 1 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xAA);                       // set pin 0,1,2,3 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xAA);                       // set pin 4,5,6,7 of Port 2 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xAA);                       // set pin 0,1,2,3 of Port 3 to negative edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.write(0xFF);                       // set pin 4,5,6,7 of Port 3 to any edge AA=10101010  neg=10, pos= 01, high/low=00, any=11
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus) 
  delay(5);
  // 5) Configure the interrupt mask to enable all input pin's interrupt
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x54);                       // command byte  ENABLE_INT        0x54
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 0
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 1
  Wire.write(0x00);                       // enable interrupts of the pin 0,1,2,3,4,5,6,7 of port 2
  Wire.endTransmission(true);             // end with "STOP" condition (release the I2C bus)   
  delay(5);
}

void getCurrentEncoderState()
{
  Wire.beginTransmission(DEVICE_ADR);     // slave address
  Wire.write(0x58);                       // command byte
  Wire.endTransmission(false);            // end with "STOP" condition (release the I2C bus)
  Wire.requestFrom(DEVICE_ADR, 3);        // read the registers 0x00, 0x01 and 0x02 out
  
  while(Wire.available())  // retrive the port 0 status first, and then port 2 and 3 
  {
    byte n;
    n = Wire.read();  //Serial.print(n,BIN);Serial.print(" ");              
    enP1Last = n;
    n = Wire.read();  //Serial.print(n,BIN);Serial.print(" ");
    enP2Last = n;
    n = Wire.read();  //Serial.print(n,BIN);Serial.print(" "); 
    enP3Last = n; 
  }  
}
void readSerial()
{
    byte b = Serial.available();
    char a[b];
    Serial.readBytes(a,b);
    byte cc = ((a[0]-48)*10) + (a[1]-48);
    byte num = ((a[2]-48)*10) + (a[3]-48);

    ccIn(1, cc, num);
}
