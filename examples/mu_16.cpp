// Too much stuff for one sketch...

#include "mu.h"
#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define THUMPS_AND_SCRATCHES_DIRECTORY "/Users/r/Projects/Mu/SoundSets/TAS/"
#define PLUCKED_NOTE_DIRECTORY "/Users/r/Projects/Mu/SoundSets/A/"
#define EXAMPLES_DIRECTORY "/Users/r/Projects/Mu/examples/"

mu::MuFloat beats_per_minute() {
  return 100.0;
}

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  mu::MuFloat tics_per_beat = 44100 * 60.0 / beats_per_minute();
  return beat * tics_per_beat;
}

mu::MuFloat pitch_to_ratio(mu::MuFloat sample, mu::MuTick tick) {
  (void) tick;
  return mu::MuUtils::pitch_to_ratio(sample);
}

// ================================================================
class PitchShift {
public:
  PitchShift() {
    identity_stream_ = new mu::IdentityStream();
    gain_stream_ = new mu::GainStream();
    resample_stream_ = new mu::ResampleStream();
    gain_stream_->set_signal_source(identity_stream_);
    resample_stream_->set_timing_source(gain_stream_);
    set_shift(1.0);
  }
  ~PitchShift() {
  }
  mu::MuFloat shift() {
    return mu::MuUtils::ratio_to_pitch(gain_stream_->gain());
  }
  void set_shift(mu::MuFloat semitones) {
    gain_stream_->set_gain(mu::MuUtils::pitch_to_ratio(semitones));
  }
  mu::MuStream *source() {
    return resample_stream_->sample_source();
  }
  void set_source(mu::MuStream *source) {
    resample_stream_->set_sample_source(source);
  }
  mu::MuStream *stream() {
    return resample_stream_;
  }

protected:
  mu::IdentityStream *identity_stream_;
  mu::GainStream *gain_stream_;
  mu::ResampleStream *resample_stream_;
};

// ================================================================
class Basic {
public:
  Basic() {
    sequence_stream_ = new mu::SequenceStream();
  }

  void add(mu::MuFloat t, 
           std::string name, 
           mu::MuFloat gain, 
           mu::MuFloat pitch) {
    // leaks PitchShift objects!
    PitchShift *ps = new PitchShift();
    ps->set_source(frs(name));
    ps->set_shift(pitch);
    sequence_stream_->add_source(ps->stream(), beat_to_tick(t), gain);
  }

  mu::MuStream *stream() {
    return sequence_stream_;
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(EXAMPLES_DIRECTORY + name);
    return s;
  }

protected:
  mu::SequenceStream *sequence_stream_;
};

// ================================================================
class ProbabilityStream: public mu::SingleSourceStream {
public:
  ProbabilityStream() : 
    probability_(0.5), 
    current_p_(0.5),
    prev_start_(-1) { }

  virtual ~ProbabilityStream() { }

  ProbabilityStream *clone() {
    ProbabilityStream *c = new ProbabilityStream();
    c->set_source(mu::MuUtils::clone_stream(source()));
    c->set_probability(probability());
    return c;
  }

  mu::MuFloat probability() { return probability_; }
  void set_probability(mu::MuFloat probability) { probability_ = probability; }

  std::string get_class_name() { return "ProbabilityStream"; }

  void inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    *ss << "probability() = " << probability() << std::endl;
    SingleSourceStream::inspect_aux(level, ss);
  }

  bool render(mu::MuTick buffer_start, mu::MuBuffer *buffer) {
    int n_frames = buffer->frames();
    if (buffer_start != prev_start_ + n_frames) {
      // time jumped...
      current_p_ = (double)rand() / RAND_MAX; 
    }
    prev_start_ = buffer_start;
#if 1
    bool ret;
    // printf("%p: p=%f, q=%f, r=%f\n", this, probability_, current_p_);
    if (probability_ <= current_p_) {
      ret = false;
    } else if (source_ == NULL) {
      ret = false;
    } else {
      ret = source_->render(buffer_start, buffer);
    }
    return ret;
#else
    return source_->render(buffer_start, buffer);
#endif
  }
protected:
  mu::MuFloat probability_;
  mu::MuFloat current_p_;
  mu::MuTick prev_start_;
};

// ================================================================
class Swish {
public:
  Swish() {
    sequence_stream_ = new mu::SequenceStream();
  }
  void add(mu::MuFloat t, std::string name, mu::MuFloat p, mu::MuFloat gain) {
    mu::FileReadStream *frs = new mu::FileReadStream();
    frs->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY + name);
    ProbabilityStream *ps = new ProbabilityStream();
    ps->set_source(frs);
    ps->set_probability(p);
    sequence_stream_->add_source(ps, beat_to_tick(t), gain);
  }
  mu::MuStream *stream() {
    return sequence_stream_;
  }
    
protected:
  mu::SequenceStream *sequence_stream_;
};

// ================================================================
class Metronome {
public:
  Metronome() {
    thump_ = new mu::FileReadStream();
    thump_->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY "s12.wav");
  }
  mu::MuStream *stream() {
    return thump_;
  }
