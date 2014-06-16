/*
 * Together Now, Very Minor
 */
#include "mu.h"

#include "NRev.h"
#include "add_stream.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "linseg_stream.h"
#include "loop_stream.h"
#include "multiply_stream.h"
#include "null_stream.h"
#include "reverb_stream.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define PERCUSSION_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

// TIMING
#define FRAMES_PER_SECOND 44100
#define SECONDS_PER_MINUTE 60.0

#define BEATS_PER_MINUTE 130.0
#define BEATS_PER_MEASURE 4

#define BEAT_TO_SECOND(__b__) (__b__ * SECONDS_PER_MINUTE / BEATS_PER_MINUTE)
#define BEAT_TO_FRAME(__b__) (BEAT_TO_SECOND(__b__) * FRAMES_PER_SECOND)
#define MEASURE_TO_BEAT(__m__) (__m__ * BEATS_PER_MEASURE)
#define MEASURE_TO_FRAME(__m__) (BEAT_TO_FRAME(MEASURE_TO_BEAT(__m__)))

#define M_INTRO 0
#define M_VERS1 (M_INTRO + 8)
#define M_VERS2 (M_VERS1 + 8)
#define M_CHOR1 (M_VERS2 + 8)
#define M_VERS3 (M_CHOR1 + 12)
#define M_CHOR2 (M_VERS3 + 8)
#define M_BRIDG (M_CHOR2 + 12)
#define M_OUTRO (M_BRIDG + 12)
#define M_END (M_OUTRO + 12)

// ================================================================
/*
 * StreamSet maps between a pitch number (i.e. midi pitch) and
 * a stream that produces that pitch.  A typical subclass maps
 * a pitch to a FileReadStream().
 */
class StreamSet {
public:
  StreamSet( void ) {
  }
  ~StreamSet( void ) {
  }
  virtual mu::Stream *findStream(std::string name) = 0;

protected:
  mu::NullStream null_stream_;
};                              // class StreamSet

// ================================================================
class FileReadStreamSet : public StreamSet {
public:
  mu::Stream *findStream(std::string name) {
    if (cache_.find(name) == cache_.end()) {
      std::string file_name = makeFileName(name);
      mu::FileReadStream *frs = new mu::FileReadStream();
      frs->fileName(file_name).doNormalize(true);
      cache_[name] = frs;
    }
    return cache_[name];
  }

  std::string getDirectoryName( void ) { return directory_name_; }
  FileReadStreamSet& setDirectoryName(std::string directory_name) { 
    directory_name_ = directory_name; 
    return *this;
  }

protected:
  std::string makeFileName(std::string name) {
    std::stringstream ss;
    ss << directory_name_ << name << ".wav";
    return ss.str();
  }
  std::string directory_name_;
  std::map<std::string, mu::FileReadStream *> cache_;
};                              // class FileReadStreamSet

// ================================================================

/*
 * Generate notes, chords and rests
 */
class Hand {
public:
  // generate a rest by incrementing time by duration.
  Hand& rest(double duration) {
    time_ += duration;
    return *this;
  }

  // generate a note and increment time by duration.
  Hand& note(double duration, std::string pitch, double legato=1.5) {
    make_note(time_, duration, pitch, legato);
    time_ += duration;
    return *this;
  }

  // play up to 5 notes as a chord.  arpeggio rolls the chord as a
  // factor of the duration: 0 = no roll, 1.0 = roll over duration.
  Hand& chord(double duration, 
              std::string n0 = "",
              std::string n1 = "",
              std::string n2 = "",
              std::string n3 = "",
              std::string n4 = "",
              double legato = 1.0,
              double arpeggio = 0.0) {
    make_note(time_ + 0.0 * arpeggio * duration, duration, n0, legato);
    make_note(time_ + 1.0 * arpeggio * duration, duration, n1, legato);
    make_note(time_ + 2.0 * arpeggio * duration, duration, n2, legato);
    make_note(time_ + 3.0 * arpeggio * duration, duration, n3, legato);
    make_note(time_ + 4.0 * arpeggio * duration, duration, n4, legato);
    time_ += duration;
    return *this;
  }

