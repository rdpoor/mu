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
#include "identity_sp.h"

namespace mu {

  IdentitySP::IdentitySP() {
    TRACE("IdentitySP::IdentitySP()\n");
  }
  
  IdentitySP::~IdentitySP() {
    TRACE("IdentitySP::~IdentitySP()\n");
  }
  
  void IdentitySP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    Tick frame_count = buffer.frames();
    Tick channel_count = buffer.channels();
    
    Tick f = tick;
    switch (channel_count) {
    case 1:
      for (Tick i=0; i<frame_count; i++, f++) {
        buffer[i] = f;
      }
      break;
    case 2:
      for (Tick i=0; i<frame_count; i++, f++) {
        buffer[2*i] = f;
        buffer[2*i+1] = f;
      }
      break;
    default:
      for (Tick i=0; i<frame_count; f++) {
        for (unsigned chan = 0; chan<channel_count; chan++) {
          buffer[channel_count * i + chan] = f;
        }
      }
    }
  }

}
