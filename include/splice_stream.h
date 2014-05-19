/*
 * SpliceStream accepts an arbitrary number of source streams, assigns
 * a time offset to each stream, and plays each stream in sequence,
 * terminating the previous stream when the next stream starts.
 * Compare to SequenceStream (which allows overlap).
 */
#ifndef MU_SPLICE_STREAM_H
#define MU_SPLICE_STREAM_H

#include "mu.h"
#include "multi_source_stream.h"

namespace mu {

  class SpliceStream : public MultiSourceStream {
  public:
    
    SpliceStream( void );
    ~SpliceStream( void );

    std::string getClassName() { return "SpliceStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    SpliceStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart();
    Tick getEnd();
    
    SpliceStream& addSource(Stream *source);
    SpliceStream& removesSource(Stream *source);
    SpliceStream& removeAllSources();

  protected:

    // ================================================================
    // private instance methods

    void resetCursor();

    void setupCursor(Tick time, Tick end);

    void sortSources();

    static bool sortPredicate(mu::Stream *s0, mu::Stream *s1);

    // ================================================================
    // private instance variables

    Tick prev_tick_;

    long int cursor_;
  };                            // class SpliceStream

}                               // namespace mu

#endif
