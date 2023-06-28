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
  - Click and Turn: Turn sync off/on
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
    - Conductor Mode: See more information below
    - Loop Mode: Play loop continuously, based on internal BPM or external MIDI sync
    - Song Mode: See more information below
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
- Swing:
  - Not implemented yet.
 

 ### Loop Mode
 Play notes set on screen in continuous loop
 - x-axis (horizontal) is time
 - y-axis (vertical) is pitch
 - Half-lit horizontal lines on screen are the 'black' notes on a paino.
Just tap on screen to set notes to play.
Fully polyphonic.  No limit, other than processing time (huge amounts of notes in parallel 'might' create audiable lag)
Adjust loop length and other parameters via the knobs.
- Long notes (tie):  Tap where you want the start of the note, and then drag your finger (without lifting your finger) to the end of where you want the note.  To shorten the note, just tap where you want it to end.
- Swipe up or down to see notes outside of screen view
- Use the zoom in/out knob to create shorter or longer notes
Note: Timing is quantized.  The start and end of notes are not based on time - but fractions of a beat - based on the MIDI divisions of 24/beat.  This means you can only zoom in so far. (12 steps-per-beat, as we need 1 step for on, and 1 step for off, which makes 24)

 
 ### Song Mode
 Play multiple saved loops in a sequence, that is then loops
 - x-axis (horizontal) is the sequence of loops
 - y-axis (vertical) is the saved loop number
 - Loop End Knob sets end point (loops at this point)
Just tap on screen to set the loop number for when it will play.  For instance: if you want loop pattern 2 to play first, then 1, then 3 and 3 - tap (from bottom left corner) Col 1/Row 2, Col 2/Row 1, Col 3/Row 3, Col 4/Row 3.  Then set the vertical bar on the last loop (col 4 in this case)


### Composer Mode
Allows you to play multiple MIDI instruments at once, using different fingers (this is not a keyboard split)
For instance, if you want your left finger of your left hand to play notes on say a bass synth, your middle finger to play another synth, 3 fingers from the left hand a pad on another synth, and the rest of your fingers another, you can.
How it works:
- Everytime a note is pressed, the program looks up to see which note held down is 1st, 2nd, 3rd and 4th - starting from the lowest note on the keyboard
- You assign these note 'fingers' (1,2,3,4) to a MIDI out.
- There is a delay of less than one step (1/24th of a beat), to allow for chords being played, and fingers not de-pressing at the same time - this way you don't get jumps if transposing a sequence on another machine
- You can also assign 'all' through.  This means all notes you play go to that synth.


 

### [Schematic and PCB files - KiCAD](PCB)

### [CODE - Arduino](CODE)

### [CAD - Solidworks](CAD)

### [Programing Touch](Azoteq_Touch)




## Issues
There are a few bugs and pooly written code.  See Code and PCB info files for known issues.
