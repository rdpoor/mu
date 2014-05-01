# mu

An experiment in blurring the lines between music composition and sound synthesis

## todo 

* Create ConstantStream and test file.
* Create MultiplyStream and test file.
* Create a stream that fiddles with time: t' = t0 + k*t.  (Oog -- am I
  going to regret using an integer frame counter?)
* Create F(t)Stream and test file.
* setSource() and related should check for circular loops.
* Think about mono to stereo (and stereo to mono) stream elements: pan.
* Think about automatic conversion from mono to stereo -- what happens
  when you connect a mono source to a stereo stream object?  Should that
  be an error or expand automatically?
* Abstract out common elements of a single-input stream, make into 
  a generic super class.
* Create a Sequencer stream and a test file.
* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?
* Add command line parsing such as argp.h
* Investigate ways to do automated unit testing.  Create a make target.
* Create mu/Makefile to make all sub-projects
* Clean up test/Makefile to avoid repetition, 
* Fix test/Makefile to assure that mu library is up to date.
* Extend src/Makefile to assure that stk library is up to date.
* Add __FILE__ and __LINE__ directives to ASSERT() macro, printed in a form
  emacs recognizes an an error message.

## changelog 

* 2014-05-01: Created MixNStream that takes an arbitrary number of inputs
to sum.  Need to write ut_mix_n_stream.cpp.

* 2014-05-01: Replaced Stream::frameCount() with Stream::getStart(),
Stream::getEnd() and Stream::getDuration(), all measured in Ticks.
Created CropStream to limit the extend of a source stream.  Created
unit tests (including getStart() and getStop() tests) for stream
classes: CropStream, DelayStream, IdentityStream, MixStream, Stream.

* 2014-04-30: Stream::streamDuration() => Stream::frameCount().  Created
MixStream and DelayStream and corresponding unit tests.  Created a unit
test for IdentityStream.  Created a simple test/assert.c file to help
with unit testing (could be better, but it's already useful).

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

### on stateful stream elements

Okay, so I claim that each call to step() can specify an arbitrary
time, so streams are effectively random access.  What happens when the
results of this buffer depends on what happened in the previous
buffer, with the obvious example of a reverberator?  Either we go back
to declaring that access is "usually" sequential and special case
non-sequential access, or we make some reasonable rules for how
stateful streams behave for sequential and non-sequential access.

The latter probably makes more sense.

### thought exercise

Compose a bit of music.  Figure out what pieces of the system are
missing and write them in order to make the music.  Don't worry about
making it clean -- cleanup can happen after lessons learned.

I can tell now that I'll need an extensible vector-like object.  C++
must have such a thing in its library.  Else I can create one easily
enough.

Towards a SequencerStream class: imagine I have a Stream object that
plays some motif (of any complexity) at time 0.  I want to be able to
say "play that motif at time A, B, and C" (Now that a stream has
distinct getStart() and getEnd() methods, it can start before time 0,
but use time 0 as a downbeat.  That should prove to be handy.)
