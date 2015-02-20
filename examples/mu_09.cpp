// Demonstrate file_write_stream.

#include "mu.h"
#include <string>
#include <iostream>
#include <cmath>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;

  mu::FileReadStream *file_read_stream = new mu::FileReadStream();
  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "purple.wav");

  mu::IdentityStream *identity_stream = new mu::IdentityStream();

  mu::GainStream *gain_stream = new mu::GainStream();
  gain_stream->set_signal_source(identity_stream);
  gain_stream->set_gain(mu::MuUtils::pitch_to_ratio(-6.0));

  mu::ResampleStream *resample_stream = new mu::ResampleStream();
  resample_stream->set_sample_source(file_read_stream);
  resample_stream->set_timing_source(gain_stream);

  mu::FileWriteStream *file_write_stream = new mu::FileWriteStream();
  file_write_stream->set_source(resample_stream);
  file_write_stream->set_file_name(EXAMPLE_DIRECTORY "mu_09.wav");

  transport.set_source(file_write_stream);
  transport.set_player(&player_rt);
  
  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
