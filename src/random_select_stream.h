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
 * RandomSelectStream chooses 1 of N sources whenever the stream resets (time
 * goes to zero). When adding a source stream, you also specify a relative
 * weight, so:
 *
 *   random_select_stream->add_source(orange, 3.0);
 *   random_select_stream->add_source(purple, 1.0);
 *
 * will play orange with a 75% probability and purple with a 25% probabilty.
 *
 * NOTE: As written, there's no possibilty for overlap among source streams: a
 * new stream replaces the incumbent whenever the time resets to zero.  An
 * alternate possibilty would be to have each source stream, once selected, play
 * to completion.  This would require (at least) a sum_stream object (so sum
 * multiple streams) and a change to the API (to define the length of the source
 * stream).
 */

#ifndef MU_RANDOM_SELECT_STREAM_H
#define MU_RANDOM_SELECT_STREAM_H

#include "mu_types.h"
#include "sum_stream.h"
#include <random>

namespace mu {

  class RandomSelectStream : public MuStream {
  public:
    
    RandomSelectStream( void );
    virtual ~RandomSelectStream( void );
    virtual RandomSelectStream *clone( void );
    
    void add_source(MuStream *source, MuFloat probability);

    bool render(MuTick buffer_start, MuBuffer *buffer);
    
    std::string get_class_name() { return "RandomSelectStream"; }

  private:
    class StreamDescriptor {
    public:
      StreamDescriptor(MuStream *source, MuFloat probability) :
        source_(source), 
        probability_(probability) {
      }
      ~StreamDescriptor( void ) {
        if (source_) delete source_;
      }
      MuStream *source() { return source_; }
      MuFloat probability() { return probability_; }
    protected:
      MuStream *source_;
      MuFloat probability_;
    };

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    MuTick prev_buffer_end_;
    MuTick needs_recompute_;
    std::vector<StreamDescriptor *> stream_descriptors_;
    MuStream *current_stream_;
    std::default_random_engine random_generator_;
    std::discrete_distribution<int> *discrete_distribution_;
    

  private:
    void recompute( void );     // recompute probabilities from stream descriptors
    void select( void );        // select a new stream

  };                            // class RandomSelectStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
