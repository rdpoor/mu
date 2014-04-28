# mu

An experiment in blurring the lines between music composition and sound synthesis

## todo
* Create a Loop node.
* Create a Mixer node.
* Can we ditch the seek() method now that step() carries time
  information?
* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?
* Add command line parsing such as argp.h
* Clean up test/Makefile to avoid repetition, 
* Fix test/Makefile to assure that mu library is up to date.
* Extend src/Makefile to assure that stk library is up to date.

## changelog 

* 2014-04-27: Created a FileReader subclass of Node that provides a
step() interface to a sound file.  Needs more error checking, and we
need to decide if and when to close the file, but it works.

* 2014-04-27: Refactored step() to take different arguments:
stk::StkFrames buffer (as before), MuTime time (current time at start
of buffer), Transport& transport (so we can discover who is at the
head of the graph, etc).  Ditch Node::framesRemaining() in favor of
Node::duration().

* 2014-04-27: Split monolithic mune04 file into src and include
library directories, created test directory for test executables.
Created src/Makefile to remake library, test/Makefile to remake test
files.

* 2014-04-26: Rename project to mu.  Put changelog in reverse
  chronologial order.  Tested pushing to new repo.

* 2014-04-25: mune04 defines a Transport object that allocates and
controls an RtAudio object, and interfaces its callback method to the
step() method of a generalized Node class.  The TestNode class simply
writes a pulse train into the buffers passed to it.

* 2014-04-17: mune03 reads a sound file via FileRead and streams it a
buffer at a time to the DAC via RtAudio.  There's an outstanding bug:
it will stop playback after the last buffer is queued, not after the
last buffer is played.  But this part of the code will change in the
next version, so it's not not worth fixing today.

* 2014-04-15: mune02 is a tweaked copy of stk's examples/bethree.cpp to
verify linking against the STK library.  Plays a note.  Yay.

* 2014-04-14: Simple "play a sine tone in each channel" real-time app.
Mostly written as a "hello, world" test to exercise RtAudio and
Makefile structure.

## design notes

If the Transport maintains a current_time value and propagates it at
each call to step(), should current_time be a long int or a double
float?  If a long int, then it is in units of frames (i.e. samples).
If a double float, then it is in units of seconds.  Which is better?

Seconds may be more convenient since it is independent of sample rate,
but how many seconds can pass and still maintain single-sample
accuracy?  An IEEE 64 bit floating point number has a mantissa of
52 bits; it can represent integers accurately up to 2^52, or
4503599627370496.  Divided by sampling rate yields 102122440529
seconds, which is adequate for most musical compositions lasting
under 3236 years...

