// Sketch for drum patterns

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
    // loop_stream_ = new mu::LoopStream();
    // loop_stream_->set_source(sequence_stream_);
    // loop_stream_->set_interval(beat_to_tick(8));
    // loop_stream_->set_source_end(beat_to_tick(8));
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
    // return loop_stream_;
    return sequence_stream_;
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(EXAMPLES_DIRECTORY + name);
    return s;
  }

protected:
  mu::SequenceStream *sequence_stream_;
  // mu::LoopStream *loop_stream_;
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
    // loop_stream_ = new mu::LoopStream();
    // loop_stream_->set_source(sequence_stream_);
    // loop_stream_->set_interval(beat_to_tick(8));
    // loop_stream_->set_source_end(beat_to_tick(8));
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
    // return loop_stream_;
    return sequence_stream_;
  }
    
protected:
  mu::SequenceStream *sequence_stream_;
  // mu::LoopStream *loop_stream_;
};

// ================================================================
class Metronome {
public:
  Metronome() {
    thump_ = new mu::FileReadStream();
    thump_->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY "s12.wav");
    // loop_stream_ = new mu::LoopStream();
    // loop_stream_->set_source(thump_);
    // loop_stream_->set_interval(beat_to_tick(1.0));
    // loop_stream_->set_source_end(beat_to_tick(1.0));
  }
  mu::MuStream *stream() {
    // return loop_stream_;
    return thump_;
  }
protected:
  mu::FileReadStream *thump_;
  // mu::LoopStream *loop_stream_;
};                              // class Metronome
  
    
int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  Metronome *metronome = new Metronome();
  Basic *basic = new Basic();
  Swish *swish = new Swish();
  mu::SumStream *mix = new mu::SumStream();
  mu::LoopStream *loop = new mu::LoopStream();

  printf("1 beat = %ld ticks\n", beat_to_tick(1));

  basic->add(0.00, "thump.wav", 1.0, -24.0);
  basic->add(1.50, "thump.wav", 1.0, -24.0);
  basic->add(3.00, "thump.wav", 1.0, -24.0);
  basic->add(4.00, "thump.wav", 1.0, -24.0);
  basic->add(5.50, "thump.wav", 1.0, -24.0);
  basic->add(7.50, "thump.wav", 0.5, -24.0);

  swish->add(0.00, "s21.wav", 0.67, 0.60);
  swish->add(0.50, "s21.wav", 0.67, 0.60);
  swish->add(1.00, "s21.wav", 0.67, 0.60);
  swish->add(1.50, "s21.wav", 0.67, 0.60);
  swish->add(2.00, "s21.wav", 0.67, 0.60);
  swish->add(2.50, "s21.wav", 0.67, 0.60);
  swish->add(3.00, "s21.wav", 0.67, 0.60);
  swish->add(3.50, "s21.wav", 0.67, 0.60);
  swish->add(4.00, "s21.wav", 0.67, 0.60);
  swish->add(4.50, "s21.wav", 0.67, 0.60);
  swish->add(5.00, "s21.wav", 0.67, 0.60);
  swish->add(5.50, "s21.wav", 0.67, 0.60);
  swish->add(6.00, "s21.wav", 0.67, 0.60);
  swish->add(6.50, "s21.wav", 0.67, 0.60);
  swish->add(7.00, "s21.wav", 0.67, 0.60);
  swish->add(7.50, "s21.wav", 0.67, 0.60);

  swish->add(0.25, "s20.wav", 0.67, 0.60);
  swish->add(0.75, "s20.wav", 0.67, 0.60);
  swish->add(1.25, "s20.wav", 0.67, 0.60);
  swish->add(1.75, "s20.wav", 0.67, 0.60);
  swish->add(2.25, "s20.wav", 0.67, 0.60);
  swish->add(2.75, "s20.wav", 0.67, 0.60);
  swish->add(3.25, "s20.wav", 0.67, 0.60);
  swish->add(3.75, "s20.wav", 0.67, 0.60);
  swish->add(4.25, "s20.wav", 0.67, 0.60);
  swish->add(4.75, "s20.wav", 0.67, 0.60);
  swish->add(5.25, "s20.wav", 0.67, 0.60);
  swish->add(5.75, "s20.wav", 0.67, 0.60);
  swish->add(6.25, "s20.wav", 0.67, 0.60);
  swish->add(6.75, "s20.wav", 0.67, 0.60);
  swish->add(7.25, "s20.wav", 0.67, 0.60);
  swish->add(7.75, "s20.wav", 0.67, 0.60);

  swish->add(0.50, "s14.wav", 0.67, 0.50);
  swish->add(1.50, "s14.wav", 0.67, 0.50);
  swish->add(2.50, "s14.wav", 0.67, 0.50);
  swish->add(3.50, "s14.wav", 0.67, 0.50);
  swish->add(4.50, "s14.wav", 0.67, 0.50);
  swish->add(5.50, "s14.wav", 0.67, 0.50);
  swish->add(6.50, "s14.wav", 0.67, 0.50);
  swish->add(7.50, "s14.wav", 0.67, 0.50);

  // mix->add_source(metronome->stream());
  mix->add_source(basic->stream());
  mix->add_source(swish->stream());
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
