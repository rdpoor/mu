# mu

An experiment in blurring the lines between music composition and
sound synthesis

## todo 

* Sprint: create framework for a Redis interface.  See
  https://github.com/redis/hiredis and
  https://github.com/luca3m/redis3m

* Adopt a unit test library (and coverage tests)

* create EventStream (or briefly ES) with first() and next() methods.

* scores/tnvm/percussion.cpp (and mune21) show a technique for
  generating percussion parts with variation.  Another (perhaps more
  satisfying) approach would be to have a fixed pattern, but randomize
  the dynamics, e.g. with a LinsegStream or StepStream.

* Many stream elements -- e.g. MultiplyStream, SineStream -- would
  benefit from a constant input and a stream input.  (Look for examples
  that have used ConstantStream.)

* Slide guitar

* Add has_errors and get_errors methods to Stream objects.

* Add doxygen comments

* Add 'make docs' to Makefile

* Make it easy to run unit tests with libgmalloc turned on.  

* Write unit test for FileWriteStream.

* Stop Player when tick >= source.getEnd().  Clean up implementations.

* Optimize a few inner loops (copy buffer, zero part of buffer...)

* Can we (should we?) change Tick to Seconds?

* Beef up assert.c=>assert.cpp Create a tester object that can print
  out context, print on error only, print always, etc.  Better, find
  an existing test package.

* Create F(t)Stream and test file.

* setSource() and related should check for compatible frameRate(), channelCount()

* setSource() and related should check for circular loops.

* Write a generalized graph traversal method for inspect and loop detection.

* Think about automatic conversion from mono to stereo -- what happens
  when you connect a mono source to a stereo stream object?  Should that
  be an error or expand automatically?

* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?

* Add command line parsing such as argp.h

* Create mechanisms for panning and localization.

* Create a fuzzbox

* Create a morley pedal

* Experiment in adding 6th harmonic to make a metal string out of nylon

* State-variable filter with stream control inputs
  
## changelog 

* 2014-06-09: Partway though major refactor: "make install" installs
library files and headers in ./usr/lib and ./usr/include
(respectively), eventually will be able to rebuild everything from
scratch.  tests, examples, sketches each have their own directory (and
makefile).
* 2014-05-17: Rename: XFadeStream => FadeStream. Stream::copy_buffer =>
Stream::copyBuffer.  Stream::zero_buffer => Stream::ZeroBuffer.
* 2014-05-17: wrote and tested XFadeStream.  Need a sound example
that uses it.
* 2014-05-15: cleaned up SpliceStream (i.e. fixed it).  mune23 is a
sketch for "strumming" using SpliceStream.  It works, but there's a
lot of crackling due to non-zero splice points.  Need to work on a
cross-fade version.
* 2014-05-14: Wrote inspect() method for streams.  mune23 uses
SpliceStream, to emulate strumming, but there's still a problem with
damping a string.  Use inspect() to debug.
* Wrote SpliceStreams that acts like SequenceStream, but stops output
of the previous stream when the next stream starts.
* 2014-05-09: test/test_rt_player runs a bare-bones RtPlayer in order
to look for memory leaks.  Ran it for 6 hours.  None detected.
* 2014-05-08: modified mune20 run run in non-real-time (i.e. as fast
as possible) to look for memory leaks.  Ran for 30 minutes of CPU time,
no leaks detected.
* 2014-05-08: mune21 generatess output identical to mune20, but using
library defined mu::ProbabilityStream
* 2014-05-08: mune20 assigns random probability of playing a source
stream at each b/n beats, where b is beats per measure and n is the
number of elements in the probability vector.  Starting to sound fun.
Found and stomped two bugs in LoopStream, not shadowing getStart() and
getEnd() and not honoring null source stream.  
* 2014-05-06: Clean up test/Makefile: any muneXX.cpp is an integration
  test, ut_xxx.cpp is a unit test.
