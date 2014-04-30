# mu

An experiment in blurring the lines between music composition and sound synthesis

## todo
* Create a test file for each Stream class.
* Create a Mixer stream and a test file.
* Create a Crop stream and a test file.
* Create a Sequencer stream and a test file.
* Create a Delay stream (allow negative offsets) and a test file.
* Allow ticks to be negative (i.e. signed), validate operation with 
  negative tick times.
* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?
* Add command line parsing such as argp.h
* Investigate ways to do automated unit testing.  Create a make target.
* Create mu/Makefile to make all sub-projects
* Clean up test/Makefile to avoid repetition, 
* Fix test/Makefile to assure that mu library is up to date.
* Extend src/Makefile to assure that stk library is up to date.

## changelog 

* 2014-04-30: Refactoring: Node => Stream.  All subclasses are now
named XxxStream.  

* 2014-04-29: With the help of MapSteam and mune09, tested and
debugged Looper.  Looping is now clean.  

* 2014-04-29: Rename ValidatorStream=>MapStream, includes Player
argument so user-supplied function can stop playback (for example).
Needed to lose the const declaration on Player.  TODO: understand
const arg for member functions.

* 2014-04-29: step() now passes a frame counter (of type Tick) rather
than a time (type MuTime).  This permits sample-accurate calculations.
Node::streamDuration() returns value in frames rather than time, 
Looper::set/getLoopDuration is also in frames.  Renamed Player's
sampleRate to frameRate.  mune08 tests the chain of TestStream =>
ValidatorStream => NrtPlayer.

* 2014-04-29: Renamed Transport to Player, now a virtual superclass,
with sub-classes RtPlayer and NrtPlayer.  Flushed SampleBuffer since
C++ semantics won't let me overload a non-virtual library class.
Created Looper and test/mune08 to test it.  Glitchy, but starting to
work.  Created TestStream that outputs value N at time N -- not yet
tested.  Got everything working to "as well as it worked before" --
seems like a good checkpoint.

* 2014-04-28: Created SampleBuffer object that subclasses and mimics
stk::StkFrames, but creates an offset into the underlying data buffer.

* 2014-04-27: Removed the seek(), acquireResources() and
releaseResources() methods until I decide what they should do.

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

### keeping time

If the Transport maintains a current_time value and propagates it at
each call to step(), should current_time be a long int or a double
float?  If a long int, then it is in units of frames (i.e. samples).
If a double float, then it is in units of seconds.  Which is better?

Seconds may be more convenient since it is independent of sample rate,
but how many seconds can pass and still maintain single-sample
accuracy?  An IEEE 64 bit floating point number has a mantissa of 52
bits; it can represent integers accurately up to 2^52, or
4503599627370496.  Divided by sampling rate yields 102122440529
seconds, which is adequate for most musical compositions lasting under
3236 years...

### passing StkFrames buffers

Many Stk library objects are written assuming that the first sample is
written into &(buffer[0]).  In the mu world, there are many (?) cases
where we'd like a library object to write into a buffer at some offset
other than zero.

SampleBuffer is a subclass of StkFrames that delegates most of the
methods to an underlying StkFrames object, but adds a slice() method
that specifies an offset.  The accessor methods, [index] and (frame,
channel) call the underlying StkFrames object while honoring that
offset.

It may need additional work, for example, delegating all the methods
that make sense and raising an error on those that don't, but for now
it's enough to get started.

### signalling an empty stream

It's tempting to think that a stream should indicate that it has been
all used up and there are no more frames available.  But that assumes
purely sequential access.  In our current scheme, each call to step()
carries its own tick time and there's no guarantee that calls will be
sequential in time.  If a stream element wants to know (e.g. to stop
the transport), it can examine the stream's frameCount().
