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
#include "splice_stream.h"

namespace mu {
  /*
   * SpliceStream(s) adds stream \c s to a collection of streams S.
   * Streams are sorted by getStart() time.  stream S[n] plays from
   * S[n].getStart() until S[n+1].getStart().  The last stream in the
   * sequence plays from S[n].getStart() until S[n].getEnd().
   *
   * The start of the SpliceStream is the minimum start time of the
   * sources.  The end of the SpliceStream is NOT the maximum start
   * time of the sources; it is the end time of the last source
   * stream, i.e. the source stream with the greatest start time.
   *
   * Note that there's a conceptual problem with SpliceStream: it
   * assumes that all source streams are fully deterministic, and that
   * their start (and end) times will not change.  Some streams, such
   * as RandomSelectStream, are non-deterministic.
   */

  // ================================================================
  // constructors

  SpliceStream::SpliceStream( void ) :
      prev_tick_ (kIndefinite),
      cursor_ (-1) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    }

  SpliceStream::~SpliceStream( void ) { 
  }

  // ================================================================
  // public instance methods

  void SpliceStream::inspectAux(std::stringstream& ss, int level) {
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
  void SpliceStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    // reset if needed
    if ((prev_tick_ == kIndefinite) || (tick <= prev_tick_)) { 
      setupCursor(tick, tick + buffer.frames()); 
    }
    prev_tick_ = tick;

    // simplify logic...
    zeroBuffer(buffer);

    // cursor_ is the lowest index into sources_ that sounds during
    // this buffer.  It will be advanced as each sources is consumed.

    Tick dst_start = tick;
    Tick dst_end = tick + buffer.frames();

    // Don't stop until we've filled the buffer or used all sources.
    while ((dst_start < dst_end) && (cursor_ < sources_.size())) {
      Stream* s = sources_.at(cursor_);
      bool is_last_source = (cursor_ == sources_.size() - 1);
      Tick src_start = s->getStart() == kIndefinite ? dst_start : s->getStart();
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
        s->step(buffer, tick, player);
      } else if (frames_to_write > 0) {
        // fill a transfer buffer and copy sub-segment
        buffer_.resize(frames_to_write, buffer.channels());
        s->step(buffer_, tmp_start, player);
        copyBuffer(buffer_, 0, buffer, tmp_start-tick, frames_to_write);
      }
      dst_start += frames_to_write;
      // if we have consumed all frames of current stream, advance to the next
      if (tmp_end == src_end) { 
        cursor_ += 1; 
      }
    }
  }

  Tick SpliceStream::getStart() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      return sources_.at(0)->getStart();
    }
  }

  Tick SpliceStream::getEnd() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      return sources_.at(sources_.size()-1)->getEnd();
    }
  }

  SpliceStream& SpliceStream::addSource(Stream *source) {
    MultiSourceStream::addSource(source);
    sortSources();
    resetCursor();
    return *this;
  }
  
  SpliceStream& SpliceStream::removesSource(Stream *source) {
    MultiSourceStream::removeSource(source);
    resetCursor();
    return *this;
  }
  
  SpliceStream& SpliceStream::removeAllSources() {
    MultiSourceStream::removeAllSources();
    resetCursor();
    return *this;
  }

  // ================================================================
  // protected instance methods

  void SpliceStream::resetCursor() {
    cursor_ = -1;
  }

  // Find the lowest indexed source stream that contributes samples
  // between start and end.  
  //
  // Implementation note: source streams are sorted by ascending start time
  // (although a start time of kIndefinite happens before all other
  // times). 
  void SpliceStream::setupCursor(Tick start, Tick end) {
    // TODO: fix this
    for (cursor_ = 0; cursor_<sources_.size(); cursor_++) {
      Stream *s = sources_.at(cursor_);
      bool is_last_source = (cursor_ == sources_.size() - 1);
      Tick src_start = s->getStart() == kIndefinite ? start : s->getStart();
      Tick src_end = is_last_source ? 
        s->getEnd() : 
        sources_.at(cursor_+1)->getStart();
      if ((src_end >= start) && (src_start < end)) { return; }
    }
    cursor_ = -1;
  }

  void SpliceStream::sortSources() {
    std::sort(sources_.begin(), sources_.end(), sortPredicate);
  }

  // A sorting predicate that sorts elements according to the start
  // time of each source.  A start time of kIndefinite sorts earlier
  // than all other times.
  bool SpliceStream::sortPredicate(mu::Stream *s0, mu::Stream *s1) {
    if (s0->getStart() == mu::kIndefinite) {
      return true;
    } else if (s1->getStart() == mu::kIndefinite) {
      return false;
    } else {
      return (s0->getStart() < s1->getStart());
    }
  }
  
} // namesmpace mu
