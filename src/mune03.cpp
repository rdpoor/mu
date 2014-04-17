/*
 * Play a sound file (from stk::FileRead) directly to the DAC (via
 * stk::RtAudio).  Sets up a double buffer of StkFrames so one plays
 * while the other is filled.
 *
 * see:
 * ~/Projects/Mune/tarballs/stk-4.4.4/include/Stk.h (for StkFrames)
 * ~/Projects/Mune/tarballs/stk-4.4.4/src/Stk.cpp
 * ~/Projects/Mune/tarballs/stk-4.4.4/include/RtWvOut.h
 * ~/Projects/Mune/tarballs/stk-4.4.4/src/RtWvOut.cpp
 * ~/Projects/Mune/tarballs/stk-4.4.4/include/FileRead.h
 * ~/Projects/Mune/tarballs/stk-4.4.4/src/FileRead.cpp
 */
#include "Stk.h"
#include "RtAudio.h"
#include "FileRead.h"
#include <signal.h>

using namespace stk;

// print buffer activity to stderr
#define NOISY

/*
 * FrameRing links a series of stk:StkFrames objects into a circular
 * ring, and associates a frame state (empty, full, etc) with each
 * one.  We use this for transferring data between our system and the
 * dac: one process fills StkFrames when they become empty, another
 * one drains them when they become full.
 */
class FrameRing {
public:

  /* the four states of a buffer */
  typedef enum { B_EMPTY, B_FILLING, B_FULL, B_DRAINING } FrameState;

  /* ================================================================
     class methods
     ================================================================ */

  static FrameRing *createRing(unsigned int nBuffers = 3) {
    FrameRing *head, *tail;

    head = tail = (FrameRing *)0;
    for (unsigned int i=0; i<nBuffers; i++) {
      FrameRing* curr = new FrameRing();
      curr->next_ = head;
      head = curr;
      if (tail == (FrameRing *)0) tail = curr;
    }
    tail->next_ = head;         // close the ring
    return head;
  }

  // TODO: figure out how to write an 'apply' method that calls an
  // arbitrary function on each element of the ring.  Then recast
  // destroyRing() and resizeFrames() to use that.

  static void destroyRing(FrameRing *ring) {
    FrameRing *next, *curr;

    curr = ring;
    while ((next = curr->next_) != ring) { 
      delete curr;
      curr = next;
    }
    delete curr;                // delete last in ring
  }

  static void resizeFrames( FrameRing *ring, size_t nFrames, unsigned int nChannels = 1 ) {
    FrameRing *next, *curr;
    curr = ring;
    while ((next = curr->next_) != ring) {
      curr->stkFrames_->resize(nFrames, nChannels);
      curr = next;
    }
    curr->stkFrames_->resize(nFrames, nChannels);
  }

  /* ================================================================
     constructor, destructor
     ================================================================ */

  //! The default constructor creates an underlying StkFrame 
  FrameRing( ) {
    stkFrames_ = new StkFrames(); // set nFrames and nChannels later...
    state_ = B_EMPTY;
    next_ = (FrameRing *)0;
  }

  // destructor
  ~FrameRing() {
    delete stkFrames_;
    next_ = (FrameRing *)0;     // be safe.
  }

  /* ================================================================
     instance methods
     ================================================================ */
  StkFrames *stkFrames() {
    return stkFrames_;
  }
  FrameState state() {
    return state_;
  }
  FrameRing *setState(FrameState state) {
    state_ = state;
    return this;
  }
  FrameRing *next() {
    return next_;
  }

  void inspect() {
    printf("FrameRing<#%p: stkFrames=%p state=%d next=%p>", this, stkFrames_, state_, next_);
  }

private:
  /* ================================================================
     instance variables
     ================================================================ */  
  StkFrames *stkFrames_;
  FrameState state_;
  FrameRing *next_;
};

/* ================================================================
   toplevel code
   ================================================================ */

struct TickData {
  FrameRing *putr;              // next buffer to be filled (in main())
  FrameRing *takr;              // next buffer to be drained (by tick())
  unsigned long int startFrame; // next frame to read from input file
  bool stopRequested;

  TickData()
    : putr(0), takr(0), startFrame(0), stopRequested(false) {}
};

/*
 * TickData needs to be global so the interrupt handler can set
 * stopRequested
 */
TickData tickData;

/* 
 * tick() is called asynchronously by RtAudio whenever a new buffer of
 * data is needed for the DAC.  If a buffer of sound samples is
 * available in the ring buffer, transfer it to RtAudio for playing,
 * otherwise signal an underrun.
 *
 */
