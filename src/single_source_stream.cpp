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
#include "single_source_stream.h"

namespace mu {

  SingleSourceStream::SingleSourceStream() 
    : source_(NULL) {
  }

  SingleSourceStream::~SingleSourceStream() {
    // printf("~SingleSourceStream()\n");
    if (source_) delete source_;
  }

#if 0
  SingleSourceStream *SingleSourceStream::clone() {
    SingleSourceStream *c = new SingleSourceStream();
    c->set_source(source_ ? source_->clone() : source_);
    return c;
  }
#endif
     
  void SingleSourceStream::inspect_aux(int level, std::stringstream *ss) {
    inspect_indent(level, ss);
    *ss << "source()" << std::endl;
    source()->inspect_aux(level+1, ss);
  }

}                               // namespace mu
