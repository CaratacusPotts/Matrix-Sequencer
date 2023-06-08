# Matrix-Sequencer
**Stand Alone Midi Sequencer**, for immediate visual interaction with hardware synthesizers

## What
Midi sequencer designed to be easy to modify for beginners, and low cost to make.

Operates in:
- Loop Mode
- Song Mode
- Conductor Mode

![laylout](images/misc/Functions.jpg)

### Basic Functions:
- Start/Stop: Play using internal clock or external midi clock
- BPM change using encoder
- Loop Start and End points via encoder
- Record:
  - Overtop: using Encoder button
  - On-Play: holding foot pedal, recording starts when first note played. Recording stops with foot off pedal. Automatically cuts loop to nearest beat, so loops nice.
- Clear notes (via encoder button)

### Canon Functions:
- Transpose (to scale, on/off beat, via encoder or key input)
- Phase (by multiples of loop, by amount in steps)
- Frequency (compress note sequence in time, in fractions of current loop length)
- Mirror Notes Pitch (with a user defined mirror plane, reflect notes in pitch axis)
- Mirror Notes Loop (with a user defined mirror plane, reflect notes in time)
- Note Cutofff Filter (mute notes like a LPF or HPF)
- Note Velocity (use finger to draw note velocity or encoder)
- Note Duration (use encoder to increase or decrease all note lengths)

### Additional Features:
- 256 events (not bound by bars or specific steps)
- Polyphonic
- Intuitive scales

## Why
### Hardware synthesizer problem:

![laylout](images/misc/img16.png)

Electronic music possesses both its greatest asset and its greatest drawback: the unparalleled flexibility it offers in sound creation. It's simple to purchase a laptop, load your favourite DAW software, and then to wake up, after you have nodded off to a loop running endlessly in your headphones. Perhaps you may get lucky, and a fortuitous accidental mouse click during your slumber may result in a masterpiece. However, it is more probable that your music will end up sounding sterile.

Hardware synthesizers currently provide immediacy, tangibility, reliability and lack of spontaneous OS updates, that PC's cannot provide.  But without expensive hardware, the only way to quickly layer sounds is to use built in key-holds playing standard arpeggios that steadily erode their beauty through endless repetition of the same up/down sequences.

Goal was to:
- make an interface that allows for quick note controller access, with immediate visual and tactile feedback - with a nod to the Reason Matrix
- Keep cost low.  There are solutions already that do similar things, but start at £650 upwards. 
- Enhance workflow and notation in areas that typically do not get attention, that have a meaningful impact of the quality of music, such as instant: 
  - Transposing in key
  - Phasing
  - High-pass and Low-pass velocity muting
  - Canon functions in key (Crab, Inversion, Reversing etc)
  - Hardware agnostic 303 style accents and glides
  - Velocity and length control
- Composer mode, allowing the user to play a midi keyboard, and to be able to select which fingers play which synthesizer (for example: left pinky plays bass, middle finger lead, first 3 fingers strings etc)
- Record mode, where it starts the recording only when you play, and stops the recording and automatically trims length when you remove you foot.  This allows seemless live looping.
- Loops that are not bound by 16,32 or 64 steps.  But is only bound by number of events.  In the current state, this is 255 events, over a 32bit length of steps (which at low tempo could be a 10 min loop)
- Scale not chosen by archaic and unrelatable names.  Notes (key) chosen by just selecting which white and black note you want it to stay on - yes, that simple.  (Does not currently handle option to change base notes if going up or down like in a some melodic minor scales - to be added) 
- Open source and easy to modify for non-programmers to allow people to add features and tweak to their setup.  I really tried to avoid the use of pointers, classes, libraries etc, as I wanted this to be software that anyone could modify - worts and all (I failed on the Midi Library - need remove this at some point)
- Fit into existing setups without changing anything (inline).
- Assemble at home with basic SMD tools, and a 3D printer.

## Where

## Who


## When

## How
## Photos

![img04](images/img04.jpg)
![img03](images/img03.jpg)
![img05](images/img05.jpg)
![img02](images/img02.jpg)

[![Version1 Sequencer](https://vimeo.com/770474252)](https://vimeo.com/770474252)

# Other Verions

## First Verison
![img12](images/img12.jpg)
![img13](images/img13.jpg)
![img15](images/img15.jpg)
![img14](images/img14.jpg)



## Inline Midi 
![img08](images/img08.jpg)
![img11](images/img11.jpg)
![img06](images/img06.jpg)
![img07](images/img07.jpg)
![img09](images/img09.jpg)
