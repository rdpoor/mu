# mu

An experiment in blurring the lines between music composition and sound synthesis

## todo
* Split monolithic files into src, include, obj directories
* Create a FileReader node.
* Create a Mixer node.
* Create a Loop node.
* Add command line parsing such as argp.h
* Fix dependencies in makefile (always makes the target)

## changelog 

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

Like Chuck, synthesis is handled by a directed graph of generators and
filters.  A node in the graph has zero or more inputs and exactly one
output. At the root of the graph, the RtAudio object issues a tick()
callback when it is ready for more data.  The contract for a node's
tick() method is to fill the caller's buffer with sample data and
return the number of frames actually written.  A return value of 0
means that no samples were written, but more might be written on a
subsequent call to tick().  A negative return value means that no more
samples will be produced untit reset() or seek() is called.

    class MuNode {
      int tick(StkFloat *buffer, unsigned int nFrames, unsigned int nChannels);
      void reset();
      void setup();
      void teardown();
      void seek(time_t t);

      bool dependsOn(MuNode& other);
    }

