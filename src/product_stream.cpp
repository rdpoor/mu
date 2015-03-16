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
    // printf("~ProductStream()\n");
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
    bool any_written = false;

    tmp_buffer_.resize(n_frames, n_channels);

#if 0
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      if (any_written == false) {
        // first source can render directly into the buffer
        if (source->render(buffer_start, buffer)) {
          any_written = true;
        }
      } else {
        // subsequent sources are rendered into tmp_buffer_
        MuUtils::zero_buffer(&tmp_buffer_);
        if (source->render(buffer_start, &tmp_buffer_)) {
          for (int tick=n_frames-1; tick>=0; --tick) {
            for (int channel=n_channels-1; channel >=0; --channel) {
              MuFloat v1 = (*buffer)(tick, channel);
              MuFloat v2 = tmp_buffer_(tick, channel);
              // printf("%ld: %f * %f = %f\n", tick + buffer_start, v1, v2, v1 * v2);
              (*buffer)(tick, channel) = v1 * v2;
            }
          }
        } else {
          // source produced no samples.  Zero the result and quit now...
          MuUtils::zero_buffer(buffer);
          break;
        }
      }
    }                                       // for
#else
    MuUtils::fill_buffer(buffer, 1.0);
    for (int i=sources_.size()-1; i>=0; --i) {
      MuStream *source = sources_.at(i);
      MuUtils::zero_buffer(&tmp_buffer_);
      if (source->render(buffer_start, &tmp_buffer_)) {
        for (int tick=n_frames-1; tick>=0; --tick) {
          for (int channel=n_channels-1; channel >=0; --channel) {
            MuFloat v1 = (*buffer)(tick, channel);
            MuFloat v2 = tmp_buffer_(tick, channel);
            // printf("%ld: %f * %f = %f\n", tick + buffer_start, v1, v2, v1 * v2);
            (*buffer)(tick, channel) = v1 * v2;
          }
        }
        any_written = true;
      } else {
        any_written = false;
        break;
      }
    }
    
    if (any_written == false) { MuUtils::zero_buffer(buffer); }
#endif


    return any_written;
  }

  void ProductStream::inspect_aux(int level, std::stringstream *ss) {
    MultiSourceStream::inspect_aux(level, ss);
  }

}                               // namespace mu
