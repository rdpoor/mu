// play a 440+(2*ch) Hz sinewave in channel ch
//
// to run:
//  ./mune 2 44100 2
//

#include "RtAudio.h"
#include <math.h>

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE  32767.0

/*
typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0

typedef double MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0
*/

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ ) || defined( __WINDOWS_WASAPI__ )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications
  std::cout << "\nuseage: mune N fs <device> <channelOffset> <time>\n";
  std::cout << "    where N = number of channels,\n";
  std::cout << "    fs = the sample rate,\n";
  std::cout << "    device = optional device to use (default = 0),\n";
  std::cout << "    channelOffset = an optional channel offset on the device (default = 0),\n";
  std::cout << "    and time = an optional time duration in seconds (default = no limit).\n\n";
  exit( 0 );
}

unsigned int channels;
RtAudio::StreamOptions options;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
const unsigned int callbackReturnValue = 1;

// ================================================================
// SinState contains the state for one sine wave oscillator

typedef struct {
  double amplitude;             // amplitude (0..1)
  double theta;                 // radians
  double d_theta;               // increment per sample
} SinState;

SinState *initialize_ss(SinState *ss, double amplitude, double frequency, double srate) {
  ss->amplitude = amplitude;
  ss->theta = 0.0;
  ss->d_theta = frequency * 2.0 * M_PI / srate;
  return ss;
}

// #define USE_INTERLEAVED
#if defined( USE_INTERLEAVED )

// Interleaved buffers
int sinner( void *outputBuffer,
            void * /* inputBuffer */,
            unsigned int nBufferFrames,
            double /* streamTime */,
            RtAudioStreamStatus status,
            void *data ) {
  MY_TYPE *buffer = (MY_TYPE *) outputBuffer;
  unsigned int i, j;

  if ( status ) {
    std::cout << "Stream underflow detected!" << std::endl;
  }

  for ( i=0; i<nBufferFrames; i++ ) {
    for ( j=0; j<channels; j++ ) {
      SinState *ss = &((SinState *)data)[j];
      *buffer++ = (MY_TYPE)(SCALE * (ss->amplitude) * sin(ss->theta));
      ss->theta += ss->d_theta;
    }
  }

  frameCounter += nBufferFrames;
  if ( checkCount && ( frameCounter >= nFrames ) ) {
    return callbackReturnValue;
  } else {
    return 0;
  }
}

#else // Use non-interleaved buffers

int sinner( void *outputBuffer,
            void * /* inputBuffer */,
            unsigned int nBufferFrames,
            double /* streamTime */,
            RtAudioStreamStatus status,
            void *data ) {
  MY_TYPE *buffer = (MY_TYPE *) outputBuffer;
  unsigned int i, j;

  if ( status ) {
    std::cout << "Stream underflow detected!" << std::endl;
  }

  for ( j=0; j<channels; j++ ) {
    SinState *ss = &((SinState *)data)[j];
    for ( i=0; i<nBufferFrames; i++ ) {
      *buffer++ = (MY_TYPE)(SCALE * (ss->amplitude) * sin(ss->theta));
      ss->theta += ss->d_theta;
    }
  }

  frameCounter += nBufferFrames;
  if ( checkCount && ( frameCounter >= nFrames ) ) {
    return callbackReturnValue;
  } else {
    return 0;
  }
}

#endif

int main( int argc, char *argv[] )
{
  unsigned int bufferFrames, fs, device = 0, offset = 0;

  // minimal command-line checking
  if (argc < 3 || argc > 6 ) usage();

  RtAudio dac;
  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  channels = (unsigned int) atoi( argv[1] );
  fs = (unsigned int) atoi( argv[2] );
  if ( argc > 3 )
    device = (unsigned int) atoi( argv[3] );
  if ( argc > 4 )
    offset = (unsigned int) atoi( argv[4] );
  if ( argc > 5 )
    nFrames = (unsigned int) (fs * atof( argv[5] ));
  if ( nFrames > 0 ) checkCount = true;

  // double *data = (double *) calloc( channels, sizeof( double ) );
  void *data = (SinState *) calloc( channels, sizeof( SinState ) );

  for (unsigned int i = 0; i < channels; i++) {
    float freq = 440.0 + 2*i;
    SinState *ss = &((SinState *)data)[i];
    initialize_ss(ss, 0.5, freq, fs);
  }

  // Let RtAudio print messages to stderr.
  dac.showWarnings( true );

  // Set our stream parameters for output only.
  bufferFrames = 512;
  RtAudio::StreamParameters oParams;
  oParams.deviceId = device;
  oParams.nChannels = channels;
  oParams.firstChannel = offset;

  options.flags = RTAUDIO_HOG_DEVICE;
  options.flags |= RTAUDIO_SCHEDULE_REALTIME;
#if !defined( USE_INTERLEAVED )
  options.flags |= RTAUDIO_NONINTERLEAVED;
#endif
  try {
    dac.openStream( &oParams, 
                    NULL, 
                    FORMAT, 
                    fs, 
                    &bufferFrames, 
                    &sinner, 
                    (void *)data, 
                    &options );
    dac.startStream();
  }
  catch ( RtError& e ) {
    e.printMessage();
    goto cleanup;
  }

  if ( checkCount ) {
    while ( dac.isStreamRunning() == true ) SLEEP( 100 );
  }
  else {
    char input;
    //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
    std::cout << "\nPlaying ... press <enter> to quit (buffer size = " << bufferFrames << ").\n";
    std::cin.get( input );

    try {
      // Stop the stream
      dac.stopStream();
    }
    catch ( RtError& e ) {
      e.printMessage();
    }
  }

 cleanup:
  if ( dac.isStreamOpen() ) dac.closeStream();
  free( data );

  return 0;
}
