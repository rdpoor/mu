/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   ================================================================ 
*/

#include "fuzz_stream.h"
#include "mu_utils.h"

namespace mu {

  FuzzStream::FuzzStream( void ) 
    : mix_(1.0), fuzz_factor_(0.5) {
  }

  FuzzStream::~FuzzStream( void ) {
    // printf("~FuzzStream()\n");
  }

  FuzzStream *FuzzStream::clone( void ) {
    FuzzStream *c = new FuzzStream();
    // TODO: Should be defined in SingleSourceStream
    c->set_source(source() ? source()->clone() : NULL);
    c->set_mix(mix());
    c->set_fuzz_factor(fuzz_factor());
    return c;
  }

  bool FuzzStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if (source_ == NULL) {
      return false;
    }

    int n_frames = buffer->frames();
    int n_channels = buffer->channels();

    tmp_buffer_.resize(n_frames, n_channels);
    MuUtils::zero_buffer(&tmp_buffer_);
    if (source_->render(buffer_start, &tmp_buffer_) == false) {
      return false;
    }

#if 0

    // Cubic polynomial with f'(0) = 1/f'(1) seemed like a good idea, but really
    // didn't work that well: you can't get a steep slope around x=0 without
    // creating a saddle point > y=1.0.  Maybe I'm just overspecifying it.

    MuFloat m = fuzz_factor_;
    MuFloat a = -2 + m + 1/m;
    MuFloat b = 3 - 2*m - 1/m;
    MuFloat c = m;

    printf("m=%f, a=%f, b=%f, c=%f\n", m, a, b, c);

    for (int frame = 0; frame < n_frames; ++frame) {
      for (int channel = 0; channel < n_channels; ++channel) {
        MuFloat x = tmp_buffer_(frame, channel);
        MuFloat fuzz_sample;
        if (x > 0) {
          fuzz_sample = a*x*x*x + b*x*x + c*x;
        } else {
          x = -x;
          fuzz_sample = -(a*x*x*x + b*x*x + c*x);
        }
        (* buffer)(frame, channel) = fuzz_sample * mix_ + x * (1.0 - mix_);
      }
    }
#else
    // Use f(x) = x^(1-fuzz_factor) when x > 0, x otherwise
    MuFloat m = 1.0 - fuzz_factor_;

    for (int frame = 0; frame < n_frames; ++frame) {
      for (int channel = 0; channel < n_channels; ++channel) {
        MuFloat x = tmp_buffer_(frame, channel);
        MuFloat fuzz_sample;
        if (x > 0) {
          fuzz_sample = pow(x, m);
        } else {
          // The asymmetry sounds good...
          // x = -x;
          // fuzz_sample = -pow(x, m);
          fuzz_sample = x;
        }
        (* buffer)(frame, channel) = fuzz_sample * mix_ + x * (1.0 - mix_);
      }
    }

#endif

    return true;
  }

  void FuzzStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss); 
    *ss << "mix() = " << mix() << std::endl;
    *ss << "fuzz_factor() = " << fuzz_factor() << std::endl;
    SingleSourceStream::inspect_aux(level, ss);
  }
  

}                               // namespace mu

