# mune

An experiment in blurring the lines between music composition and sound synthesis

## todo
* Add command line parsing such as argp.h
* Fix dependencies in makefile (always makes the target)
* "feeder" process should not busy wait; it should block instead.
(This implies that the feeder process should be in its own thread.)

## changelog 

* 2014-04-14: Simple "play a sine tone in each channel" real-time app.
Mostly written as a "hello, world" test to exercise RtAudio and
Makefile structure.

* 2014-04-15: mune02 is a tweaked copy of stk's examples/bethree.cpp to
verify linking against the STK library.  Plays a note.  Yay.

* 2014-04-17: mune03 reads a sound file via FileRead and streams it a
buffer at a time to the DAC via RtAudio.  There's an outstanding bug:
it will stop playback after the last buffer is queued, not after the
last buffer is played.  But this part of the code will change in the
next version, so it's not not worth fixing today.