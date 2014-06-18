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
#include "stream.h"

namespace mu {

  // return a string describing this stream and all of its inputs.
  std::string Stream::inspect(int level) {
    std::stringstream ss;
    inspectIndent(ss, level); 
    ss << "#<" << getClassName() << " " << this << ">" << std::endl;
    inspectIndent(ss, level+1); 
    if (getStart() == kIndefinite) {
      ss << "getStart() = " << "kIndefinite" << std::endl;
    } else {
      ss << "getStart() = " << getStart() << std::endl;
    }      
    inspectIndent(ss, level+1); 
    // ss << "getStart() = " << ((getEnd()==kIndefinite)?"kIndefinite":getEnd()) << std::endl;
    if (getEnd() == kIndefinite) {
      ss << "getEnd() = " << "kIndefinite" << std::endl;
    } else {
      ss << "getEnd() = " << getEnd() << std::endl;
    }      
    inspectAux(ss, level+1);
    return ss.str();
  }

  void Stream::inspectIndent(std::stringstream& ss, int level) {
    for (int i=0; i<level; i++) { ss << "| "; }
  }

  void Stream::inspectAux(std::stringstream& ss, int level) {
  }
  
}
