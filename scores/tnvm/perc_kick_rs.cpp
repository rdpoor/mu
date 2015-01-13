#include "perc_kick_rs.h"

#include "add_rs.h"
#include "delay_rs.h"
#include "file_read_rs.h"
#include "loop_rs.h"
#include "random_select_rs.h"

#define PERCUSSION_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"
#define PLUCKED_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

/*
 * Return a stream that whenever start_tick == 0 returns a randomly
 * chosen kick sound.  TODO: Generalize
 *
 * &(new mu::DelayRS())->set_delay(BEAT_TO_FRAME(beat_));
 */
mu::RenderStream *make_kicks_rs() {
  // mu::RandomSelectRS *random_select_rs = new mu::RandomSelectRS();
  // random_select_rs->add_source(&(new mu::FileReadRS())->set_file_name(PERCUSSION_SOUND_DIR "s02.wav"));
  // random_select_rs->add_source(&(new mu::FileReadRS())->set_file_name(PERCUSSION_SOUND_DIR "s15.wav"));
  // random_select_rs->add_source(&(new mu::FileReadRS())->set_file_name(PERCUSSION_SOUND_DIR "s15.wav"));
  // return random_select_rs;
  mu::FileReadRS *frs = new mu::FileReadRS();
  frs->set_file_name(PERCUSSION_SOUND_DIR "s15.wav");
  return frs;
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

// ================================================================
//

mu::RenderStream *make_perc_kick_rs() {
  // mu::RenderStream *kicks_rs = make_kicks_rs();
  Monophonic *kick = new Monophonic();
  // kick->play(0, kicks_rs);
  // kick->play(0.5, kicks_rs);
  // kick->play(1.25, kicks_rs);
  // kick->play(3.0, kicks_rs);

  mu::FileReadRS *frs1 = new mu::FileReadRS();
  frs1->set_file_name(PLUCKED_SOUND_DIR "D4.wav");

  mu::FileReadRS *frs2 = new mu::FileReadRS();
  frs2->set_file_name(PLUCKED_SOUND_DIR "E4.wav");

  mu::FileReadRS *frs3 = new mu::FileReadRS();
  frs3->set_file_name(PLUCKED_SOUND_DIR "F4.wav");

  mu::FileReadRS *frs4 = new mu::FileReadRS();
  frs4->set_file_name(PLUCKED_SOUND_DIR "G4.wav");

  kick->play(0, frs1);
  kick->play(0.25, frs2);
  kick->play(0.5, frs3);
  kick->play(3.0, frs4);

  mu::LoopRS *loop_rs = new mu::LoopRS();
  loop_rs->set_source(kick->render_stream());
  loop_rs->set_duration(BEAT_TO_FRAME(4));

  return loop_rs;
}



  