int tick( void *outputBuffer,          // where to write the samples
          void *inputBuffer,           // unused (for A/D input)
          unsigned int nBufferFrames,
          double streamTime, 
          RtAudioStreamStatus status, 
          void *userData ) {
  TickData *data = (TickData *)userData;
  register StkFloat *samples = (StkFloat *) outputBuffer;

  if (data->stopRequested) {
    return 1;
  } else if (data->takr->state() != FrameRing::B_FULL) {
    fprintf(stderr, "Buffer underrun, stopping...\n");
    return 1;
  }

  StkFrames *frames = data->takr->stkFrames();

  // announce that we're draining the buffer
  data->takr->setState(FrameRing::B_DRAINING); 
#ifdef NOISY
  fprintf(stderr, "<");
#endif

  // copy the samples ouf of the stkFrames into the DAC buffer
  // (note: assumes that frames->size() == sample buffer size)
  for (unsigned int i=0; i<frames->size(); i++) {
    *samples++ = (*frames)[i];
  }
  // announce that the buffer is available for filling
  data->takr->setState(FrameRing::B_EMPTY); 

  // prepare to drain the next buffer in the circular ring
  data->takr = data->takr->next(); 

  return 0;
}

void usage(int argc, char *argv[]) {
  printf("usage: %s filename-to-play\n", argv[0]);
  exit( 0 );
}

void fillBuffers(FileRead *file, TickData *data) {
  while (data->putr->state() == FrameRing::B_EMPTY) {
    // announce that we are filling the buffer.
    data->putr->setState(FrameRing::B_FILLING);
#ifdef NOISY
    fprintf(stderr, ">");
#endif

    // copy bits from sound file into stkFrames buffer
    file->read(*(data->putr->stkFrames()),
               data->startFrame,
               true);
    // increment start frame
    data->startFrame += data->putr->stkFrames()->frames();

    // Request stop if end of file has been reached.  BUG: As written,
    // it stops the stream prematurely since N-1 stkFrames have yet to
    // be played.
    if (data->startFrame > file->fileSize()) {
      printf("at end of sound file.  requesting stop\n");
      data->stopRequested = true;
    }

    // announce that the buffer is available for draining
    data->putr->setState(FrameRing::B_FULL);
    
    // prepare to fill the next buffer in the circular ring
    data->putr = data->putr->next();
  }
  // return when no buffers are available for filling
}

// called from SIGINT, set stopRequested true for an orderly shutdown
static void finish(int ignore) { 
  printf("finish called with argument %d\n", ignore);
  tickData.stopRequested = true;
}

/* ================================================================
   main()
   ================================================================ */

int main(int argc, char *argv[]) {
  if (argc != 2) usage(argc, argv);
  char *filename = argv[1];

  printf("--- reading %s\n", filename);
  FileRead file;
  file.open(filename);
  printf("--- file.isOpen() = %d\n", file.isOpen());
  printf("--- file.fileSize() = %lu\n", file.fileSize());
  printf("--- file.channels() = %u\n", file.channels());
  printf("--- file.format() = %ld\n", file.format());
  printf("--- file.fileRate() = %f\n", file.fileRate());
  
  // Now that we know the sound file details, configure DAC playback.
  RtAudio dac;
  RtAudio::StreamParameters parameters;
  parameters.deviceId = dac.getDefaultOutputDevice();
  parameters.nChannels = file.channels();
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  unsigned int bufferFrames = RT_BUFFER_SIZE;
  try {
    dac.openStream( &parameters, NULL, format, (unsigned int)file.fileRate(), &bufferFrames, &tick, (void *)&tickData);
  } catch (RtError &error) {
    error.printMessage();
    // goto cleanup; -- hmm - how to handle this?
  }

  // Install an interrupt handler to clean up on ^C
  (void) signal(SIGINT, finish);

  // Create a ring of buffers and resize the StkFrames accordingly
  FrameRing *ring = FrameRing::createRing();
  tickData.putr = tickData.takr = ring;
  FrameRing::resizeFrames(ring, bufferFrames, file.channels());
  
  tickData.stopRequested = false;
  tickData.startFrame = 0;        // start at beginning of the file

  // main loop here...
  while ( !tickData.stopRequested ) {

    // fill all available buffers
    fillBuffers(&file, &tickData);

    // start dac if not already running
    if (!dac.isStreamRunning()) {
      try {
        dac.startStream();
      } catch ( RtError &error ) {
        error.printMessage();
        // goto cleanup;  -- hmm.  how to handle this?
      }
    }

    // wait for a buffer to become available
    Stk::sleep(12);
  }

  // cleanup:
  if (dac.isStreamOpen()) {
    try {
      dac.closeStream();
    } catch ( RtError &error ) {
      error.printMessage();
    }
  }

  return 0;
}
