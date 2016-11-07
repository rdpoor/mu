# mu

mu is an experiment in blurring the lines between music composition and sound
synthesis.  It comprises a software library of digital audio processing modules
and the means to render audio in real time to a computer's sound card or to a
file.

Each digital audio processing module (called a "stream") exposes a method `bool
render(MuBuffer buffer, MuTick buffer_start)`, whose contract is to fill the
`buffer` with audio samples, whose first sample is at time `buffer_start`.

Many streams take other streams as inputs in order to process or modify the
samples.  Ultimately, the streams are connected in a "processing graph",
specifically a directed acyclic graph in which audio samples are computed on
demand by calls to `render()` at the root stream.

In a typical configuration, a Transport object connects to the root stream.  The
Transport's contract is to wait for buffer requests from the computer's sound
driver, then call render() on the root stream to generate the requested samples.

## Processing Streams

mu defines the following streaming audio classes.

### CropStream

Limits an input stream to a starting and ending time.

### DelayStream

Delays its input stream by `delay` samples.

### DiracStream

Produces a sample value of 1.0 for sample number 0, 0.0 otherwise.

### FileReadStream

Produces samples from a sound file.

### FileWriteStream

Writes sample data to a sound file.

### IdentityStream

Produces sample value t for sample number t.

### LoopStream

Generates a loop from an input stream.

### ProductStream

Generates the product of all of its inputs.

### SineStream

Generates a sine wave with optional inputs for amplitude and phase modulation.

### SumStream

Generates the sum of all of its inputs.  

## todo 

* Clean up usr/include: put fftw* into a fftw directory and jansson*
  into a jansson directory.

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

* Write a generalized graph traversal method for inspect and loop detection.

* When do we release resources?  Do we need a Transport.pause() method
  distinct from Transport.stop()?

* Add command line parsing such as argp.h

* Create mechanisms for panning and localization.

* Create a fuzzbox

* Create a morley pedal

* Experiment in adding 6th harmonic to make a metal string out of nylon

* State-variable filter with stream control inputs
  
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

### Finding memory smashers

The following can help find memory smashers:

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

### PsiStream

Psi = Phase Synchronous Interpolation, which is my variant on SOLA.
It's like SOLA in that it's a time-domain time stretcher, but it does
a crossfade on every sample..

### Some streams to write

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

## Decreed

bool mu::Stream.render(mu::FrameBuffer buffer, 
                       mu::Tick start_tick, 
                       bool is_new_event)

instructs the receiver to write up to buffer.frames() samples into `buffer`,
where buffer[0] corresponds to time `start_tick`.  For any given tick, the
receiver may not write a sample if the stream has an undefined value at that
time.

The method returns true if any samples were written into `buffer, false
otherwise.

