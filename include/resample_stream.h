/*
 * ResampleStream has two inputs: a sample stream and a timing stream.
 * When called to produce samples, it uses the timing stream to create
 * a timing map into the sample data.  It then fetches the sample data
 * and uses linear interpolation to generate output samples.
 */
#ifndef MU_RESAMPLE_STREAM_H
#define MU_RESAMPLE_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class ResampleStream : public Stream {
  public:

    ResampleStream( void );
    ~ResampleStream( void );

    std::string getClassName() { return "ResampleStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    ResampleStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Stream *getSampleSource( void ) { return sample_source_; }
    ResampleStream& setSampleSource(Stream *source) { sample_source_ = source; return *this; }

    Stream *getTimingSource( void ) { return timing_source_; }
    ResampleStream& setTimingSource(Stream *source) { timing_source_ = source; return *this; }

    // getStart() and getEnd() can only be properly computed by taking
    // the min and max of the entire timingStream.  Better just to say 
    // kIndefinite.

  protected:
    Stream *sample_source_;
    Stream *timing_source_;
    stk::StkFrames sample_buffer_;
    stk::StkFrames timing_buffer_;

  };                            // class ResampleStream
  
}                               // namespace mu

#endif
