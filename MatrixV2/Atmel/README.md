# Programing the microcontroller.

## If the microncontoller already has the Arduino bootloader on it:

(using Arduino IDE), plug board into computer and use for board:
- Adafruit ItsyBitsy M0 Express (SAMD21)
- Don't forget to install libraries (sorry - I should get rid of libraries, as having them as dependencies sucks):
  - MIDI (https://github.com/FortySevenEffects/arduino_midi_library)
  - Wire (standard Arduino library)
  - FastLED for single wire LEDS (https://github.com/FastLED/FastLED)

## If you need to put the bootloader on to the microcontroller (new/fresh microcontroller)

Using MICROCHIP STUDIO software:
- Purchase a: ATATMEL-ICE-PCBA (its the cheapest I could find) Farnell: 2407171
- Connect to the following wires (see 'Connecting to an SWD Target' on page 22 in the ATMEL ICE datasheet for the ICE pins)
  - SWDIO
  - SWCLK
  - RESET
  - GND
  - 3.3V
- Open and go to tools - Device Programming
- Follow this guide: https://learn.adafruit.com/how-to-program-samd-bootloaders/programming-the-bootloader-with-atmel-studio
- I have put the adafruit Itsybitsy bootloader in this directory, if you need it
  - Happy Days.


Further Information:
- To add or shift the serial ports to other pins see:
  - https://learn.adafruit.com/using-atsamd21-sercom-to-add-more-spi-i2c-serial-ports/creating-a-new-serial
  - https://docs.arduino.cc/tutorials/communication/SamdSercom
- https://docs.arduino.cc/hardware/mkr-zero
- Bootloader https://github.com/adafruit/uf2-samdx1/releases
- Programing with bootloaders https://learn.adafruit.com/how-to-program-samd-bootloaders/programming-the-bootloader-with-atmel-studio