* 2014-05-06: Created mune19 to test fade in/out using LinsegStream
and MultiplyStream.
* 2014-05-05: Created mune18 which connects to stk::PitShift (or
LentPitShift) and does real-time pitch shifting.
* 2014-05-05: Created mune17, identical to mune12 to play an
arpeggiated chord, but using SequenceStream.  Very short.
* 2014-05-05: Created LinsegStream that generates interpolated line
segments as a function of tick time.
* 2014-05-04: Created library class RandomSelectStream that does
exactly what mune14 does: when tick counter decreases, randomly
chooses an input stream to start playing.  Created mune15 based on the
library function.  Created SequenceStream, subclass of AddStream, that
interposes a DelayStream on each input.  Replicated mune13; plays
identical melody, but (by design) doesn't crop each note to stop when
the next one starts (though that would be a possible extention to the
design of SequenceStream).
* 2014-05-04: Created mune14: when tick counter decreases (e.g. at a
loop point), randomly choose an input stream to start playing.
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

### Varying notes on each playback

I woke up this AM and realized there's a simple heuristic for making
changes to a stream at the start of each note: if the tick counter
decreases, assume that you are starting a "new" note.  I wrote mune14
to test this: at each loop, pick a different sound to play.  Works
great.  Going this route, we need to go back to the idea of "one
source per input", that is, can't share streams across multiple sinks.

mune15 does the same thing as mune14 using the new RandomSelectPlayer.

### a peculiar thought

Reading the sox documentation, it occurs to me that one could create a
complete musical composition using sox, driven by one huge and strange
looking script.

A related thought is that at some point I'll need a high-level score.
This could be in Ruby or scheme or whatever.  Done properly, it
wouldn't really care how the sound ultimately gets rendered, whether
that's Sox or Mu.

### when it comes to composing...

As a discipline, there should be a directory of raw source sounds.
`make composition` should process the raw sounds in any way needed
(pitch shifting, resampling, trimming, etc) to create processed sound
files needed by the composition.  After that, the composition proper
can start.

### cross-fading

How about a CropStream that fades in and out at each end?  i.e.:

  XFadeStream& setStart(Tick start);
  XFadeStream& setEnd(Tick end);
  XFadeStream& setXFadeTime(Tick x_fade_time);

The fade in starts at \c start - x_fade_time / 2 or at the starting
time of the source signal, whichever comes later.  Similarly, the fade
out starts at end - x_fade_time / 2 or at the ending time of the
source signal, whichever comes earlier.

If the x_fade does not have time to fade all the way in before
starting to fade out, then there will be a smooth transition, even
though the gain never reaches unity.

### Finding memory smashers

My test examples have started to exibit non-repeatable crashes and
malloc errors.  I suspect that I've written a memory smasher (or two);
the following may help find them:

https://developer.apple.com/library/mac/documentation/performance/conceptual/managingmemory/Articles/MallocDebug.html

"Run your program against libgmalloc.dylib in gdb. This library is an
aggressive debugging malloc library that can help track down insidious
bugs in your code. For more information, see the libgmalloc man page."

https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/libgmalloc.3.html
https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/malloc.3.html


DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib

### Finding memory smashers (redux)

[1] compile with -O0 -g to make debugger friendly object files
[2] DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib to use libgmalloc

See https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man3/libgmalloc.3.html
for a good treatment (including running lldb).

It would be useful to run unit tests under libgmalloc.  It would also
be useful to be able to use a command line definition to `make` to
select debugging flags (-O0 -g -Wall) vs (-03 -Wall).

Anyway, a lot of problems were solved when I changed:

