// Demonstrate PsiStream.

#include "mu.h"
#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

const mu::MuTick kTicsPerBeat = 44100/4;

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  return beat * kTicsPerBeat;
}

int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  mu::SequenceStream *sequence_stream = new mu::SequenceStream();
  
  // ================================================================
  // play original note "the hard way" using identity function for tau(t) and
  // constant 1.0 for omega(t)
  mu::PsiStream *psi_stream_01 = new mu::PsiStream();
  mu::IdentityStream *identity_01 = new mu::IdentityStream();
  mu::ConstantStream *constant_01 = new mu::ConstantStream();
  constant_01->set_value(1.0);
  psi_stream_01->set_tau_source(identity_01);
  psi_stream_01->set_omega_source(constant_01);
  psi_stream_01->set_psi_filename(EXAMPLE_DIRECTORY "69.psi");
  sequence_stream->add_source(psi_stream_01, beat_to_tick(0), 1.0);

  // ================================================================
  // play the attack at the original rate, stretch the rest of the note until
  // the release, then play the release at original speed.  Make the entire
  // note last four beats.
  mu::PsiStream *psi_stream_02 = new mu::PsiStream();
  psi_stream_02->set_psi_filename(EXAMPLE_DIRECTORY "69.psi");
  mu::MuTick frame_count = psi_stream_02->frame_count();

  // "u" is input (original) time
  double u0 = 0;
  double u1 = 3000;             // end of initial attack
  double u2 = 9000;             // onset of end of note
  double u3 = frame_count;

  // computing stretch factor:
  // 
  // (u1 - u0) * 1.0 + (u2 - u1) * stretch + (u3 - u2) * 1.0 = beat_to_tick(8)
  //
  // solve for stretch:
  // (u1 - u0) * 1.0 + (u2 - u1) * stretch + (u3 - u2) * 1.0 = beat_to_tick(8)
  // u1 + (u2 - u1) * stretch + u3 - u2 = beat_to_tick(8)
  // (u2 - u1) * stretch = beat_to_tick(8) - u1 - u3 + u2
  // stretch = (beat_to_tick(8) - u1 - u3 + u2)  / (u2 - u1)
  double stretch = (beat_to_tick(8) - u1 - u3 + u2)  / (u2 - u1);

  // "t" is output (warped) time
  //
  double t0 = u0;
  double t1 = u1;
  double t2 = (u1 - u0) * 1.0 + (u2 - u1) * stretch;
  double t3 = (u1 - u0) * 1.0 + (u2 - u1) * stretch + (u3 - u2) * 1.0;

  printf("stretch = %lf\n", stretch);
  printf("u0 = %lf, t0 = %lf\n", u0, t0);
  printf("u1 = %lf, t1 = %lf\n", u1, t1);
  printf("u2 = %lf, t2 = %lf\n", u2, t2);
  printf("u3 = %lf, t3 = %lf\n", u3, t3);
  printf("t3 = %lf, beat_to_tick(4) = %ld\n", t3, beat_to_tick(4));

  //
  mu::PiecewiseLinearStream *piecewise_linear_stream_02 = new mu::PiecewiseLinearStream();
  piecewise_linear_stream_02->add_breakpoint(t0, u0);
  piecewise_linear_stream_02->add_breakpoint(t1, u1);
  piecewise_linear_stream_02->add_breakpoint(t2, u2);
  piecewise_linear_stream_02->add_breakpoint(t3, u3);
  mu::ConstantStream *constant_02 = new mu::ConstantStream();
  constant_02->set_value(1.0);
  psi_stream_02->set_tau_source(piecewise_linear_stream_02);
  psi_stream_02->set_omega_source(constant_02);
  sequence_stream->add_source(psi_stream_02, beat_to_tick(4), 1.0);

  // ================================================================
  // play 3 semitones down
  mu::PsiStream *psi_stream_03 = new mu::PsiStream();
  mu::IdentityStream *identity_03 = new mu::IdentityStream();
  mu::ConstantStream *constant_03 = new mu::ConstantStream();
  constant_03->set_value(mu::MuUtils::pitch_to_ratio(-3));
  psi_stream_03->set_tau_source(identity_03);
  psi_stream_03->set_omega_source(constant_03);
  psi_stream_03->set_psi_filename(EXAMPLE_DIRECTORY "69.psi");
  sequence_stream->add_source(psi_stream_03, beat_to_tick(12), 1.0);
  
  // ================================================================
  // play 1 semitone down with 6Hz vibrato
  mu::PsiStream *psi_stream_04 = new mu::PsiStream();
  mu::IdentityStream *identity_04 = new mu::IdentityStream();
  mu::ConstantStream *constant_04 = new mu::ConstantStream();
  constant_04->set_value(mu::MuUtils::pitch_to_ratio(-1));
  mu::SineStream *sine_04 = new mu::SineStream();
  sine_04->set_f0(6.0);
  sine_04->set_a0(0.01);
  mu::SumStream *sum_stream_04 = new mu::SumStream();
  sum_stream_04->add_source(constant_04);
  sum_stream_04->add_source(sine_04);
  psi_stream_04->set_tau_source(identity_04);
  psi_stream_04->set_omega_source(sum_stream_04);
  psi_stream_04->set_psi_filename(EXAMPLE_DIRECTORY "69.psi");
  sequence_stream->add_source(psi_stream_04, beat_to_tick(16), 1.0);

  // ================================================================
  // put it all together and play in a loop.
  mu::LoopStream *loop_stream = new mu::LoopStream();
  loop_stream->set_interval(beat_to_tick(20));
  loop_stream->set_source_end(beat_to_tick(20));
  loop_stream->set_source(sequence_stream);

  transport.set_player(&player_rt);
  transport.set_source(loop_stream);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
