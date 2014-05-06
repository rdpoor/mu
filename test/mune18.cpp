/*
 * Play a sound file through _stk::LentPitShift_ stk::PitShift
 */

// #include "LentPitShift.h"
#include "PitShift.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include "sequence_stream.h"
#include <unistd.h>

class PitchShiftStream : public mu::Stream {
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

  PitchShiftStream( void )
    : ratio_(1.0), pit_shift_(NULL) {
  }

  ~PitchShiftStream( void ) {
    delete pit_shift_;
    pit_shift_ = NULL;
  }

  PitchShiftStream& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) {
    if (source_ == NULL) {
      zero_buffer (buffer);
    } else {
      setup();
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      buffer_.resize(frame_count, channel_count);

      source_->step(buffer_, tick, player);
      for (int frame=0; frame<frame_count; frame++) {
        stk::StkFloat x = buffer_(frame, 0);
        stk::StkFloat y = pit_shift_->tick(x);
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = y;
        }
      }
      // fprintf(stderr,"F");
    }
    return *this;
  }

  mu::Stream *getSource() { return source_; }
  PitchShiftStream& setSource(mu::Stream *source) { source_ = source; return *this; }

  stk::StkFloat getRatio( void ) { return ratio_; }
  PitchShiftStream& setRatio(stk::StkFloat ratio) { ratio_ = ratio; return *this; }
  
protected:
  mu::Stream *source_;
  stk::StkFrames buffer_;
  stk::StkFloat ratio_;
  stk::PitShift *pit_shift_;
}; // class PitchShiftStream


#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (44100*2)

mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  mu::CropStream *cs = &(new mu::CropStream())->setSource(frs).setStart(0);
  return cs;
}

mu::SequenceStream *makeSequence( double tempo ) {
  mu::SequenceStream *ss = new mu::SequenceStream();
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
  mu::SequenceStream *sequence_stream_a = makeSequence(44100/2);
  mu::SequenceStream *sequence_stream_b = makeSequence(44110/2);
  mu::SequenceStream *sequence_stream_c = makeSequence(44120/2);
  PitchShiftStream pitch_shift_stream_a;
  PitchShiftStream pitch_shift_stream_b;
  PitchShiftStream pitch_shift_stream_c;
  mu::AddStream add_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  pitch_shift_stream_b.setSource(sequence_stream_b).setRatio(0.25);
  pitch_shift_stream_a.setSource(sequence_stream_a).setRatio(0.5);
  pitch_shift_stream_c.setSource(sequence_stream_c).setRatio(0.66667);

  add_stream.addSource(&pitch_shift_stream_a);
  add_stream.addSource(&pitch_shift_stream_b);
  add_stream.addSource(&pitch_shift_stream_c);

  loop_stream.setSource(&add_stream).setLoopDuration(add_stream.getEnd());
  player.setSource(&loop_stream);
  // player.setSource(&add_stream);

  player.start();
  sleep(20);
  player.stop();

  return 0;
}
