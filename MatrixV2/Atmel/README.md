# Programing the microcontroller.

## If the microncontoller already has the Arduino bootloader on it:

(using Arduino IDE), plug board into computer and use for board:
- Adafruit ItsyBitsy M0 Express (SAMD21)

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
  - Happy Days.
