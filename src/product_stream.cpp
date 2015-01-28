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
#include "product_stream.h"
#include "mu_utils.h"

namespace mu {

  ProductStream::ProductStream() {
    tmp_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ProductStream::~ProductStream() {
    // TODO: should be in MultiSourceStream::clone() (but how to call it?)
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      if (source != NULL) delete source;
    }
  }

  ProductStream *ProductStream::clone() {
    ProductStream *c = new ProductStream();
    // TODO: this should be in MultiSourceStream::clone() (but how do I call it?)
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      c->add_source((source == NULL) ? source : source->clone());
    }
    return c;
  }

  bool ProductStream::render(MuTick buffer_start, MuBuffer *buffer) {

    int n_frames = buffer->frames();
    int n_channels = buffer->channels();
    bool any_modified = false;

    tmp_buffer_.resize(n_frames, n_channels);

#ifndef ZERO_BUFFER
    MuUtils::zero_buffer(buffer);
    MuUtils::zero_buffer(&tmp_buffer_);
#endif

    // we could optimize the following...
    MuUtils::fill_buffer(buffer, 1.0);

    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      if (source->render(buffer_start, &tmp_buffer_)) {
        any_modified = true;
        for (int tick=n_frames-1; tick>=0; --tick) {
          for (int channel=n_channels-1; channel >=0; --channel) {
            // (*buffer)(tick, channel) *= tmp_buffer_(tick, channel);
            MuFloat v1 = (*buffer)(tick, channel);
            MuFloat v2 = tmp_buffer_(tick, channel);
            // printf("%ld: %f * %f = %f\n", tick + buffer_start, v1, v2, v1 * v2);
            (*buffer)(tick, channel) = v1 * v2;
          }
        }
      } else {
        // BUG: Shouldn't have to do this.
        MuUtils::zero_buffer(buffer);
      }
    }

    return any_modified;
  }

  void ProductStream::inspect_aux(int level, std::stringstream *ss) {
    MultiSourceStream::inspect_aux(level, ss);
  }

}                               // namespace mu
