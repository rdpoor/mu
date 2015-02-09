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

#include "map_stream.h"
#include "mu_utils.h"

namespace mu {

  MapStream::MapStream( void ) 
    : callback_(NULL) {
  }

  MapStream::~MapStream( void ) {
    // printf("~MapStream()\n");
  }

  MapStream *MapStream::clone( void ) {
    MapStream *c = new MapStream();
    // TODO: Should be defined in SingleSourceStream
    c->set_source(source() ? source()->clone() : NULL);
    c->set_callback(callback());
    return c;
  }

  bool MapStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if (source_ == NULL) {
      return false;
    }
    if (callback_ == NULL) {
      return false;
    }

    int n_frames = buffer->frames();
    int n_channels = buffer->channels();

    tmp_buffer_.resize(n_frames, n_channels);
    MuUtils::zero_buffer(&tmp_buffer_);
    if (source_->render(buffer_start, &tmp_buffer_) == false) {
      return false;
    }

    for (int frame = 0; frame < n_frames; ++frame) {
      for (int channel = 0; channel < n_channels; ++channel) {
        MuFloat value = callback_(tmp_buffer_(frame, channel), frame + buffer_start);
        (* buffer)(frame, channel) = value;
      }
    }

    return true;
  }

  void MapStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss); 
    *ss << "callback() = " << callback() << std::endl;
    SingleSourceStream::inspect_aux(level, ss);
  }
  

}                               // namespace mu

