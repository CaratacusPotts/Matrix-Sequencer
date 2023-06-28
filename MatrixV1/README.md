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
- Touch: 4x Azoteq multitouch QS550 I2C
- Led Latch: 3x CAT4016HV6G
- Leds for display: APTR3216SECK
- Encoders, LDO power, Caps, Resistors etc

### Controls (from left to right)

- Play
- Mute notes
- Record notes played in via midi, over top of current loop
- Loop Mode: Play loop continuously, based on internal BPM or external MIDI sync (see more info below)
  - If in pattern mode, tapping this will enter loop mode
  - If already in loop mode, this will allow you to select a loop (loop no:1 is bottom row, loop no:2 second row from bottom, and so on)
- Song Mode (see more info below)
  - Set loops to play in sequence  
- Velocity: Adjust note velocity by using finger to draw note velocity
- Swing (not yet implemented
- Steps Per Beat.  Zoom in/out by amount in steps-per-beat (max 12 steps per beat)
- Loop Start and End points
- Transpose up/down is on (will react to keyboard input)
- Phase left/right on (will phase notes by amount set)
- Shift Notes: Transpose or phase notes by moving finger
- BPM (double tap to set to external MIDI or internal sync)
- Save: Tap the 'save' icon, and select the row to save the pattern
- Settings:
  - Tap this , then select the icon to which you want to modify the settings.
    - Play: MIDI in/out channels
    - Transpose: Notes to transpose onto (so you don't have to remember a scale name)
    - Phase: Phase direction and amount (negative number is reverse)
- Clear: Clear all notes 
 

 ### Loop Mode
 Play notes set on screen in continuous loop
 - x-axis (horizontal) is time
 - y-axis (vertical) is pitch
 - Half-lit horizontal lines on screen are the 'black' notes on a paino.
Just tap on screen to set notes to play.
Fully polyphonic.  No limit, other than processing time (huge amounts of notes in parallel 'might' create audiable lag)
Adjust loop length and other parameters via the icons.
- Long notes (tie):  Tap where you want the start of the note, and then drag your finger (without lifting your finger) to the end of where you want the note.  To shorten the note, just tap where you want it to end.
- Swipe up or down to see notes outside of screen view
Note: Timing is quantized.  The start and end of notes are not based on time - but fractions of a beat - based on the MIDI divisions of 24/beat.  This means you can only zoom in so far. (12 steps-per-beat, as we need 1 step for on, and 1 step for off, which makes 24)

 
 ### Song Mode
 Play multiple saved loops in a sequence, that is then loops
 - x-axis (horizontal) is the sequence of loops
 - y-axis (vertical) is the saved loop number
 - Loop End icon sets end point (loops at this point)
Just tap on screen to set the loop number for when it will play.  For instance: if you want loop pattern 2 to play first, then 1, then 3 and 3 - tap (from bottom left corner) Col 1/Row 2, Col 2/Row 1, Col 3/Row 3, Col 4/Row 3.  Then set the vertical bar on the last loop (col 4 in this case)


### Composer Mode
Allows you to play multiple MIDI instruments at once, using different fingers (this is not a keyboard split)
Access, by pressing settings, then Play Icon.
For instance, if you want your left finger of your left hand to play notes on say a bass synth, your middle finger to play another synth, 3 fingers from the left hand a pad on another synth, and the rest of your fingers another, you can.
How it works:
- Everytime a note is pressed, the program looks up to see which note held down is 1st, 2nd, 3rd and 4th - starting from the lowest note on the keyboard
- You assign these note 'fingers' (1,2,3,4) to a MIDI out.
- There is a delay of less than one step (1/24th of a beat), to allow for chords being played, and fingers not de-pressing at the same time - this way you don't get jumps if transposing a sequence on another machine
- You can also assign 'all' through.  This means all notes you play go to that synth.


 

### [Schematic and PCB files - EASY EDA](PCB)

### [CODE - Arduino](CODE)

### [CAD - Solidworks](CAD)

### [Programing Touch](Azoteq_Touch)




## Issues
There are a few bugs and pooly written code.  See Code and PCB info files for known issues.
Because the capacative grid is on the top layer, the sensing does not work without a layer of some dielectric.  I used clear coat enamel spray paint.  You'll need to put 6-10 thick coats on - to get to a minimum of 0.5mm.  Ideally more.
