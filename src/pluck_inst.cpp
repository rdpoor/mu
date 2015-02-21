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

#include "mu_types.h"
#include "mu_utils.h"
#include "pluck_inst.h"

namespace mu {

  PluckInst::PluckInst() :
    prev_note_(NULL),
    prev_start_(0),
    prev_gain_db_(0.0) {
    sequence_stream_ = new SequenceStream();
  }

  PluckInst::~PluckInst() {
    delete sequence_stream_;
    if (prev_note_ != NULL) delete prev_note_;
  }

  void PluckInst::pluck(MuTick start, MuStream *source, MuFloat gain_db) {
    MuTick elapsed = start - prev_start_;
    emit_prev_note(elapsed);
    prev_start_ = start;
    prev_note_ = source;
    prev_gain_db_ = gain_db;
  }

  void PluckInst::hammer(MuTick start, MuStream *source, MuFloat gain_db) {
    if (prev_note_ == NULL) {
      pluck(start, source, gain_db);
    } else {
      MuTick elapsed = start - prev_start_;
      CropStream *crop_stream = new CropStream();
      crop_stream->set_source(source);
      crop_stream->set_source_start(elapsed);
      emit_prev_note(elapsed);
      prev_note_ = crop_stream;
      // prev_start_ = prev_start_;
      prev_gain_db_ = gain_db;
    }
  }

  void PluckInst::dampen(MuTick start) {
    mu::MuTick elapsed = start - prev_start_;
    emit_prev_note(elapsed);
  }

  void PluckInst::emit_prev_note(mu::MuTick start) {
    if (prev_note_ == NULL) return;
    mu::CropStream *crop_stream = new mu::CropStream();
    crop_stream->set_source(prev_note_);
    crop_stream->set_source_end(start);
    sequence_stream_->add_source(crop_stream, 
                                 prev_start_, 
                                 MuUtils::db_to_ratio(prev_gain_db_));
    prev_note_ = NULL;
  }
  

}                               // namespace mu
