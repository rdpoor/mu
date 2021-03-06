/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/
#include "splice_sp.h"

namespace mu {
  /*
   * SpliceSP(s) adds stream \c s to a collection of streams S.
   * SPs are sorted by getStart() time.  stream S[n] plays from
   * S[n].getStart() until S[n+1].getStart().  The last stream in the
   * sequence plays from S[n].getStart() until S[n].getEnd().
   *
   * The start of the SpliceSP is the minimum start time of the
   * sources.  The end of the SpliceSP is NOT the maximum start
   * time of the sources; it is the end time of the last source
   * stream, i.e. the source stream with the greatest start time.
   *
   * Note that there's a conceptual problem with SpliceSP: it
   * assumes that all source streams are fully deterministic, and that
   * their start (and end) times will not change.  Some streams, such
   * as RandomSelectSP, are non-deterministic.
   */

  // ================================================================
  // constructors

  SpliceSP::SpliceSP( void ) :
      cursor_ (-1) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    }

  SpliceSP::~SpliceSP( void ) { 
  }

  // ================================================================
  // public instance methods

  void SpliceSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Inputs:" << std::endl;
    for (int i=0; i<sources_.size(); i++) {
      ss << sources_.at(i)->inspect(level+1);
    }
  }
  // TODO:
  // Some things to test:
  // infinite stream superceded by a short stream > 512 frames later
  // short stream, blank space > 512 frames, another short stream
  //
  void SpliceSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    // reset if needed
    if (is_new_event) {
      setupCursor(tick, tick + buffer.frames()); 
    }

    // simplify logic...
    zeroBuffer(buffer);

    // cursor_ is the lowest index into sources_ that sounds during
    // this buffer.  It will be advanced as each sources is consumed.

    Tick dst_start = tick;
    Tick dst_end = tick + buffer.frames();

    // Don't stop until we've filled the buffer or used all sources.
    while ((dst_start < dst_end) && (cursor_ < sources_.size())) {
      SampleProcessor *s = sources_.at(cursor_);
      bool is_last_source = (cursor_ == sources_.size() - 1);
      Tick src_start = TickUtils::isIndefinite(s->getStart()) ? dst_start : s->getStart();
      Tick src_end = is_last_source ? 
        s->getEnd() : sources_.at(cursor_+1)->getStart();

      Tick tmp_start = std::max(src_start, dst_start);
      Tick tmp_end = std::min(src_end, dst_end);

      long int frames_to_write = tmp_end - tmp_start;

//       fprintf(stderr,"ds=%ld,de=%ld,ss=%ld,se=%ld,ts=%ld,te=%ld,ftw=%ld\n",
//               dst_start, dst_end, src_start, src_end, tmp_start, tmp_end, 
//               frames_to_write);

      if (frames_to_write == buffer.frames()) {
        // current stream spans entire buffer -- copy directly
        s->step(buffer, tick, is_new_event);
      } else if (frames_to_write > 0) {
        // fill a transfer buffer and copy sub-segment
        buffer_.resize(frames_to_write, buffer.channels());
        s->step(buffer_, tmp_start, is_new_event);
        copyBuffer(buffer_, 0, buffer, tmp_start-tick, frames_to_write);
      }
      dst_start += frames_to_write;
      // if we have consumed all frames of current stream, advance to the next
      if (tmp_end == src_end) { 
        cursor_ += 1; 
      }
    }
  }

  Tick SpliceSP::getStart() {
    if (sources_.size() == 0) {
      return TickUtils::indefinite();
    } else {
      return sources_.at(0)->getStart();
    }
  }

  Tick SpliceSP::getEnd() {
    if (sources_.size() == 0) {
      return TickUtils::indefinite();
    } else {
      return sources_.at(sources_.size()-1)->getEnd();
    }
  }

  SpliceSP& SpliceSP::addSource(SampleProcessor *source) {
    MultiSourceSP::addSource(source);
    sortSources();
    resetCursor();
    return *this;
  }
  
  SpliceSP& SpliceSP::removesSource(SampleProcessor *source) {
    MultiSourceSP::removeSource(source);
    resetCursor();
    return *this;
  }
  
  SpliceSP& SpliceSP::removeAllSources() {
    MultiSourceSP::removeAllSources();
    resetCursor();
    return *this;
  }

  // ================================================================
  // protected instance methods

  void SpliceSP::resetCursor() {
    cursor_ = -1;
  }

  // Find the lowest indexed source stream that contributes samples
  // between start and end.  
  //
  // Implementation note: source streams are sorted by ascending start time
  // (although a start time of TickUtils::indefinite() happens before all other
  // times). 
  void SpliceSP::setupCursor(Tick start, Tick end) {
    // TODO: fix this
    for (cursor_ = 0; cursor_<sources_.size(); cursor_++) {
      SampleProcessor *s = sources_.at(cursor_);
      bool is_last_source = (cursor_ == sources_.size() - 1);
      Tick src_start = TickUtils::isIndefinite(s->getStart()) ? start : s->getStart();
      Tick src_end = is_last_source ? 
        s->getEnd() : 
        sources_.at(cursor_+1)->getStart();
      if ((src_end >= start) && (src_start < end)) { return; }
    }
    cursor_ = -1;
  }

  void SpliceSP::sortSources() {
    std::sort(sources_.begin(), sources_.end(), sortPredicate);
  }

  // A sorting predicate that sorts elements according to the start
  // time of each source.  A start time of TickUtils::indefinite()
  // sorts earlier than all other times.
  bool SpliceSP::sortPredicate(mu::SampleProcessor *s0, mu::SampleProcessor *s1) {
    if (mu::TickUtils::isIndefinite(s0->getStart())) {
      return true;
    } else if (mu::TickUtils::isIndefinite(s1->getStart())) {
      return false;
    } else {
      return (s0->getStart() < s1->getStart());
    }
  }
  
} // namesmpace mu
