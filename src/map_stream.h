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

/*
 * MapStream calls a user-supplied function for each sample from a
 * source and passes the result to its output.  This makes MapStream
 * useful for processing data as well as printing or analyzing the
 * samples.
 */

#ifndef MU_MAP_STREAM_H
#define MU_MAP_STREAM_H

#include "mu_types.h"
#include "single_source_stream.h"

namespace mu {

  typedef MuFloat (MapStreamCallback)(MuFloat sample, MuTick tick);
  
  class MapStream : public SingleSourceStream {
  public:

    MapStream( void );
    virtual ~MapStream( void );
    virtual MapStream *clone( void );

    MapStreamCallback *callback() { return callback_; }
    void set_callback(MapStreamCallback *callback) { callback_ = callback; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "MapStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    MapStreamCallback *callback_;

  };                            // class MapStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