protected:
  mu::FileReadStream *thump_;
};                              // class Metronome
  
//================================================================
// ================================================================
class RoomTone {
public:
  RoomTone() {
    room_ = new mu::FileReadStream();
    loop_ = new mu::LoopStream();
    room_->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY "room_tone.wav");
    loop_->set_source(room_);
    loop_->set_interval(room_->duration());
    loop_->set_source_end(room_->duration());
  }
    
  mu::MuStream *stream() {
    return loop_;
  }
    
protected:
  mu::FileReadStream *room_;
  mu::LoopStream *loop_;
};                              // class RoomTone

// ================================================================
// pluck, hammer on / off, mute a note.  The "note" is a stream, but
// usually a plucked string sound file.  Note that a final dampen()
// must be called to flush the final note.
class PString {
public:
  PString() {
    sequence_stream_ = new mu::SequenceStream();
    prev_note_ = NULL;
  }
  ~PString() {
    delete sequence_stream_;
    if (prev_note_ != NULL) delete prev_note_;
  }

  // start playing a note
  void pluck(mu::MuStream *source, mu::MuTick start, mu::MuFloat gain = 1.0) {
    mu::MuTick elapsed = start - prev_start_;
    flush_prev(elapsed);
    prev_note_ = source;
    prev_start_ = start;
    prev_gain_ = gain;
  }

  // truncate the previous note, start the new note 'd' seconds after its
  // start, where 'd' is the time elapsed between previous note and this.
  void hammer(mu::MuStream *source, mu::MuTick start, mu::MuFloat gain = 1.0) {
    if (prev_note_ == NULL) {
      pluck(source, start, gain);
    } else {
      mu::MuTick elapsed = start - prev_start_;
      mu::CropStream *c = new mu::CropStream();
      c->set_source(source);
      c->set_source_start(elapsed);
      flush_prev(elapsed);
      prev_note_ = c;
      // prev_start_ = prev_start_;
      prev_gain_ = gain;
    }
  }

  // stop playing the current note.  In actuality, this emits the previous
  // note, and you must call dampen() to cause the final note to be emitted.
  void dampen(mu::MuTick start) {
    mu::MuTick elapsed = start - prev_start_;
    flush_prev(elapsed);
  }

  // return the stream with the collection of plucked, hammered and dampened
  // notes.
  mu::MuStream *stream() {
    return sequence_stream_;
  }
protected:
  mu::MuStream *prev_note_;
  mu::MuTick prev_start_;
  mu::MuFloat prev_gain_;
  mu::SequenceStream *sequence_stream_;
  void flush_prev(mu::MuTick start) {
    if (prev_note_ != NULL) {
      mu::CropStream *c = new mu::CropStream();
      c->set_source(prev_note_);
      c->set_source_end(start);
      sequence_stream_->add_source(c, prev_start_, prev_gain_);
      prev_note_ = NULL;
    }
  }
};

// ================================================================
class Plucker {
public:
  Plucker() {
    pstring_ = new PString();
  };
  ~Plucker() {};
  void pluck(mu::MuFloat t, 
             std::string name, 
             mu::MuFloat gain) {
    pstring_->pluck(frs(name), beat_to_tick(t), gain);
  }
  void hammer(mu::MuFloat t, 
              std::string name, 
              mu::MuFloat gain) {
    pstring_->hammer(frs(name), beat_to_tick(t), gain);
  }
  void dampen(mu::MuFloat t) {
    pstring_->dampen(beat_to_tick(t));
  }
  mu::MuStream *stream() {
    return pstring_->stream();
  }
protected:
  PString *pstring_;

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(PLUCKED_NOTE_DIRECTORY + name);
    return s;
  }

};

// ================================================================
class Plucks2 {
public:
  mu::MuStream *setup() {
    PString *p = new PString();
    p->pluck(frs("67.wav"), beat_to_tick(0.00), 0.44);
    p->pluck(frs("67.wav"), beat_to_tick(0.75), 0.44);
    p->pluck(frs("72.wav"), beat_to_tick(1.50), 0.44);
    p->hammer(frs("74.wav"), beat_to_tick(1.65), 0.44);
    p->pluck(frs("72.wav"), beat_to_tick(2.50), 0.44);
    p->pluck(frs("72.wav"), beat_to_tick(3.00), 0.44);
    p->pluck(frs("72.wav"), beat_to_tick(4.00), 0.44);
    p->hammer(frs("74.wav"), beat_to_tick(4.15), 0.44);
    p->pluck(frs("72.wav"), beat_to_tick(4.75), 0.44);
    p->pluck(frs("74.wav"), beat_to_tick(5.50), 0.44);
    p->pluck(frs("74.wav"), beat_to_tick(6.50), 0.44);
    p->pluck(frs("74.wav"), beat_to_tick(7.00), 0.44);
    p->dampen(beat_to_tick(8.00));

    PString *q = new PString();
    q->pluck(frs("60.wav"), beat_to_tick(3.50), 0.44);
    q->dampen(beat_to_tick(5.50));

    mu::SumStream *sstream = new mu::SumStream();
    sstream->add_source(p->stream());
    sstream->add_source(q->stream());

    return sstream;
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(PLUCKED_NOTE_DIRECTORY + name);
    return s;
  }

};

