# mu

An experiment in blurring the lines between music composition and sound synthesis

## todo 

* FileReadStream should allow negative Tick times
* Create a stream that fiddles with time: t' = t0 + k*t.  (Oog -- am I
  going to regret using an integer frame counter?)
* Create F(t)Stream and test file.
* setSource() and related should check for compatible frameRate(), channelCount()
* setSource() and related should check for circular loops.
* Think about mono to stereo (and stereo to mono) stream elements: pan.
* Think about automatic conversion from mono to stereo -- what happens
  when you connect a mono source to a stereo stream object?  Should that
  be an error or expand automatically?
* Abstract out common elements of a single-input stream, make into 
  a generic super class.
* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?
* Add command line parsing such as argp.h
* Investigate ways to do automated unit testing.  Create a make target.
* Create mu/Makefile to make all sub-projects
* Clean up test/Makefile to avoid repetition, 
* Fix test/Makefile to assure that mu library is up to date.
* Extend src/Makefile to assure that stk library is up to date.

## changelog 

* 2014-05-03: Created mune11, mune12 to play arpeggiated chords.
Created mune13 to play a melody.
* 2014-05-02: Beefed up ASSERT macro to report filename and line
number so tests can be run in an emacs compile window.
* 2014-05-01: Created MixNStream that takes an arbitrary number of inputs
to sum.  Created ut_mix_n_stream and debugged class.  Wrote a fun little
musical bit mune10, reminiscent of Steve Reich's Violin Phase.  Factored
MultiStream out of MixNStream to provide generalized N-source stream
support.  Deleted MixStream.  Renamed MixNStream to AddStream.  Created
MultiplyStream.

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

### Thought exercise

Compose a bit of music.  Figure out what pieces of the system are
missing and write them in order to make the music.  Don't worry about
making it clean -- cleanup can happen after lessons learned.

I can tell now that I'll need an extensible vector-like object.  C++
must have such a thing in its library.  Else I can create one easily
enough.

Towards a SequenceStream class: imagine I have a Stream object that
plays some motif (of any complexity) at time 0.  I want to be able to
say "play that motif at time A, B, and C" (Now that a stream has
distinct getStart() and getEnd() methods, it can start before time 0,
but use time 0 as a downbeat.  That should prove to be handy.)

On that topic, a FileReadStream object could carry an offset parameter
that defines the "downbeat" of a sound file, assuming it doesn't
happen exactly at 0.  This is somewhat analagous to the baseline value
for a font.

### Thoughts on a SequenceStream

A sequence stream takes N sources, where each stream is offset by a
different time, and mixes them together.  We could choose to abut
streams (not start one until another has finished) so we're only
playing one stream at a time, but we shouldn't go that route until
computation time becomes an issue.

A SequenceStream could be quickly built out of existing stream
objects: a DelayStream for each input and a single MixNStream.  That
might be a good exercise in figuring out how to reuse components.

### Stateless vs Stateful Streams

A stateless stream is one that will always produce the same output
when given the same Tick, regardless of order.

A stateful stream may produce different output when given the same
tick, e.g. it may not be possible to "back up" a Reverberator, as its
output depends on previous input.  It's worth pointing out that any
stream object has acess to the Player, so it can discover the global
time at each call to step() and change its behavior accordingly  This
is another way a node could become stateful.

The distinction between stateful and stateless streams may be useful:
a stateless stream can be used as input to an arbitrary number of
downstream streams.  Stateful streams may only be used as an input
to a single downstream stream.

This makes me thing that we should define a Patch or Wire class that
connects the output of one Stream object to the input of another.
This way, a single output could connect to mutliple inputs.  And
presumably, the act of patching would verify whether or not the
upstream object is stateful or not, and warn if its output is being
connected to more than one input.

### Thoughts on a strummed instrument

A strummed instrument has N strings.  Each string has an input that
dictates a fret number (or damped).  The string is responsible for
mapping fret number into an appropriate sound file or synthesis
technique.  Doing a step(b, tick, player) gets the pitch information
from the input for the given tick time.  

Hmm...this means that the string can only ever play one note in its
lifetime, that is, the tick given to the string is the same tick given
to the pitch source.  Is that okay?  What does it mean to play a
sequence of notes?  If we create a different string object for each
note, it may feel like a lot more work but it has the advantage that
everything is entirely deterministic (and you can dependibly rewind to
a specific point).  [NOTE: this determinism does not hold for stream
objects that preserve state from one step() to the next.  We'll deal
with those later.]

Stated another way, a stream can really only play one thing.  It can
be repeated, it can be shifted in time, but since the only external
state passed to it is the current tick, everything it plays is a 
function of the current tick.

### mune11

Created a simple arpeggiated chord.  This required:

* 4 file_read_stream (for the sound sources)
* 4 crop_stream (because file_read_stream can't tolerate negative tick times)
* 4 delay_stream (to offset the start times)
* 1 add_stream (to sum them together)
* 1 loop_stream (to make it interesting to listen do)

Once stream elements are connected together, you have to pay attention
to the order of setting things up.  In this case, you can't find the
end time of a delay_stream until you've set the file name for the
file_read_stream, since the former depends upon the latter.

Perhaps a reasonable discipline is to (1) connect stream element to
its sub-tree then (2) configure that stream element, starting with the
leaf nodes and working towards the root.

### mune12

Created a relatively simple ArpeggiateStream class that encapsulates
all the pieces in mune11.  Using it is as simple as:

      arpeggiate_stream.setArpeggioDelay(882);
      arpeggiate_stream.addFile(SOUND_DIR SOUND_0 ".wav");
      arpeggiate_stream.addFile(SOUND_DIR SOUND_1 ".wav");
      arpeggiate_stream.addFile(SOUND_DIR SOUND_2 ".wav");
      arpeggiate_stream.addFile(SOUND_DIR SOUND_3 ".wav");
      loop_stream.setSource(&arpeggiate_stream);
      loop_stream.setLoopDuration(arpeggiate_stream.getEnd()/2);
      player.setSource(&loop_stream);
      player.start();
      sleep(30);
      player.stop();

### Half-baked idea on deterministic vs non-deterministic streams

So far, we've made a big deal about how streams are deterministic: if
you call step() for a given tick, you always get the same results.
But we really don't always want that.  Sometimes, for example, you
want a note to sound slightly different each time you play it. So
maybe there's some concept of "start a note" which allows a stream to
select some parameters that remain invariant until the note ends.

