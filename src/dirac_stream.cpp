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
#include "dirac_stream.h"

namespace mu {

  DiracStream::DiracStream() {
  }
  
  DiracStream::~DiracStream() {
    // printf("~DiracStream()\n");
  }
  
  DiracStream *DiracStream::clone() {
    DiracStream *c = new DiracStream();
    return c;
  }

  bool DiracStream::render(MuTick buffer_start, MuBuffer *buffer) {
    int n_frames = buffer->frames();
    int n_channels = buffer->channels();
    
    for (int frame = n_frames-1; frame >= 0; frame--) {
      MuFloat v = ((frame + buffer_start) == 0) ? 1.0 : 0.0;
      for (int channel = n_channels-1; channel >= 0; channel--) {
        (*buffer)(frame, channel) = v;
      }
    }
    return true;
  }

  void DiracStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
  }


}                               // namespace mu
