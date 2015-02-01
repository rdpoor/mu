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
#include "sum_stream.h"
#include "mu_utils.h"

namespace mu {

  SumStream::SumStream() {
    tmp_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  SumStream::~SumStream() {
    // printf("~SumStream()\n");
  }

  SumStream *SumStream::clone() {
    SumStream *c = new SumStream();
    // TODO: this should be in MultiSourceStream::clone() (but how do I call it?)
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      c->add_source((source == NULL) ? source : source->clone());
    }
    return c;
  }


  bool SumStream::render(MuTick buffer_start, MuBuffer *buffer) {

    bool any_modified = false;

    tmp_buffer_.resize(buffer->frames(), buffer->channels());

    for (int i=sources_.size()-1; i>=0; i--) {
      MuStream *source = sources_.at(i);
      if (any_modified == false) {
        // render first source directly into buffer
        any_modified = source->render(buffer_start, buffer);
      } else {
        // render source into temp buffer and sum into buffer
        MuUtils::zero_buffer(&tmp_buffer_);
        if (source->render(buffer_start, &tmp_buffer_)) {
          for (int tick=buffer->frames()-1; tick >= 0; tick--) {
            for (int ch=buffer->channels()-1; ch>=0; ch--) {
              (*buffer)(tick, ch) += tmp_buffer_(tick, ch);
            } // for ch
          }   // for tick
        }     // if (source->)
      }       // else
    }         // for i
    return any_modified;
  }

  void SumStream::inspect_aux(int level, std::stringstream *ss) {
    MultiSourceStream::inspect_aux(level, ss);
  }

}                               // namespace mu
