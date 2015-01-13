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
#include "add_rs.h"

namespace mu {

  AddRS::AddRS() {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  AddRS::~AddRS() {
  }

  void AddRS::render(stk::StkFrames& frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    bool is_first = true;

    for (int i=sources_.size()-1; i>=0; i--) {
      RenderStream *source = sources_.at(i);
      buffer_.resize(frames.frames(), frames.channels());
      
      if (is_first) {
        // The first source can be rendered directly into frames
        source->render(frames, base_tick, start_tick, end_tick);
        is_first = false;
      } else {
        // Subsequent sources render into temp buffer and sum into frames
        source->render(buffer_, base_tick, start_tick, end_tick);
        for (int i=start_tick; i<end_tick; i++){
          for (int j=buffer_.channels()-1; j>=0; j--) {
            frames(frame_index(base_tick,i),j) += buffer_(frame_index(base_tick,i),j);
          } // for j
        }   // for i
      }     // else
    }       // for
  }

}
