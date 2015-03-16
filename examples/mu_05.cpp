// Play a thump repeatedly while shifting pitch

#include "mu.h"
#include <string>
#include <iostream>
#include <cmath>

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/sounds/"

// ================================================================
// An example of composing a number of processing elements into a single object.

class PitchShifter {
public:
  PitchShifter( void );
  ~PitchShifter( void ) {}

  mu::MuFloat shift() { 
    return constant_stream_->value(); 
  }
  void set_shift(mu::MuFloat shift) { 
    constant_stream_->set_value(shift); 
  }

  mu::MuStream *source() { 
    return resample_stream_->sample_source(); 
  }
  void set_source(mu::MuStream *source) {
    resample_stream_->set_sample_source(source);
  }

  // get the resulting stream
  mu::MuStream *stream() {
    return resample_stream_;
  }
protected:
  mu::ConstantStream *constant_stream_;
  mu::IdentityStream *identity_stream_;
  mu::ProductStream *product_stream_;
  mu::ResampleStream *resample_stream_;
};

PitchShifter::PitchShifter() {
  constant_stream_ = new mu::ConstantStream();
  identity_stream_ = new mu::IdentityStream();
  product_stream_ = new mu::ProductStream();
  resample_stream_ = new mu::ResampleStream();

  product_stream_->add_source(constant_stream_);
  product_stream_->add_source(identity_stream_);
  resample_stream_->set_timing_source(product_stream_);
}

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::FileReadStream *file_read_stream = new mu::FileReadStream();
  mu::CropStream *crop_stream = new mu::CropStream();
  mu::LoopStream *loop_stream = new mu::LoopStream();
  mu::SequenceStream *sequence_stream = new mu::SequenceStream();

  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "thump.wav");
  mu::MuTick n_frames = file_read_stream->duration();
  crop_stream->set_source(file_read_stream);
  crop_stream->set_source_start(0);
  crop_stream->set_source_end(n_frames);

  mu::MuTick ticks_per_beat = 44100 / 16;

  std::cout << "================" << std::endl;
  std::cout << crop_stream->inspect() << std::endl << std::endl;
  std::cout << "================" << std::endl;

  for (int i=0; i<12; i++) {
    double relative_frequency = pow(2.0, (i - 12.0)/12.0);
    PitchShifter *pitch_shifter = new PitchShifter();
    pitch_shifter->set_shift(relative_frequency);
    mu::CropStream *c2 = crop_stream->clone();
    printf("=== crop_stream_copy = %p, crop_stream_copy->source() = %p\n", c2, c2->source());
    pitch_shifter->set_source(c2);
    sequence_stream->add_source(pitch_shifter->stream(),
                                i * ticks_per_beat,
                                1.0);
  }
    

  loop_stream->set_source_start(0);
  loop_stream->set_source_end(12 * ticks_per_beat);
  loop_stream->set_interval(12 * ticks_per_beat);
  loop_stream->set_source(sequence_stream);

  std::cout << loop_stream->inspect() << std::endl;

  transport.set_source(loop_stream);
  transport.set_player(&player_rt);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
