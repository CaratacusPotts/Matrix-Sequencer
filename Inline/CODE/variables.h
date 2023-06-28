#include <Arduino.h>                  // required before wiring_private.h
#include "wiring_private.h"           // pinPeripheral() function
//Uart Serial2 (&sercom2, 12, 11, SERCOM_RX_PAD_1, UART_TX_PAD_0);  //TX on D4 (SERCOM2.0) and RX on D3 (SERCOM2.1)  3to12 4to11
//void SERCOM2_Handler(){Serial2.IrqHandler();}
// For above, see: https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
// https://docs.arduino.cc/tutorials/communication/SamdSercom

//C:\Users\Carasquid\AppData\Local\Arduino15\packages\adafruit\hardware\samd\1.7.11\variants\itsybitsy_m0
// Bootloader https://github.com/adafruit/uf2-samdx1/releases
// https://learn.adafruit.com/how-to-program-samd-bootloaders/programming-the-bootloader-with-atmel-studio

#include <MIDI.h>
#include <Wire.h> 

//#include "DFRobot_RGBLCD1602.h"
//DFRobot_RGBLCD1602 lcd(/*lcdCols*/16,/*lcdRows*/2);  //16 characters and 2 lines of show

// EXPANDER
#define DEVICE_ADR 0x22    // set it according to your IO expander device address
#define INT_STATUS        0x58


// MODE
byte ledColor = 1;   // 0 Off, 1=Red, 2=Blue, 3=(Red/Blue)Purple, 4=Green, 5=(Red/Green)Mustard, 6=Green/Blue(Cyan), 7=Brown(RGB)
byte channelOut = 1;

// TIMING
unsigned long keyTimeLast;                              // check the touch and input panels
unsigned long ccOutputLast[127];


// LED
#define latchPin 10                      // LED array Latch (when active, enables led switch)
#define clockPin 13                      // LED array Clock (moves the data over to the switches one bit, one pulse at a time)
#define dataPin 11                       // LED array Data (the data line, that is sent to the switches)
bool ledState[48];


// BUTTONS
bool buttonState[22];
byte enP1Last = 0xFF, enP2Last = 0xFF, enP3Last = 0xFF;
