/*
 * A long shot for a sketch: start with the code I want to write, then
 * see if we can make it work.
 *
 * A Stream has two methods:
 *   first() - a StreamElement
 *   rest() - a Stream that will deliver the next or NULL_STREAM
 */

typedef double TimeType;

template <typename T> class TimedStreamElement {
public:
  virtual TimeType getStart() { return start_; }
  virtual T& setStart( TimeType start ) { start_ = start; return *static_cast<T *>(this); }
  virtual TimeType getEnd() { return end_; }
  virtual T& setEnd( TimeType end ) { end_ = end; return *static_cast<T *>(this); }
  virtual TimeType getDuration() { return end_ - start_; }
  virtual T& setDuration( TimeType duration ) { end_ = start_ + duration; return *static_cast<T *>(this); }
protected:
  TimeType start_;
  TimeType end_;
  }

int main( void ) {
  TimedStream *sf1 = TimedStream::cons(new SoundFile("s1.wav"), NULL_STREAM);
  TimedStream *sf2 = TimedStream::cons(new SoundFile("s2.wav"), NULL_STREAM)->delay(2.0);
  TimedStream *sf3 = Timedstream::cons(new SoundFile("s3.wav"), NULL_STREAM)->delay(3.0);

  TimedStream *s1 = new MergeStream(TimedStream::sortByStartTime, sf1, sf2, sf3);
  TimedStream *s2 = s1->loop(5.0)->crossFade(0.05);
  Player player.setSource(s2);
  player.run();

  return 0;
}

