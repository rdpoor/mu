/*
 * SpliceStream accepts an arbitrary number of source streams, assigns
 * a time offset to each stream, and plays each stream in sequence,
 * terminating the previous stream when the next stream starts.
 * Compare to SequenceStream (which allows overlap).
 */
#ifndef MU_SPLICE_STREAM_H
#define MU_SPLICE_STREAM_H

#include "mu.h"
#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"

namespace mu {

  class SpliceStream : public AddStream {
  public:
    
    SpliceStream( void ) :
      prev_tick_ (kIndefinite) {
    }

    ~SpliceStream( void ) { }

    std::string getClassName() { return "SpliceStream"; }

    SpliceStream& step(stk::StkFrames& buffer, Tick tick, Player &player) {
      if ((prev_tick_ == kIndefinite) || (tick <= prev_tick_)) { reset(); }
      prev_tick_ = tick;
      AddStream::step(buffer, tick, player);
      // TODO: if the signature of step() was Stream& step(), then we could do
      // return AddStrem::step(buffer, tick, player) and (maybe) get the benefits
      // of tail recursion.
      return *this;
    }

    /*
     * Implementation note: Each time we add a stream, we actually add:
     *
     * source => delay => crop 
     * 
     * Upon reset (each time the stream starts or backs up), we sort
     * inputs by increasing delay time, and crop stream N-1 to end at the
     * delay time of the Nth stream.
     */
    SpliceStream& addSource(Stream *source, Tick delay) {
      DelayStream *delay_stream = &((new DelayStream())->setSource(source).setDelay(delay));
      CropStream *crop_stream = &((new CropStream())->setSource(delay_stream));
      AddStream::addSource(crop_stream);
      return *this;
    }

  protected:
    Tick prev_tick_;

    // A sorting predicate that sorts elements according to the delay
    // applied to the source stream.
    static bool sortByDelay(mu::Stream *s0, mu::Stream *s1) {
      // We can get away with these typecasts because we built the
      // chain of streams in the addSource() method.
      CropStream *c0 = (CropStream *)s0;
      CropStream *c1 = (CropStream *)s1;
      DelayStream *d0 = (DelayStream *)c0->getSource();
      DelayStream *d1 = (DelayStream *)c1->getSource();

      if (d0->getDelay() == mu::kIndefinite) {
        return true;
      } else if (d1->getDelay() == mu::kIndefinite) {
        return false;
      } else {
        return (d0->getDelay() < d1->getDelay());
      }
    }
    
    // dly  crop
    // 0    indef, 10
    // 10   indef, 100
    // 100  indef, indef
    void reset() {
      if (sources_.size() == 0) return;

      std::sort(sources_.begin(), sources_.end(), sortByDelay);
      Tick end_tick = kIndefinite;
      for (int i=sources_.size()-1; i>=0; i--) {
        CropStream *c = (CropStream *)sources_.at(i);
        DelayStream *d = (DelayStream *)c->getSource();
        c->setEnd(end_tick);
        end_tick = d->getDelay();
      }
    }

  };                            // class SpliceStream

}                               // namespace mu

#endif
