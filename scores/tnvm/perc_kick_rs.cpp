#include "perc_kick_rs.h"

#include "add_rs.h"
#include "constant_rs.h"
#include "delay_rs.h"
#include "file_read_rs.h"
#include "identity_rs.h"
// #include "linseg_rs.h"
#include "loop_rs.h"
#include "multiply_rs.h"
#include "resample_rs.h"
#include "stk_effect_rs.h"
#include <BiQuad.h>
#include <TwoPole.h>
#include <limits.h>             // long max

/*
 * Thoughts:
 * 
 */

#define PERCUSSION_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"
#define PLUCKED_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

/*
s07 - solid
s11 - click'y
s15 - solid, muffled (clipped?)
*/

mu::RenderStream *make_single_kick_rs() {
  // mu::LinsegRS *timing = new mu::LinsegRS();
  mu::ConstantRS *constant_rs = new mu::ConstantRS();
  mu::FileReadRS *file_read_rs = new mu::FileReadRS();
  mu::IdentityRS *identity_rs = new mu::IdentityRS();
  mu::MultiplyRS *multiply_rs = new mu::MultiplyRS();
  mu::ResampleRS *resample_rs = new mu::ResampleRS();
  
  file_read_rs->set_file_name(PERCUSSION_SOUND_DIR "s07.wav");
  // return file_read_rs;

  constant_rs->set_value(0.5);
  multiply_rs->add_source(constant_rs);
  multiply_rs->add_source(identity_rs);
  resample_rs->set_timing_source(multiply_rs);
  // resample_rs->set_timing_source(identity_rs);
  resample_rs->set_sample_source(file_read_rs);
  return resample_rs;
}

class Monophonic {
public:
  Monophonic() {}
  
  void play(double beat, mu::RenderStream *rs) {
    mu::DelayRS *drs = new mu::DelayRS();
    printf("set delay %f\n", beat);
    drs->set_delay(BEAT_TO_FRAME(beat));
    drs->set_source(rs);
    add_rs_.add_source(drs);
  }
  
  mu::RenderStream *render_stream( void ) { 
    printf("add_rs.source_count() = %ld\n", add_rs_.source_count());
    return &add_rs_; 
  }
  
protected:
  mu::AddRS add_rs_;
  
};

class ResonRS : public mu::StkEffectRS {
public:
  ResonRS() {
    two_pole_.setResonance(120.0, 0.99, true);
  }
  ~ResonRS() {
  }

  void tick(mu::MuFloat value) {
    two_pole_.tick(value);
  }

  mu::MuFloat last_out(int channel) {
    (void)channel;
    return two_pole_.lastOut();
  }

protected:
  stk::TwoPole two_pole_;

};

class Reson2RS : public mu::StkEffectRS {
public:
  Reson2RS() {
    bi_quad_.setResonance(70.0, 0.9997, true);
  }
  ~Reson2RS() {
  }

  void tick(mu::MuFloat value) {
    bi_quad_.tick(value);
  }

  mu::MuFloat last_out(int channel) {
    (void)channel;
    // * 64.0 is quick hack to normalize gain
    return bi_quad_.lastOut() * 64.0;
  }

protected:
  stk::BiQuad bi_quad_;

};

// ================================================================
//

mu::RenderStream *make_perc_kick_rs() {
  mu::RenderStream *kicks_rs = make_single_kick_rs();
  Monophonic *kick = new Monophonic();

  kick->play(0, kicks_rs);
  kick->play(1.0, kicks_rs);
  kick->play(2.0, kicks_rs);
  kick->play(3.0, kicks_rs);
  kick->play(3.5, kicks_rs);

  // return kick->render_stream();
  mu::LoopRS *loop_rs = new mu::LoopRS();
  loop_rs->set_source(kick->render_stream());
  loop_rs->set_duration(BEAT_TO_FRAME(4));

  return loop_rs;

  // Reson2RS *reson_rs = new Reson2RS();
  // reson_rs->set_source(loop_rs);
  // reson_rs->set_blend(1.0);
  
  // return reson_rs;
}



  
