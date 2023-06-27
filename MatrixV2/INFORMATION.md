## PLEASE NOTE:
This is a low TRL device, and as such, please see known issues below.

## Overview
### Inputs and Outputs
- USB C to power and program
- 4x Midi TRS type A
  - Midi out to Synth
  - Midi in from Synth
  - Midi in from Computer/Master Clock
  - Midi out to computer/other
- 1x Trigger (3.5mm type) jack for pedal etc.

### Electronics
- Based on Adafruit ItsyBitsy MO Express https://www.adafruit.com/product/3727
- Processor: ATSAMD21 Cortex M0 processor running at 48 MHz
- External Memory: EEPROM 512k I2C
- Touch: 2x Azoteq multitouch QS550 I2C
- 24 pin I/O Expander: 2x PCAL6524HEAZ 
- Led Latch: 2x CAT4016HV6G
- Leds for display: APTR3216SECK and RGB Leds for indication: IN-PI55TATPRPGPB 
- Encoders, LDO power, Caps, Resistors etc

### Controls

Buttons 1-12.  
- Used for saving (long press) loop
- Loading loop or song based on mode you are in (tap new number)
- Mute all notes (tap the same loop number).
- Also used to see scale, by holding the transpose button, you can select which notes you want the tranpose and other function to keep your pattern to.
- Play Notes
Encoders and Encoder Buttons (working down in columns - from left)
- Play/BPM
  - Click:  Start/Stop
  - Turn:  BPM
- Record/Loop End
  - Click: Record notes played in via midi, over top of current loop
  - Turn:  Adjust loop end point
- Clear/Loop Start
  - Click: Clear all notes
  - Turn: Adjust loop start point
- Scroll/Zoom
  - Turn: Scroll screen left/right
  - Press and Turn: Zoom in/out (by amount in steps-per-beat)
- Mode:
  - Turn: Left=Conductor Mode, Middle=Loop Mode, Right=Song Mode
- Transpose:
  - Click: Active remote transpose.  This allows keyboard MIDI input to transpose note
  - Click and Hold: Set scale for tranpose, using number buttons (layed out as paino keys) or MIDI keyboard input.  
  - Turn: Transpose up/down
  - NOTE: Tranpose will try and shift notes onto the selected note only.
- Phase:
  - Click: Active phase.  This will phase shift the loop based on the number of times the loop has played
  - Click and Turn: Adjust the phase amount (negative or positive)
  - Turn: Manually phase shift notes
- Frequency
  - Turn: Change how fast the notes are played (*2 or /2 of current notes)
- Y-Mod Notes:
  - Turn: Reflect notes progressively about a defined axis
  - Click and Turn: Adjust where the reflection axis is (notes will be progressively reflected about this point)
- X-Mod Notes:
  - Turn: Reflect notes progressively about a defined axis
  - Click and Turn: Adjust where the reflection axis is (notes will be progressively reflected about this point)
- Mute LPF:
  - Click: Turn on.  All notes below threshold will play
  - Turn: Left sets threshold down in pitch.  Up goes up.
- Mute HPF:
  - Click: Turn on.  All notes above threshold will play
  - Turn: Left sets threshold down in pitch.  Up goes up.  
- Velocity:
  - Click: Use finger to draw note velocity
  - Turn: When playing, if you turn, this will record the velocity for one loop. 

## Issues

### SCHEMATIC

### PCB

### CAD

### CODE