// sine-stream.cpp
    for (Tick i=0; i<buffer.size(); i++) { 
to
    for (Tick i=0; i<buffer.frames(); i++) { 

### PsiStream

Psi = Phase Synchronous Interpolation, which is my variant on SOLA.
It's like SOLA in that it's a time-domain time stretcher, but it does
a crossfade on every sample..

Some ideas:

* TODO: For pitch detection, benchmark discrete cross-correlation
algos vs FFT based algos.  Since we're looking only for a peak, we can
use a gradient descent method so we only call the core correlator a
few times.  And our discrete cross-correlation doesn't need to
normalize.

To compute the period at time t, we can call 2 ffts and one ifft O(3 n
log n) OR "a few" calls to the discrete cross-correlation algo (since
we're looking to find a well-defined peak). Overall, the latter may be
more efficient.  [DONE.  Using Brent minimizer from gsl minimization
library.]

* TODO: Take the derivative of the waveform (actually: finite
difference) at initialization time to speed up interpolation.  [DONE.]
We could even do something fancier with spline fits, but the sound
quality is good as is.

* TODO: modify PsiStream to read a ".psi" file that contains the
original waveform, the deltas, and the period at each sample.  Give
it the means to create the file from a .wav file as well...[DONE]

### debugging stk's makefile

The current STK makefile doesn't create the shared libstk.dylib
correctly.  There are a couple of problems, but I monkey patch a
couple of files in $HOME/src/Makefile to work around it.

See 
http://stackoverflow.com/questions/24146452/how-to-create-an-absolute-path-reference-in-a-dylib
http://stackoverflow.com/questions/6569478/detect-if-executable-file-is-on-users-path
http://stackoverflow.com/questions/10319652/check-if-a-file-is-executable
http://stackoverflow.com/questions/592620/how-to-check-if-a-program-exists-from-a-bash-script

### slide guitar

see examples/mune28 for a programming pattern

inst.rest(dur);
inst.pluck(double dur, double pitch, double portamento_rate, double portamento_error, double portamento_correction_rate)
inst.gliss(double dur, double pitch, double portamento_rate, double portamento_error, double portamento_correction_rate)

inst.pluck() starts a new note (with an attack).  inst.gliss() extends
the current note and shifts pitch without a new attack.

portamento_rate is semitones per second: how fast we glide from one pitch to another, which can be zero for a fretted
note (via pluck()) or hammer-on (via gliss()).  

portamento_error is in semitones: the note will change to (pitch + portamento_error * frand()), and then settle back
to pitch over portamento_correction_rate seconds.

Hmmm.  For now, make it simpler and remove the error and correction rate.

### Deterministic streams and connection objects

Someday it would be useful to be able to connect a stream to more than
one output, iff it is "deterministic", that is, each call to step()
with the same arguments will produce exactly the same result.
FileReaderStream is deterministic, ReverbStream is non-deterministic.
Stated another way, a stream is deterministic if it contains no hidden
state.

To make this work, create:

* a "connector" object that patches stream elements together
* a traversal method that knows how to traverse the stream graph
* a "deterministic" flag on each stream element 

With this, a stream element may connect to multiple outputs if it (and
all upstream elements) are deterministic.

Abstracting out a connector object and a traversal method would have
other benefits as well: it would detect attempts to create circular
graphs, automatic mono/stereo conversion, copy an entire subtree, etc.

### Streams vs Events

I've been pondering if Streams should support discrete events in
addition to blocks of sample data.  At the same time, I've been
wondering if the Stream base class should provide a tickHasJumped()
method to signal discontinuities in the stream data.

These are related.

When a call to step(buffer, tick, player) is continuous (i.e. tick
equals prev_tick + buffer.frames()), that can be thought of as a
continuation of a previous event.  When the call to step() is
discontinuous, that can be thought of as a new event.

I'm not sure how this translates into a design, but a germ of an idea
is in there.

### Events, redux.

Assume a stream of discrete events, perhaps the notes of a scale.  And
(at this point), the notes don't even have times associated with them.
Each call to nextEvent() would return the next note, or NULL_EVENT if
there were no more notes.

Assume another stream of timing information.  Each call to nextEvent()
returns a start time and perhaps a duration.  Again, if there are no
more events, it returns NULL_EVENT.

We could convince a SineOsc to generate a scale like:

                       | <= scaleToFreqStream <= scaleStream
DACPlayer <= SinStream |
                       | <= timingStream

... although that starts to look a lot like linsegStream or stepStream.

Perhaps take a step back (away from the strictures imposed by C++):

LoopStream([e0, e1, e2]) => e0, e1, e2, e0, e1, e2...

DropStream([e0, e1, e2, e3, e4], 2) => e2, e3, e4

TakeStream([e0, e1, e2, e3, e4], 2) => e0, e1

AppendStream(s1, s2) => elements of s1 followed by elements of s2 when
s1 evaluates to NULL_STREAM

FlattenStream(s1) => if an element of s1 is a stream, inserts the
elements of the stream at that point.

MergeStream(s1, s2, predicate) => elements of s1 and s2 merged
according to predicate.

MapStream(s1, operator) => a stream where each element of s1 is
replaced by operator(s1)

ZipStream(s1, s2) => a stream of streams containing [s1.e0, s2.e0],
[s1.e1, s2.e2] ...

How would all these work together to continually loop through three
sound files with a cross-fade in-between?

se1 = soundFileSE("s1.wav").setDuration(eigth);  // sound file stream element
se2 = soundFileSE("s2.wav").setDuration(eigth);
se3 = soundFileSE("s3.wav").setDuration(half);
s1 = consStream(se1, consStream(se2, consStream(se3, NULL_STREAM)));
s2 = loopStream(s1);
s3 = crossFadeSteam(s2, 0.05);
DACPlayer.setSource(s3);

A pidgen definition of crossFadeStream() is:
Stream *crossFadeStream(s, fade_time) {
  if (s == NULL_STREAM) {
    return NULL_STREAM;
  } else if (s.next() == NULL_STREAM) {
    return s;
  } else {
    e1 = s.first;
    e2 = s.next.first;
    r = s.next.next;
    return consStream(cropElement(e1, 0, e1.dur-fade_time),
             consStream(fadeOut(cropElement(e1, e1.dur-fade_time, e1.dur)),
               consStream(fadeIn(cropElement(e2, 0, fade_time)),
                 crossFadeStream(
                   consStream(cropElement(e2, fade_time, e2.dur-fade_time), r),
                   fade_time))));
  }
}

This could be cleaned up with a listStream() operator (analogous to a
Scheme or LISP (list ...) procedure:

Stream *crossFadeStream(Stream *s, double fade_time) {
  if (s == NULL_STREAM) {
    return NULL_STREAM;
  } else if (s->next() == NULL_STREAM) {
    return s;
  } else {
    StreamElement *e1 = s->first();
    StreamElement *e2 = s->next()->first();
    Stream *rest = s->next()->next();
    return 
      listStream(e1->cropElement(0, e1->dur()-fade_time),
                 e1->cropElement(e1->dur()-fade_time, e1->dur())->fadeOut(),
                 e2->cropElement(0, fade_time)->fadeIn(),
                 crossFadeStream(
                   consStream(e2->cropElement(fade_time, e2->dur()-fade_time), rest),
                   fade_time));
  }
}

These definitions omit shifting each element to the correct time, but
the structure is what's important: each element of the original stream
is broken into up to three elements: fadeIn, steady state, and
fadeOut.  It is written so that an element omits the fadeIn if it is
first and omits the fadeOut if it is last, which is what you normally
want.

Each element operation (crop, fadeIn, etc), is really just a deferred
wrapper around the element that will be interpreted (only) when the
stream is rendered.

I gotta say: this feels much more like the right thing.  

### Event streams

After bashing my head on the C++ (Turing complete) type system, I need
to step back and think about a natural way to express events and
streams of events rather than lose myself in C++ semantics and its
strict typing system.

Proto-example

We have three sound files f0, f1, f2.  We want to play one second of
f0, followed by two seconds of f1, followed by three seconds of f2,
and we want that entire sequence to loop. In addition, we want
cross-fades between each transition including going from f2 back to
f0.

We assume that f1 is shifted by one second, f2 is shifted by three
seconds, and that the entire sequence (before looping) is six seconds
long.  

One design question: when does cropping happen?  Assume that cross
fades are 0.2 seconds long (I know that's abnormally long), so we're
really taking d + 0.2 seconds of each sound file (0.1 extra at the
beginning and 0.1 at the end).  The best answer may be that cropping
doesn't shorten the actual sound, it just dictates new starting and
ending times, which are hints for the renderer.

e0 = soundFileRead("f0").crop(0.0, 1.0).delay(0.0);
e1 = soundFileRead("f1").crop(0.0, 2.0).delay(1.0);
e2 = soundFileRead("f2").crop(0.0, 3.0).delay(3.0);
s1 = mergeStream(e0, e1, e2);  // merge on start times by default 
s2 = loopStream(s1);           // default loop dur = last end time
s3 = xfadeStream(s2).setXFadeTime(0.2);

player.setSource(s3);
player.start();

Q: When does an operation work on an element vs a stream?
A: When it makes sense.  crop() and delay() are applicable to entire
   streams as well as to indivual events.  In many cases, it might be
   easiest to create an event stream with a single event and define
   methods to work (only) on streams.

Q: Then is there such a thing as a single event?
A: Dunno.  They can probably be manupulated as singleton streams, but
   at some point each event needs to get rendered, which is specific
   to that event.

Q: Is there a difference between:

    e0 = soundFileRead("f0").crop(0.0, 1.0).delay(3.0);

and

    e0 = soundFileEvent.new().setFileName("f0");
    e1 = cropEvent.new().setSource(e0).setStart(0.0).setEnd(1.0);
    e2 = delayEvent.new().setSource(e1).setDelay(3.0)

"Everything is a motif", where a motif is one or more renderable
events.  Events have their own starting and ending times (possibly
indefinite).  By default, events are mixed together, not sequenced.
If you want one event to end before another starts, you use xfade or
cropping to enforce an order.

=== Hmmm

s <= makeMergeStream().addSource(s0).addSource(s1).setSortPredicate(p);

Returns a stream that merges all of its sources.  Items are sorted by
their natural sort order or with the explicit sorting predictate.

s <= makeProbabilityStream().setProbability(p).setSource(s0);

Returns a stream that when evaluated returns s0 with a probability of
p and NULL_STREAM with a probability of 1-p.  [Extra credit: p can be
a stream as well?]

=== Back to earth

1. In the step() method, replace the player argument with a
is_new_event argument, set to true at the start of a new event.

2. Create a family of "untimed" stream elements, e.g. ones that emit
new double values (or new strings, or new streams, etc) whenever
called.  I'll call these EStream objects:

class SequencePlayerStream : public Stream {
  void step(buffer, tick, is_new_event) {
    if (is_new_event) current_source_ = getNextSource();
    if (current_source_ != NULL_STREAM) current_source_->step(buffer, tick, is_new_event);
  }

  Stream * getNextSource() {
    Stream *s = input_.first();
    input_ = input_.next();
    return s;
  }

  protected:
  Stream *current_source_;
  EStream *input_;
}

In the code above, there's no way to rewind input_.  Perhaps that's okay,
and we accept the fact that once you start the machine running, it always
plays from the start.  We can keep a reference to the original stream_source_
if needed, like this:

class SequencePlayerStream : public Stream {
  void step(buffer, tick, is_new_event) {
    current_source_ = getSource();
    if (current_source_ != NULL_STREAM) current_source_->step(buffer, tick, is_new_event);
    if (is_new_event) advanceSource();
  }

  EStream *getStreamSource( void ) { return stream_source_; }
  SequencePlayerStream *setStreamSource( EStream *stream_source) {
    stream_source_ = input_ = stream_source;
    return this;
  }

  Stream *getSource() {
    (input_ == NULL_ESTREAM) ? NULL_STREAM : input_->first();
  }

  void advanceSource() {
    if (input_ != NULL_ESTREAM) {
      EStream *tmp = input_->next();
      if (input_ != stream_source_) delete input_;
      input_ = tmp;
    }
  }
  protected:
  Stream *current_source_;
  EStream *stream_source_;
  EStream *input_;

}

So...

SStream (pka Stream) is a sample stream, responds to step().

EStream is an event stream, responds to first() and next().  An
EStream is immutable, so keeping a reference to a specific stream
element means you can always get back to that state.  An EStream has
only one consumer (one entity calling next()), so that consumer may
call delete on the previous element without worry.

As an aside, we can emulate the immutable nature with an itarative
(non-recursive) definition by creating a copy method that captures the
current state.  Calling next() on a stream could (then) actually
modify its internal state.

Next steps: 

rename Stream to SStream (or maybe SP for Sample Processor). [done]
change step(..., player) to step(..., is_new_event) [done]
create EStream (or maybe ES for Event Stream)

=== Some streams to write

Generalized Events:

* ConstantES(v)
* AppendES(s0, s1, ...) 
* InterleaveES(s0, s1, ...)
* CycleES(s)
DropES(s, n)
MapES(s, fn)
FilterES(s, fn)
* TakeES(s, n)
ZipES(s0, s1, ...)

Stolen from Ruby:

s.all?(fn)
s.any?(fn)
s.count()

With time associated:

MergeES(s0, s1, ...)
DelayES(s, t)
StretchES(s, t)

=== hello again

It's been a while.  Let's see how much we can do without creating an entirely
new class of EventStream.  Perhaps we can accomplish (nearly?) everything with
just StreamProcessors.

s1 = new FileReadSP().setFileName("f1");
s2 = new CropSP().setSource(s1).setStart(1.0).setEnd(2.0);
player.setSource(s2);
player.start();

At time = 0.0, this plays samples from 1.0 seconds into file f1.  It stops at
time = 1.0.  Note that CropSP now creates an implicit delay: the start time is
shifted to time 0.0.  Syntactically (and semantically), this is similar to
what we already have.  

s1 = new FileReadSP().setFileName("f1");
s2 = new CropSP().setSource(s1).setStart(1.0).setEnd(2.0);
s3 = new FileReadSP().setFileName("f2");
s4 = new CropSP().setSource(s1).setStart(1.0).setEnd(2.0);
s5 = new AppendSP().addSource(s1).addSource(s2);
player.setSource(s5);
player.start();

This introduces AppendSP, whose contract is to start playing one stream when
the previous one ends, with an implicit delay of the previous stream's end 
time.  This effectively creates a butt splice between each stream.

s1 = new FileReadSP().setFileName("f1");
s2 = new CropSP().setSource(s1).setStart(1.0).setEnd(2.0);
s3 = new FileReadSP().setFileName("f2");
s4 = new CropSP().setSource(s1).setStart(1.0).setEnd(2.0);
s5 = new AppendSP().addSource(s1).addSource(s2).setXFadeTime(0.2);
player.setSource(s5);
player.start();

Here, AppendSP has a new method, setXFadeTime(), that creates a cross-fade
between successive streams.

I think where all this is going is that we build up a symbolic representation
of all the transformations (sound source, delay, crop, cross-fade), and don't
actually manpulate any audio data until it is time to render the samples.

Next question: is this "score" a static data structure?  If so, how does it
handle loops?  Or is it an object that responds to some well defined methods?

And at the end of the day, what does this buy us?  A time-ordered stream of
events, where each event has a render() method.

=== Meh.

Rather than create an entirely new subsystem, I just want to see how
much progress I can make with the existing structure.  Created a good
sounding strummed multi-string instrument with hammer-on, hammer-off.
See mune33.cpp.

=== Client / Server Split

It's time to split the system into a server and a client.  The server
is a synthesis engine, responsible for generating audio.  The client
runs as a separate process (possibly on a different machine) and feed
feeds "gestural" directives to the server.

As a first step, I'm defining the interface to be JSON based, with
messages flowing from client to server.  (We may want server => client
messages at some point, but I'm not worrying about them yet).  The
basic message is JSON object with two well-known fields: name and
method:

       { "name": "fred",
         "method": "addSegment",
         "time": 123.456,
         "file_name": "lib/snd/a.snd"
       }

which, on the server side translates into a method call on named
object "fred":

    fred.addSegment(... args ...);

I expect the server to be written in pure C++, the client to be
written in the language du jour -- I'm leaning towards Ruby or Python,
but any language that can push JSON over an IPC port is fair game.