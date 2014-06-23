/*
 * ChannelizerStream fits mono sources to stereo sinks and vice versa.
 * It really should not be necessary.  At the very least, a stream
 * should announce how many channels it provides so we don't have to
 * set it manually.
 */
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

#ifndef MU_CHANNELIZER_STREAM_H
#define MU_CHANNELIZER_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class ChannelizerStream : public SingleSourceStream<ChannelizerStream> {
  public:

    ChannelizerStream( void );
    ~ChannelizerStream( void );

    std::string getClassName() { return "ChannelizerStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, Player &player);

    unsigned int getSourceChannelCount() const { return source_channel_count_; }
    ChannelizerStream& setSourceChannelCount(Tick source_channel_count) { 
      source_channel_count_ = source_channel_count; 
      return *this; 
    }

    
  protected:
    stk::StkFrames tmp_buffer_;
    unsigned int source_channel_count_;

  };                            // class ChannelizerStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
