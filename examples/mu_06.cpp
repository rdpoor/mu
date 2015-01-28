// A funny echo that repeats N times while changing rate, pitch and gain.

#include "mu.h"

#include <string>
#include <iostream>
#include <cmath>

class EchoPlus {
public:
  EchoPlus() :
    source_(NULL),
    needs_setup_(true),
    duration_(44100),
    repeat_count_(1),
    warp_(0.0),
    pitch_start_(0),
    pitch_end_(0),
    db_gain_start_(0),
    db_gain_end_(0) {
    sum_stream_ = new mu::SumStream();
  }

  mu::MuStream *sink() { return sum_stream_; }

  mu::MuStream *source() { return source_; }
  void set_source(mu::MuStream *source) { 
    source_ = source; 
    needs_setup_ = true;
  }

  // Duration between first echo (inclusive) and last (exclusive)
  mu::MuTick duration() { return duration_; }
  void set_duration(mu::MuTick duration) { 
    duration_ = duration; 
    needs_setup_ = true;
  }

  // Number of echos 
  int repeat_count() { return repeat_count_; }
  void set_repeat_count(int repeat_count) { 
    repeat_count_ = repeat_count; 
    needs_setup_ = true;
  }

  // 0.0 = evenly spaced echos.  positive means the echos accelerate, negative
  // means they decelerate.
  double warp() { return warp_; }
  void set_warp(double warp) { 
    warp_ = warp; 
    needs_setup_ = true;
  }

  double pitch_start() { return pitch_start_; }
  void set_pitch_start(double pitch_start) { 
    pitch_start_ = pitch_start; 
    needs_setup_ = true;
  }

  double pitch_end() { return pitch_end_; }
  void set_pitch_end(double pitch_end) { 
    pitch_end_ = pitch_end; 
    needs_setup_ = true;
  }

  double db_gain_start() { return db_gain_start_; }
  void set_db_gain_start(double db_gain_start) { 
    db_gain_start_ = db_gain_start; 
    needs_setup_ = true;
  }

  double db_gain_end() { return db_gain_end_; }
  void set_db_gain_end(double db_gain_end) { 
    db_gain_end_ = db_gain_end; 
    needs_setup_ = true;
  }

  void teardown() {
    delete sum_stream_;
    sum_stream_ = new mu::SumStream();
  }

  void setup() {
    teardown();
    for (int i=repeat_count()-1; i>=0; --i) {
      double u = (double)i/repeat_count();
      create_one_echo(u);
    }
  }

private:
  mu::MuStream *source_;
  bool needs_setup_;
  mu::MuTick duration_;
  int repeat_count_;
  double warp_;
  double pitch_start_;
  double pitch_end_;
  double db_gain_start_;
  double db_gain_end_;
  mu::SumStream *sum_stream_;

  // u ranges from 0.0 (inclusive) to 1.0 (exclusive)
  void create_one_echo(double u) {
    if (source() == NULL) return;
    mu::MuStream *s = source()->clone();

    double pitch_shift = compute_pitch_shift(u);
    s = insert_pitch_shift(s, pitch_shift);

    double db_gain = compute_db_gain(u);
    s = insert_gain(s, db_gain);

    mu::MuTick delay = compute_delay(u);
    s = insert_delay(s, delay);

    sum_stream_->add_source(s);
  }

  mu::MuTick compute_delay(double u) {
    mu::MuFloat m = (1+warp_)/(1-warp_);
    mu::MuFloat f = pow(1.0 - pow(1-u, m), 1.0/m);
    return f * duration_;
  }

  double compute_pitch_shift(double u) {
    return pitch_start_ + (pitch_end_ - pitch_start_) * u;
  }

  double compute_db_gain(double u) {
    return db_gain_start_ + (db_gain_end_ - db_gain_start_) * u;
  }

  mu::MuStream *insert_delay(mu::MuStream *s, mu::MuTick delay) {
    if (delay == 0) return s;
    mu::DelayStream *delay_stream = new mu::DelayStream();
    delay_stream->set_delay(delay);
    delay_stream->set_source(s);
    return delay_stream;
  }

  mu::MuStream *insert_pitch_shift(mu::MuStream *s, double pitch_shift) {
    if (pitch_shift == 0.0) return s;
    mu::ConstantStream *constant_stream = new mu::ConstantStream();
    mu::IdentityStream *identity_stream = new mu::IdentityStream();
    mu::ProductStream *product_stream = new mu::ProductStream();
    mu::ResampleStream *resample_stream = new mu::ResampleStream();

    double relative_frequency = mu::MuUtils::pitch_to_ratio(pitch_shift);

    constant_stream->set_value(relative_frequency);
    product_stream->add_source(constant_stream);
    product_stream->add_source(identity_stream);
    resample_stream->set_timing_source(product_stream);
    resample_stream->set_sample_source(s);

    return resample_stream;
  }

  mu::MuStream *insert_gain(mu::MuStream *s, double db_gain) {
    if (db_gain == 0.0) return s;
    mu::ConstantStream *constant_stream = new mu::ConstantStream();
    mu::ProductStream *product_stream = new mu::ProductStream();
  
    double relative_gain = mu::MuUtils::db_to_ratio(db_gain);

    constant_stream->set_value(relative_gain);
    product_stream->add_source(constant_stream);
    product_stream->add_source(s);

    // std::cout << product_stream->inspect();
    return product_stream;
  }

};                              // class EchoPlus

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

static const mu::MuTick kTicsPerBeat = 22050;

mu::MuStream *create_echo_plus(mu::MuStream *source,
                               double start_beat,
                               double duration,
                               int repeat_count,
                               double warp,
                               double pitch_start,
                               double pitch_end,
                               double db_gain_start,
                               double db_gain_end) {
  EchoPlus *echo_plus = new EchoPlus();
  echo_plus->set_source(source);
  echo_plus->set_duration(duration * kTicsPerBeat);
  echo_plus->set_repeat_count(repeat_count);
  echo_plus->set_warp(warp);
  echo_plus->set_pitch_start(pitch_start);
  echo_plus->set_pitch_end(pitch_end);
  echo_plus->set_db_gain_start(db_gain_start);
  echo_plus->set_db_gain_end(db_gain_end);
  echo_plus->setup();

  mu::DelayStream *delay_stream = new mu::DelayStream();
  delay_stream->set_source(echo_plus->sink());
  delay_stream->set_delay(start_beat * kTicsPerBeat);

  // std::cout << delay_stream->inspect();

  return delay_stream;
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::FileReadStream *file_read_stream = new mu::FileReadStream();
  mu::SumStream *main_mix = new mu::SumStream();
  mu::LoopStream *loop_stream = new mu::LoopStream();
  
  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "thump.wav");
  // file_read_stream->set_file_name(EXAMPLE_DIRECTORY "purple.wav");

  loop_stream->set_source(main_mix);
  loop_stream->set_interval(kTicsPerBeat * 11);
  loop_stream->set_source_start(0);
  // BUG: source_end > interval leads to artifacts.  Sounds like non-initialized buffer?
  // loop_stream->set_source_end(kTicsPerBeat * 9);
  loop_stream->set_source_end(loop_stream->interval());

  transport.set_source(loop_stream);
  transport.set_player(&player_rt);

  main_mix->add_source(create_echo_plus(file_read_stream, 0, 5, 50, -0.3, 36, -12, -30, -3));
  main_mix->add_source(create_echo_plus(file_read_stream, 5, 5, 50, 0.3, -12, 36, -3, -25));

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
