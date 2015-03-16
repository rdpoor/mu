// Demonstrate FuzzStream

#include "mu.h"
#include <string>
#include <iostream>

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/sounds/"

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

mu::MuFloat beats_per_minute() {
  return 45.0;
}

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  mu::MuFloat tics_per_beat = 44100 * 60.0 / beats_per_minute();
  return beat * tics_per_beat;
}

// ================================================================
class Stretcher {
public:
  Stretcher() {
    constant_stream_ = new mu::ConstantStream();
    piecewise_linear_stream_ = new mu::PiecewiseLinearStream();
    psi_stream_ = new mu::PsiStream();

    psi_stream_->set_omega_source(constant_stream_);
    psi_stream_->set_psi_filename(EXAMPLE_DIRECTORY "69.psi");
    set_pitch_shift(0.0);
    set_duration(psi_stream_->frame_count()/(mu::MuFloat)beat_to_tick(1.0));
  }
  ~Stretcher() {
  }

  mu::MuFloat duration() {
    return duration_;            // too lazy to back-compute it
  }

  // BUG: PsiStream should be silent after duration.  Instead, it plays its last
  // cycle repeatedly.
  void set_duration(mu::MuFloat duration) {
    duration_ = duration;

    // because there's no way to delete breakpoints once created...
    if (piecewise_linear_stream_) {
      delete piecewise_linear_stream_;
      piecewise_linear_stream_ = new mu::PiecewiseLinearStream();
      psi_stream_->set_tau_source(piecewise_linear_stream_);
    }

    // ux is original time (in frames)
    double u0 = 0;
    double u1 = 3000;           // end of attack
    double u2 = 9000;           // onset of end of note
    double u3 = psi_stream_->frame_count();
    double stretch = (beat_to_tick(duration) - u1 - u3 + u2)  / (u2 - u1);

    // tx is warped time (in frames)
    double t0 = u0;
    double t1 = u1;
    double t2 = (u1 - u0) * 1.0 + (u2 - u1) * stretch;
    double t3 = (u1 - u0) * 1.0 + (u2 - u1) * stretch + (u3 - u2) * 1.0;

    printf("stretch = %lf\n", stretch);
    printf("u0 = %lf, t0 = %lf\n", u0, t0);
    printf("u1 = %lf, t1 = %lf\n", u1, t1);
    printf("u2 = %lf, t2 = %lf\n", u2, t2);
    printf("u3 = %lf, t3 = %lf\n", u3, t3);
    printf("t3 = %lf, duration = %ld\n", t3, beat_to_tick(duration));

    piecewise_linear_stream_->add_breakpoint(t0, u0);
    piecewise_linear_stream_->add_breakpoint(t1, u1);
    piecewise_linear_stream_->add_breakpoint(t2, u2);
    piecewise_linear_stream_->add_breakpoint(t3, u3);
  }

  mu::MuFloat pitch_shift() {
    return mu::MuUtils::ratio_to_pitch(constant_stream_->value());
  }
  void set_pitch_shift(mu::MuFloat shift) {
    constant_stream_->set_value(mu::MuUtils::pitch_to_ratio(shift));
  }

  mu::MuStream *stream() {
    return psi_stream_;
  }
protected:
  mu::MuFloat duration_;
  mu::ConstantStream *constant_stream_;
  mu::PiecewiseLinearStream *piecewise_linear_stream_;
  mu::PsiStream *psi_stream_;
};

// ================================================================
int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  mu::SequenceStream *sequence_stream = new mu::SequenceStream();

  // mu::MuFloat ms[] = {0.275, 0.5, 1.0, 2.0, 3.6};
  
#if 0
  // Play 10 notes with increasing fuzziness
  for (int i = 0; i < 10; i++ ) {
    mu::MuFloat fuzz = (float)i/10;

    mu::FuzzStream *fuzz_stream = new mu::FuzzStream();
    fuzz_stream->set_mix(1.0);
    fuzz_stream->set_fuzz_factor(fuzz);
#if 0
    mu::FileReadStream *file_read_stream = new mu::FileReadStream();
    file_read_stream->set_file_name(EXAMPLE_DIRECTORY "69.wav");
    fuzz_stream->set_source(file_read_stream);
#else
    Stretcher *stretcher = new Stretcher();
    stretcher->set_duration(4.0);
    stretcher->set_pitch_shift(-12.0);
    fuzz_stream->set_source(stretcher->stream());
#endif
    sequence_stream->add_source(fuzz_stream, beat_to_tick(i*4), 1.0);
  }
#else
    Stretcher *stretcher1 = new Stretcher();
    stretcher1->set_duration(8.0);
    stretcher1->set_pitch_shift(0.0);
    mu::FuzzStream *fuzz_stream1 = new mu::FuzzStream();
    fuzz_stream1->set_mix(1.0);
    fuzz_stream1->set_fuzz_factor(0.8);
    fuzz_stream1->set_source(stretcher1->stream());

    sequence_stream->add_source(fuzz_stream1, beat_to_tick(0.0), 0.5);
    
    Stretcher *stretcher2 = new Stretcher();
    stretcher2->set_duration(8.1);
    stretcher2->set_pitch_shift(0.01);
    mu::FuzzStream *fuzz_stream2 = new mu::FuzzStream();
    fuzz_stream2->set_mix(1.0);
    fuzz_stream2->set_fuzz_factor(0.8);
    fuzz_stream2->set_source(stretcher2->stream());

    sequence_stream->add_source(fuzz_stream2, beat_to_tick(0.0), 0.5);
#endif

  // ================================================================
  // put it all together
  transport.set_player(&player_rt);
  transport.set_source(sequence_stream);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