  double getTime( void ) { return time_; }
  Hand& setTime(double time) { time_ = time; return *this; }

  StreamSet *getStreamSet( void ) { return stream_set_; }
  Hand& setStreamSet(StreamSet *stream_set) { stream_set_ = stream_set; return *this; }

  mu::Stream *getStream( void ) { return &add_stream_; }

protected:
  // add a note to the stream.  Does not alter time_.
  Hand& make_note(double time, double duration, std::string pitch, double legato) {
    if (pitch.empty()) return *this;

    mu::Stream *s = stream_set_->findStream(pitch);
    mu::CropStream *cs = &(new mu::CropStream())->
      setStart(0).
      setEnd((mu::Tick)(duration * legato)).
      setSource(s);
    mu::DelayStream *ds = &(new mu::DelayStream())->
      setDelay((mu::Tick)time).
      setSource(cs);
    mu::FadeStream *fs = &(new mu::FadeStream())->
      setFadeTime(100).
      setSource(ds);
    add_stream_.addSource(fs);
    return *this;
  }

  double time_;
  mu::AddStream add_stream_;
  StreamSet *stream_set_;
};                              // class Hand

//================================================================

mu::Stream *make_metronome_stream() {
  mu::FileReadStream *thump_stream = new mu::FileReadStream();
  thump_stream->fileName(PERCUSSION_SOUND_DIR "s15.wav");
  mu::CropStream *thump_crop = new mu::CropStream();
  thump_crop->setSource(thump_stream);
  thump_crop->setStart(0);
  thump_crop->setEnd(thump_stream->getEnd());

  mu::FileReadStream *tic_stream = new mu::FileReadStream();
  tic_stream->fileName(PERCUSSION_SOUND_DIR "s11.wav");
  mu::CropStream *tic_crop = new mu::CropStream();
  tic_crop->setSource(tic_stream);
  tic_crop->setStart(0);
  tic_crop->setEnd(tic_stream->getEnd());

  mu::ConstantStream *tic_gain = new mu::ConstantStream();
  tic_gain->setValue(0.025);

  mu::MultiplyStream *tic_attenuator = new mu::MultiplyStream();
  tic_attenuator->addSource(tic_gain);
  tic_attenuator->addSource(tic_crop);

  mu::LoopStream *tic_loop = new mu::LoopStream();
  tic_loop->setSource(tic_attenuator);
  tic_loop->setLoopDuration(BEAT_TO_FRAME(1));

  mu::CropStream *tic_loop_cropped = new mu::CropStream();
  tic_loop_cropped->setStart(0);
  tic_loop_cropped->setEnd(MEASURE_TO_FRAME(1));
  tic_loop_cropped->setSource(tic_loop);

  mu::AddStream *mix_stream = new mu::AddStream();
  mix_stream->addSource(thump_crop);
  mix_stream->addSource(tic_loop_cropped);

  mu::LoopStream *loop_stream = new mu::LoopStream();
  loop_stream->setLoopDuration(MEASURE_TO_FRAME(1));
  loop_stream->setSource(mix_stream);

  return loop_stream;
}

// ================================================================

mu::Stream *make_intro_stream() {
  mu::AddStream *sub_mix = new mu::AddStream();
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);
  sub_mix->addSource(bass->getStream());
  printf("bass.getStream() = %p (%s)\n", bass->getStream(), bass->getStream()->getClassName().c_str());

  bass->setTime(MEASURE_TO_FRAME(4));
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs3", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs3", normal_legato);

  sub_mix->addSource(make_metronome_stream());
  return sub_mix;
}

mu::Stream *make_vers1_stream() {
  mu::AddStream *sub_mix = new mu::AddStream();
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);
  sub_mix->addSource(bass->getStream());

  bass->setTime(MEASURE_TO_FRAME(0));
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "fs1", normal_legato);
  bass->note(dur_q + dur_e, "d1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);

  sub_mix->addSource(make_metronome_stream());

  return sub_mix;
}

