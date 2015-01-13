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
#include "loop_rs.h"
#include <stdio.h>

namespace mu {

  LoopRS::LoopRS() :
    duration_ ( kDefaultDuration ) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }
  
  LoopRS::~LoopRS() {
  }
  
  // loop_size = 7
  // render(0, 0, 20) =>   [00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19]
  //                       [00 01 02 03 04 05 06 00 01 02 03 04 05 06 00 01 02 03 04 05] 
  //    render(0, 0, 7)
  //    render(-7, 0, 7)
  //    render(-14, 0, 6)
  //
  // render(20, 20, 40) => [20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39]
  //                       [06 00 01 02 03 04 05 06 00 01 02 03 04 05 06 00 01 02 03 04]
  //    render(6, 6, 7)    tick=20, src_start=6, avail=1, ?=src_start+tick-base = 6
  //    render(-1, 0, 7)   tick=21, src_start=0, avail=7, ?=src_start+tick-base = -1
  //    render(-8, 0, 7)   tick=28, src_start=0, avail=7, ?=src_start+tick-base = -8
  //    render(-15, 0, 5)  tick=35, src_start=0, avail=5, ?=src_start+tick-base = -15
  //

  void LoopRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (source_ == NULL) return;

    // printf("LoopRS::render(%p, %ld, %ld, %ld)\n", &frames, base_tick, start_tick, end_tick);
    MuTick tick = start_tick;
    while (tick < end_tick) {
      MuTick src_start = tick % duration_;
      MuTick src_base = src_start + base_tick - tick;
      int avail = std::min(duration_ - src_start, end_tick - tick);
      MuTick src_end = src_start + avail;
      // avail = how many samples can be fetched in one call to render
      // src_start = tick of first sample to be fetched 
      // src_base = effective tick of frames[0]
      // printf("  LoopRS::render(%p, %ld, %ld, %ld) [avail=%d]\n", &frames, src_base, src_start, src_end, avail);
      source_->render(frames, src_base, src_start, src_end);
      tick += avail;
    }

  }


}
