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

#ifndef MU_REVERB_STREAM
#define MU_REVERB_STREAM

#include "NRev.h"
#include "mu.h"
#include "stream.h"

namespace mu {

  // TODO: Make subclass of SingleSourceStream
  class ReverbStream : public Stream {
  public:

    ReverbStream( void );
    ~ReverbStream( void );

    std::string getClassName() { return "ReverbStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    ReverbStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getEnd();

    Stream *getSource() { return source_; }
    ReverbStream& setSource(Stream *source) { source_ = source; return *this; }

    stk::StkFloat getRatio( void ) { return wet_; }
    ReverbStream& setRatio(stk::StkFloat ratio) { 
      wet_ = ratio;
      dry_ = 1.0 - ratio;
      return *this;
    }
  
  protected:
    Stream *source_;
    stk::StkFrames buffer_;
    stk::StkFloat dry_;
    stk::StkFloat wet_;
    stk::NRev *reverb_;

  private:
    void setup() {
      if (reverb_ == NULL) {
        reverb_ = new stk::NRev();
        reverb_->setT60( 2.0 );
      }
      return;
    }

  }; // class ReverbStream
    
} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
