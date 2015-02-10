// Sketch

#include "mu.h"
#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define THUMPS_AND_SCRATCHES_DIRECTORY "/Users/r/Projects/Mu/SoundSets/TAS/"
#define PLUCKED_NOTE_DIRECTORY "/Users/r/Projects/Mu/SoundSets/A/"

mu::MuFloat beats_per_minute() {
  return 100.0;
}

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  mu::MuFloat tics_per_beat = 44100 * 60.0 / beats_per_minute();
  return beat * tics_per_beat;
}

mu::MuFloat pitch_to_ratio(mu::MuFloat sample, mu::MuTick tick) {
  (void) tick;
  return mu::MuUtils::pitch_to_ratio(sample);
}

class RoomTone {
public:
  RoomTone() {
    room_ = new mu::FileReadStream();
    loop_ = new mu::LoopStream();
    room_->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY "room_tone.wav");
    loop_->set_source(room_);
    loop_->set_interval(room_->duration());
    loop_->set_source_end(room_->duration());
  }
    
  mu::MuStream *stream() {
    return loop_;
  }
    
protected:
  mu::FileReadStream *room_;
  mu::LoopStream *loop_;
};                              // class RoomTone

class Metronome {
public:
  Metronome() {
    thump_ = new mu::FileReadStream();
    loop_stream_ = new mu::LoopStream();
    thump_->set_file_name(THUMPS_AND_SCRATCHES_DIRECTORY "s12.wav");
    loop_stream_->set_source(thump_);
    loop_stream_->set_interval(beat_to_tick(1.0));
    loop_stream_->set_source_end(beat_to_tick(1.0));
  }
  mu::MuStream *stream() {
    return loop_stream_;
  }
protected:
  mu::ChannelFitterStream *channel_fitter_;
  mu::FileReadStream *thump_;
  mu::LoopStream *loop_stream_;
};                              // class Metronome
  
    
class Plucks {
public:
  mu::MuStream *hammer_on(mu::MuStream *s1, mu::MuStream *s2, mu::MuTick dur, double gain1) {
    mu::CropStream *c1 = new mu::CropStream();
    c1->set_source(s1);
    c1->set_source_end(dur);
    mu::CropStream *c2 = new mu::CropStream();
    c2->set_source(s2);
    c2->set_source_start(dur);
    mu::SequenceStream *seq = new mu::SequenceStream();
    seq->add_source(c1, 0, gain1);
    seq->add_source(c2, 0, 1.0);
    return seq;
  }
    
  Plucks() {
    f67_.set_file_name(PLUCKED_NOTE_DIRECTORY "67.wav");
    f72_.set_file_name(PLUCKED_NOTE_DIRECTORY "72.wav");
    f74_.set_file_name(PLUCKED_NOTE_DIRECTORY "74.wav");

    mu::SequenceStream *sequence_stream = new mu::SequenceStream();
    sequence_stream->add_source(f67_.clone(), beat_to_tick(0.00), 1.0);
    sequence_stream->add_source(f67_.clone(), beat_to_tick(0.75), 1.0);
    sequence_stream->add_source(hammer_on(f72_.clone(),
                                          f74_.clone(),
                                          beat_to_tick(0.15),
                                          1.0), beat_to_tick(1.50), 1.0);
    sequence_stream->add_source(f72_.clone(), beat_to_tick(2.50), 1.0);
    sequence_stream->add_source(f72_.clone(), beat_to_tick(3.00), 1.0);
    sequence_stream->add_source(hammer_on(f72_.clone(),
                                          f74_.clone(),
                                          beat_to_tick(0.15),
                                          1.0), beat_to_tick(4.00), 1.0);
    sequence_stream->add_source(f72_.clone(), beat_to_tick(4.75), 1.0);
    sequence_stream->add_source(f74_.clone(), beat_to_tick(5.50), 1.0);
    sequence_stream->add_source(f74_.clone(), beat_to_tick(6.50), 1.0);
    sequence_stream->add_source(f74_.clone(), beat_to_tick(7.00), 1.0);

    loop_stream_ = new mu::LoopStream();
    loop_stream_->set_source(sequence_stream);
    loop_stream_->set_interval(beat_to_tick(8));
    loop_stream_->set_source_end(beat_to_tick(8));
  }

  mu::MuStream *stream() {
    return loop_stream_;
  }

protected:
  mu::FileReadStream f67_;
  mu::FileReadStream f72_;
  mu::FileReadStream f74_;
  mu::LoopStream *loop_stream_;


};                              // plucks

int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  // Metronome *metronome = new Metronome();
  RoomTone *room_tone = new RoomTone();
  Plucks *plucks = new Plucks();
  mu::SumStream *mix = new mu::SumStream();

  printf("1 beat = %ld ticks\n", beat_to_tick(1));

  mix->add_source(room_tone->stream());
  mix->add_source(plucks->stream());
  // mix->add_source(metronome->stream());

  printf("mix:\n%s\n", mix->inspect().c_str());

  transport.set_player(&player_rt);
  transport.set_source(mix);

  transport.run();
  wait_for_input();
  transport.stop();
  
  return 0;
}
