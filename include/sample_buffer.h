#ifndef MU_SAMPLE_BUFFER_H
#define MU_SAMPLE_BUFFER_H

#include <stdio.h>
#include "mu.h"
#include "stk.h"

namespace mu {

  class SampleBuffer : public stk::StkFrames {
  public:

    SampleBuffer();
    ~SampleBuffer();

    // set up so subsequent accesses to [] and () are offset into the
    // original data.
    SampleBuffer& slice(stk::StkFrames& stk_frames, unsigned int frame_offset = 0);

    // delegate methods that honor frame_offset
    stk::StkFloat& operator[] ( size_t n );
    stk::StkFloat operator[] ( size_t n ) const;
    stk::StkFloat& operator() ( size_t frame, unsigned int channel );
    stk::StkFloat operator() ( size_t frame, unsigned int channel ) const;
    unsigned int frames(void) const;
    unsigned int size(void) const;

    // TODO: add methods that delegate to the underlying stk_frames_
    // when it makes sense and raises an error when it doesn't.

  private:
    stk::StkFrames stk_frames_;
    unsigned int frame_offset_;
  };

  inline SampleBuffer::SampleBuffer() {
    TRACE("SampleBuffer::SampleBuffer()\n");
  }

  inline SampleBuffer::~SampleBuffer() {
    TRACE("SampleBuffer::~SampleBuffer()\n");
  }

  inline SampleBuffer& SampleBuffer::slice(stk::StkFrames& stk_frames, unsigned int frame_offset) {
    stk_frames_ = stk_frames;
    frame_offset_ = frame_offset;
    return *this;
  }

  // delegate methods
  inline stk::StkFloat& SampleBuffer::operator[] ( size_t n ) { 
    return stk_frames_[n + frame_offset_];
  }
  inline stk::StkFloat SampleBuffer::operator[] ( size_t n ) const { 
    return stk_frames_[n + frame_offset_];
  }
  inline stk::StkFloat& SampleBuffer::operator() ( size_t frame, unsigned int channel ) {
    return stk_frames_( frame + frame_offset_, channel);
  }
  inline stk::StkFloat SampleBuffer::operator() ( size_t frame, unsigned int channel ) const {
    return stk_frames_( frame + frame_offset_, channel);
  }
  inline unsigned int SampleBuffer::frames(void) const { return stk_frames_.frames() - frame_offset_; }
  inline unsigned int SampleBuffer::size(void) const { return frames() * stk_frames_.channels(); }
};
#endif
