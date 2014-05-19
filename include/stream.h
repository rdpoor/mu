#ifndef MU_STREAM_H
#define MU_STREAM_H

#include "mu.h"
#include "player.h"
#include <iostream>

namespace mu {

  
  class Stream {
  public:

    virtual Stream& step(stk::StkFrames& buffer, Tick tick, Player &player) = 0;
    
    // define the extent of this stream: when it starts and ends
    virtual Tick getStart( void ) { return kIndefinite; }
    virtual Tick getEnd( void ) { return kIndefinite; }
    Tick getDuration( void ) {     
      return ((getStart()==kIndefinite) || (getEnd()==kIndefinite)) ? kIndefinite : (getEnd() - getStart());
    }

    virtual std::string getClassName() { return "Stream"; }
    std::string inspect(int level = 0);
    void inspectIndent(std::stringstream& ss, int level);
    // each stream subclass should define an inspect_aux method
    // that prints this node and all sub-nodes
    virtual void inspectAux(std::stringstream& ss, int level);

    // Return true if tick falls within start (inclusive) and end
    // (exclusive) of this stream.
    bool doesContain( Tick tick ) {
      bool after_start = (getStart() == kIndefinite) || (tick >= getStart());
      bool before_end = (getEnd() == kIndefinite) || (tick < getEnd());
      return after_start && before_end;
    }
    
    // utilities
    
    // zero out a buffer
    Stream& zeroBuffer(stk::StkFrames& b) {
#if 0
      bzero(&(b[0]), b.frames() * b.channels() * sizeof(stk::StkFloat));
#else
      for (int i=0; i<b.frames(); i++) {
        for (int j=0; j<b.channels(); j++) {
          b(i,j) = 0.0;
        }
      }
#endif
      return *this;
    }
    
    // copy src[src_offset] into dst[dst_offset] for n_frames
    Stream& copyBuffer(stk::StkFrames& src, 
                       Tick src_offset, 
                       stk::StkFrames& dst, 
                       Tick dst_offset, 
                       Tick n_frames) {
#if 0
      memcpy(&(dst(dst_offset,0)),
             &(src(src_offset,0)), 
             n_frames * src.channels() * sizeof(stk::StkFloat));
#else
      for (int i=0; i<n_frames; i++) {
        if (((i+dst_offset) > dst.frames()) || ((i+dst_offset) < 0)) {
          fprintf(stderr, "dst index error in copyBuffer: 0 <= %ld < %d\n",
                  (i + dst_offset), dst.frames());
        }
        if (((i+src_offset) > src.frames()) || ((i+src_offset) < 0)) {
          fprintf(stderr, "src index error in copyBuffer: 0 <= %ld < %d\n",
                  (i + src_offset), src.frames());
        }
        for (int j=src.channels() - 1; j>=0; j--) {
          dst(i+dst_offset, j) = src(i+src_offset, j);
        }
      }
#endif
      return *this;
    }

  };                            // class Stream

} // namespace mu



#endif
