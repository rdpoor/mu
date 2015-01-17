/*
 * Play a sound file through _stk::LentPitShift_ stk::PitShift
 */

// #include "LentPitShift.h"
#include "PitShift.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include "sequence_sp.h"
#include <unistd.h>

class PitchShiftSP : public mu::SampleProcessor {
private:
  void setup() {
//     if (lent_pit_shift_ == NULL) { 
//       lent_pit_shift_ = new stk::LentPitShift(ratio_, stk::RT_BUFFER_SIZE); 
//     }
    if (pit_shift_ == NULL) { 
      pit_shift_ = new stk::PitShift();
      pit_shift_->setShift(ratio_);
      pit_shift_->setEffectMix(1.0);
    }
    return;
  }

public:

  PitchShiftSP( void )
    : ratio_(1.0), pit_shift_(NULL) {
  }

  ~PitchShiftSP( void ) {
    delete pit_shift_;
    pit_shift_ = NULL;
  }

  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) {
    if (source_ == NULL) {
      zeroBuffer (buffer);
    } else {
      setup();
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      buffer_.resize(frame_count, channel_count);

      source_->step(buffer_, tick, true);
      for (int frame=0; frame<frame_count; frame++) {
        stk::StkFloat x = buffer_(frame, 0);
        stk::StkFloat y = pit_shift_->tick(x);
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = y;
        }
      }
      // fprintf(stderr,"F");
    }
  }

  mu::SampleProcessor *getSource() { return source_; }
  PitchShiftSP& setSource(mu::SampleProcessor *source) { source_ = source; return *this; }

  stk::StkFloat getRatio( void ) { return ratio_; }
  PitchShiftSP& setRatio(stk::StkFloat ratio) { ratio_ = ratio; return *this; }
  
protected:
  mu::SampleProcessor *source_;
  stk::StkFrames buffer_;
  stk::StkFloat ratio_;
  stk::PitShift *pit_shift_;
}; // class PitchShiftSP


#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (44100*2)

mu::SampleProcessor *getSoundFile(std::string file_name) {
  mu::FileReadSP *frs = &((new mu::FileReadSP())->fileName(file_name).doNormalize(true));
  return frs;
}

mu::SequenceSP *makeSequence( double tempo ) {
  mu::SequenceSP *ss = new mu::SequenceSP();
  ss->addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 0.0*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "C5" ".wav"), 1.0*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "B4" ".wav"), 1.5*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "G4" ".wav"), 2.5*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 3.5*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 4.5*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "D4" ".wav"), 5.5*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "G4" ".wav"), 6.0*tempo);
  ss->addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 6.5*tempo);
  return ss;
}

int main() {
  mu::SequenceSP *sequence_sp_a = makeSequence(44100/2);
  mu::SequenceSP *sequence_sp_b = makeSequence(44110/2);
  mu::SequenceSP *sequence_sp_c = makeSequence(44120/2);
  PitchShiftSP pitch_shift_sp_a;
  PitchShiftSP pitch_shift_sp_b;
  PitchShiftSP pitch_shift_sp_c;
  mu::AddSP add_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  pitch_shift_sp_b.setSource(sequence_sp_b).setRatio(0.25);
  pitch_shift_sp_a.setSource(sequence_sp_a).setRatio(0.5);
  pitch_shift_sp_c.setSource(sequence_sp_c).setRatio(0.66667);

  add_sp.addSource(&pitch_shift_sp_a);
  add_sp.addSource(&pitch_shift_sp_b);
  add_sp.addSource(&pitch_shift_sp_c);

  loop_sp.setSource(&add_sp).setLoopDuration(add_sp.getEnd());
  player.setSource(&loop_sp);
  // player.setSource(&add_sp);

  player.start();
  sleep(20);
  player.stop();

  return 0;
}
