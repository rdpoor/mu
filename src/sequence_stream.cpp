/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.  

  ================================================================
*/
#include "delay_stream.h"
#include "gain_stream.h"
#include "sequence_stream.h"
#include "mu_utils.h"

namespace mu {

  SequenceStream::SequenceStream() {
  }

  SequenceStream::~SequenceStream() {
    // printf("~SequenceStream()\n");
  }

  SequenceStream *SequenceStream::clone() {
    SequenceStream *c = new SequenceStream();
    // TODO: this should be in SumStream::clone() (but how do I call it?)
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      c->SumStream::add_source((source == NULL) ? source : source->clone());
    }
    return c;
  }

  void SequenceStream::add_source(MuStream *source, MuTick delay, MuFloat gain) {
    MuStream *s = source;
    if (delay != 0) {
      DelayStream *delay_stream = new DelayStream();
      delay_stream->set_delay(delay);
      delay_stream->set_source(s);
      s = delay_stream;
    }
    if (gain != 1.0) {
      GainStream *gain_stream = new GainStream();
      gain_stream->set_gain(gain);
      gain_stream->set_signal_source(s);
      s = gain_stream;
    }
    SumStream::add_source(s);
  }

  bool SequenceStream::render(MuTick buffer_start, MuBuffer *buffer) {
    return SumStream::render(buffer_start, buffer);
  }

}                               // namespace mu
