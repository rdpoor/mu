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

#include "crop_stream.h"
#include "mu_utils.h"

namespace mu {

  CropStream::CropStream() : 
    source_start_(kUndefined), 
    source_end_(kUndefined) {
  }

  CropStream::~CropStream() {
    // printf("~CropStream()\n");
  }

  CropStream *CropStream::clone() {
      CropStream *c = new CropStream();
      c->set_source_start(source_start());
      c->set_source_end(source_end());
      c->set_source(source() ? source()->clone() : NULL);
      return c;
    }
    
  bool CropStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if (source_ == NULL) { return false; }

    MuTick buffer_end = buffer->frames() + buffer_start;

    MuTick earliest = (source_start_ == kUndefined) ? 
      buffer_start : std::max(buffer_start, source_start_);
    MuTick latest = (source_end_ == kUndefined) ?
      buffer_end : std::min(buffer_end, source_end_);

    if ((earliest >= buffer_end) || (latest < buffer_start)) {
      // nothing to write
      return false;

    } else if ((earliest == buffer_start) && (latest == buffer_end)) {
      // common case: write the whole buffer
      return source_->render(buffer_start, buffer);

    } else if (earliest == latest) {
      return false;

    } else {
      // get samples between earliest and latest
      tmp_buffer_.resize(latest-earliest, buffer->channels());
      MuUtils::zero_buffer(&tmp_buffer_);
      bool any_written = source_->render(earliest, &tmp_buffer_);
      if (any_written) {
        MuUtils::copy_buffer_subset(&tmp_buffer_, 
                                    buffer, 
                                    earliest-buffer_start,
                                    latest-earliest);
      }
      return any_written;
    }

  }                             // bool CropStream::render()

  void CropStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "source_start() = " << source_start() << std::endl;
    inspect_indent(level, ss);
    *ss << "source_end() = " << source_end() << std::endl;
    SingleSourceStream::inspect_aux(level, ss);
  }

}                               // namespace mu