mu::Stream *make_vers2_stream() {
  return make_vers1_stream();
}

mu::Stream *make_chor1_stream() {
  mu::AddStream *sub_mix = new mu::AddStream();
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);
  sub_mix->addSource(bass->getStream());

  bass->setTime(MEASURE_TO_FRAME(0));
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  sub_mix->addSource(make_metronome_stream());
  return sub_mix;
}

mu::Stream *make_vers3_stream() {
  return make_vers1_stream();
}

mu::Stream *make_chor2_stream() {
  return make_chor1_stream();
}

mu::Stream *make_bridg_stream() {
  mu::AddStream *sub_mix = new mu::AddStream();
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);
  sub_mix->addSource(bass->getStream());

  bass->setTime(MEASURE_TO_FRAME(0));
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "gs2", normal_legato);
  bass->note(dur_h + dur_e, "gs2", normal_legato);
  bass->note(dur_q + dur_e, "gs2", normal_legato);
  bass->note(dur_h + dur_e, "gs2", normal_legato);

  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "fs2", normal_legato);

  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "a2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "e2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(4 * dur_q, "cs2", normal_legato);

  sub_mix->addSource(make_metronome_stream());
  return sub_mix;
}

mu::Stream *make_outro_stream() {
  mu::AddStream *sub_mix = new mu::AddStream();
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);
  sub_mix->addSource(bass->getStream());
  printf("bass.getStream() = %p (%s)\n", bass->getStream(), bass->getStream()->getClassName().c_str());

  bass->setTime(0);

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  sub_mix->addSource(make_metronome_stream());
  return sub_mix;
}


// ================================================================

int main() {

  mu::ConstantStream master_gain;
  mu::MultiplyStream master_fader;
  mu::ReverbStream reverb_stream;
  mu::AddStream main_mix;
  mu::FileWriteStream file_write_stream;
  mu::RtPlayer player;
  mu::DelayStream 
    intro_delay, 
    vers1_delay, 
    vers2_delay, 
    chor1_delay, 
    vers3_delay, 
    chor2_delay, 
    bridg_delay, 
    outro_delay;

  fprintf(stderr,"%d, %f, %f, %f\n", 
          MEASURE_TO_BEAT(1),
          MEASURE_TO_FRAME(1),
          BEAT_TO_FRAME(4),
          BEAT_TO_SECOND(1));
  intro_delay.setSource(make_intro_stream());
  vers1_delay.setSource(make_vers1_stream());
  vers2_delay.setSource(make_vers2_stream());
  chor1_delay.setSource(make_chor1_stream());
  vers3_delay.setSource(make_vers3_stream());
  chor2_delay.setSource(make_chor2_stream());
  bridg_delay.setSource(make_bridg_stream());
  outro_delay.setSource(make_outro_stream());

  main_mix.
    addSource(&(intro_delay.setDelay(MEASURE_TO_FRAME(M_INTRO)))).
    addSource(&(vers1_delay.setDelay(MEASURE_TO_FRAME(M_VERS1)))).
    addSource(&(vers2_delay.setDelay(MEASURE_TO_FRAME(M_VERS2)))).
    addSource(&(chor1_delay.setDelay(MEASURE_TO_FRAME(M_CHOR1)))).
    addSource(&(vers3_delay.setDelay(MEASURE_TO_FRAME(M_VERS3)))).
    addSource(&(chor2_delay.setDelay(MEASURE_TO_FRAME(M_CHOR2)))).
    addSource(&(bridg_delay.setDelay(MEASURE_TO_FRAME(M_BRIDG)))).
    addSource(&(outro_delay.setDelay(MEASURE_TO_FRAME(M_OUTRO))));

  master_gain.setValue(0.25);

  master_fader.addSource(&master_gain);
  master_fader.addSource(&main_mix);

  reverb_stream.setSource(&master_fader).setRatio(0.20);
  file_write_stream.setSource(&reverb_stream).
    setFileName("/users/r/Projects/Mu/binary/tnvm.wav");
  player.setSource(&file_write_stream);

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  
  return 0;
}