class Plucks3 {
public:
  mu::MuStream *setup() {
    PString *p = new PString();
    p->pluck(frs("31.wav"), beat_to_tick(0.00), 0.5);
    p->pluck(frs("36.wav"), beat_to_tick(1.50), 0.5);
    p->pluck(frs("29.wav"), beat_to_tick(4.00), 0.5);
    p->pluck(frs("38.wav"), beat_to_tick(5.50), 0.5);
    p->dampen(beat_to_tick(8.00));

    return p->stream();
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(PLUCKED_NOTE_DIRECTORY + name);
    return s;
  }

};

// ================================================================
    
int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  Metronome *metronome = new Metronome();
  Basic *basic = new Basic();
  Swish *swish = new Swish();
  RoomTone *room_tone = new RoomTone();
  Plucker *plucker_alto = new Plucker();
  Plucker *plucker_alto2 = new Plucker();
  Plucker *plucker_bass = new Plucker();
  mu::SumStream *mix = new mu::SumStream();
  mu::LoopStream *loop = new mu::LoopStream();

  printf("1 beat = %ld ticks\n", beat_to_tick(1));

#define KICK_GAIN 1.0
#define CLICK1_GAIN 0.30
#define CLICK2_GAIN 0.30
#define BUMP_GAIN 0.30
#define ALTO_GAIN 0.25
#define BASS_GAIN 0.35

  basic->add(0.00, "thump.wav", KICK_GAIN, -24.0);
  basic->add(1.50, "thump.wav", KICK_GAIN, -24.0);
  basic->add(3.00, "thump.wav", KICK_GAIN, -24.0);
  basic->add(3.50, "thump.wav", KICK_GAIN/2.0, -24.0);
  basic->add(4.00, "thump.wav", KICK_GAIN, -24.0);
  basic->add(5.50, "thump.wav", KICK_GAIN, -24.0);
  basic->add(7.50, "thump.wav", KICK_GAIN/2.0, -24.0);

  swish->add(0.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(0.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(1.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(1.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(2.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(2.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(3.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(3.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(4.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(4.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(5.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(5.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(6.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(6.50, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(7.00, "s21.wav", 0.67, CLICK1_GAIN);
  swish->add(7.50, "s21.wav", 0.67, CLICK1_GAIN);

  swish->add(0.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(0.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(1.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(1.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(2.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(2.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(3.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(3.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(4.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(4.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(5.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(5.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(6.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(6.75, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(7.25, "s20.wav", 0.67, CLICK2_GAIN);
  swish->add(7.75, "s20.wav", 0.67, CLICK2_GAIN);

  swish->add(0.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(1.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(2.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(3.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(4.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(5.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(6.50, "s14.wav", 0.67, BUMP_GAIN);
  swish->add(7.50, "s14.wav", 0.67, BUMP_GAIN);

  // alto part
  plucker_alto->pluck(0.00, "67.wav", ALTO_GAIN);
  plucker_alto->pluck(0.75, "67.wav", ALTO_GAIN);
  plucker_alto->pluck(1.50, "72.wav", ALTO_GAIN);
  plucker_alto->hammer(1.65, "74.wav", ALTO_GAIN);
  plucker_alto->pluck(2.50, "72.wav", ALTO_GAIN);
  plucker_alto->pluck(3.00, "72.wav", ALTO_GAIN);
  plucker_alto->pluck(4.00, "72.wav", ALTO_GAIN);
  plucker_alto->hammer(4.15, "74.wav", ALTO_GAIN);
  plucker_alto->pluck(4.75, "72.wav", ALTO_GAIN);
  plucker_alto->pluck(5.50, "74.wav", ALTO_GAIN);
  plucker_alto->pluck(6.50, "74.wav", ALTO_GAIN);
  plucker_alto->pluck(7.00, "74.wav", ALTO_GAIN);
  plucker_alto->dampen(8.00);

  plucker_alto2->pluck(3.50, "60.wav", ALTO_GAIN);
  plucker_alto2->dampen(5.50);

  // bass part
  plucker_bass->pluck(0.00, "31.wav", BASS_GAIN);
  plucker_bass->pluck(1.50, "36.wav", BASS_GAIN);
  plucker_bass->pluck(4.00, "29.wav", BASS_GAIN);
  plucker_bass->pluck(5.50, "38.wav", BASS_GAIN);
  plucker_bass->dampen(8.00);

  // mix->add_source(metronome->stream());
  mix->add_source(basic->stream());
  mix->add_source(swish->stream());
  mix->add_source(room_tone->stream());
  mix->add_source(plucker_alto->stream());
  mix->add_source(plucker_alto2->stream());
  mix->add_source(plucker_bass->stream());

  loop->set_source(mix);
  loop->set_interval(beat_to_tick(8.0));
  loop->set_source_end(beat_to_tick(8.0));

  // printf("mix:\n%s\n", mix->inspect().c_str());

  transport.set_player(&player_rt);
  transport.set_source(loop);

  transport.run();
  wait_for_input();
  transport.stop();
  
  return 0;
}
