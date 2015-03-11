// Sketch for plucked stting: pluck, hammer on/off, rest

#include "mu.h"
#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLES_DIRECTORY "/Users/r/Projects/Mu/examples/"

// #define THUMPS_AND_SCRATCHES_DIRECTORY "/Users/r/Projects/Mu/SoundSets/TAS/"
// #define PLUCKED_NOTE_DIRECTORY "/Users/r/Projects/Mu/SoundSets/A/"

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

// ================================================================
class RoomTone {
public:
  RoomTone() {
    room_ = new mu::FileReadStream();
    loop_ = new mu::LoopStream();
    room_->set_file_name(EXAMPLES_DIRECTORY "room_tone.wav");
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

// ================================================================
class Metronome {
public:
  Metronome() {
    thump_ = new mu::FileReadStream();
    loop_stream_ = new mu::LoopStream();
    thump_->set_file_name(EXAMPLES_DIRECTORY "tick.wav");
    loop_stream_->set_source(thump_);
    loop_stream_->set_interval(beat_to_tick(1.0));
    loop_stream_->set_source_end(beat_to_tick(1.0));
  }
  mu::MuStream *stream() {
    return loop_stream_;
  }
protected:
  mu::FileReadStream *thump_;
  mu::LoopStream *loop_stream_;
};                              // class Metronome
  
    
// ================================================================
// pluck, hammer on / off, mute a note.  The "note" is a stream, but
// usually a plucked string sound file.  Note that a final dampen()
// must be called to flush the final note.
class PString {
public:
  PString() {
    sequence_stream_ = new mu::SequenceStream();
    prev_note_ = NULL;
  }
  ~PString() {
    delete sequence_stream_;
    if (prev_note_ != NULL) delete prev_note_;
  }

  // start playing a note
  void pluck(mu::MuStream *source, mu::MuTick start, mu::MuFloat gain = 1.0) {
    mu::MuTick elapsed = start - prev_start_;
    flush_prev(elapsed);
    prev_note_ = source;
    prev_start_ = start;
    prev_gain_ = gain;
  }

  // truncate the previous note, start the new note 'd' seconds after its
  // start, where 'd' is the time elapsed between previous note and this.
  void hammer(mu::MuStream *source, mu::MuTick start, mu::MuFloat gain = 1.0) {
    if (prev_note_ == NULL) {
      pluck(source, start, gain);
    } else {
      mu::MuTick elapsed = start - prev_start_;
      mu::CropStream *c = new mu::CropStream();
      c->set_source(source);
      c->set_source_start(elapsed);
      flush_prev(elapsed);
      prev_note_ = c;
      // prev_start_ = prev_start_;
      prev_gain_ = gain;
    }
  }

  // stop playing the current note.  In actuality, this emits the previous
  // note, and you must call dampen() to cause the final note to be emitted.
  void dampen(mu::MuTick start) {
    mu::MuTick elapsed = start - prev_start_;
    flush_prev(elapsed);
  }

  // return the stream with the collection of plucked, hammered and dampened
  // notes.
  mu::MuStream *stream() {
    return sequence_stream_;
  }
protected:
  mu::MuStream *prev_note_;
  mu::MuTick prev_start_;
  mu::MuFloat prev_gain_;
  mu::SequenceStream *sequence_stream_;
  void flush_prev(mu::MuTick start) {
    if (prev_note_ != NULL) {
      mu::CropStream *c = new mu::CropStream();
      c->set_source(prev_note_);
      c->set_source_end(start);
      sequence_stream_->add_source(c, prev_start_, prev_gain_);
      prev_note_ = NULL;
    }
  }
};

class Plucks2 {
public:
  mu::MuStream *setup() {
    PString *p = new PString();
    p->pluck(frs("67.wav"), beat_to_tick(0.00), 1.0);
    p->pluck(frs("67.wav"), beat_to_tick(0.75), 1.0);
    p->pluck(frs("72.wav"), beat_to_tick(1.50), 1.0);
    p->hammer(frs("74.wav"), beat_to_tick(1.65), 1.0);
    p->pluck(frs("72.wav"), beat_to_tick(2.50), 1.0);
    p->pluck(frs("72.wav"), beat_to_tick(3.00), 1.0);
    p->pluck(frs("72.wav"), beat_to_tick(4.00), 1.0);
    p->hammer(frs("74.wav"), beat_to_tick(4.15), 1.0);
    p->pluck(frs("72.wav"), beat_to_tick(4.75), 1.0);
    p->pluck(frs("74.wav"), beat_to_tick(5.50), 1.0);
    p->pluck(frs("74.wav"), beat_to_tick(6.50), 1.0);
    p->pluck(frs("74.wav"), beat_to_tick(7.00), 1.0);
    p->dampen(beat_to_tick(8.00));

    PString *q = new PString();
    q->pluck(frs("60.wav"), beat_to_tick(3.50), 1.0);
    q->dampen(beat_to_tick(5.50));

    mu::SumStream *sstream = new mu::SumStream();
    sstream->add_source(p->stream());
    sstream->add_source(q->stream());

    mu::LoopStream *loop_stream = new mu::LoopStream();
    loop_stream->set_source(sstream);
    loop_stream->set_interval(beat_to_tick(8.00));
    loop_stream->set_source_end(beat_to_tick(8.00));

    return loop_stream;
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(EXAMPLES_DIRECTORY + name);
    return s;
  }

};

class Plucks3 {
public:
  mu::MuStream *setup() {
    PString *p = new PString();
    p->pluck(frs("31.wav"), beat_to_tick(0.00), 1.0);
    p->pluck(frs("36.wav"), beat_to_tick(1.50), 1.0);
    p->pluck(frs("29.wav"), beat_to_tick(4.00), 1.0);
    p->pluck(frs("38.wav"), beat_to_tick(5.50), 1.0);
    p->dampen(beat_to_tick(8.00));

    mu::LoopStream *loop_stream = new mu::LoopStream();
    loop_stream->set_source(p->stream());
    loop_stream->set_interval(beat_to_tick(8.00));
    loop_stream->set_source_end(beat_to_tick(8.00));

    return loop_stream;
  }

  mu::FileReadStream *frs(std::string name) {
    mu::FileReadStream *s = new mu::FileReadStream();
    s->set_file_name(EXAMPLES_DIRECTORY + name);
    return s;
  }

};

int main() {
  mu::PlayerRt player_rt;
  mu::Transport transport;
  Metronome *metronome = new Metronome();
  RoomTone *room_tone = new RoomTone();
  Plucks2 *plucks2 = new Plucks2();
  Plucks3 *plucks3 = new Plucks3();
  mu::SumStream *mix = new mu::SumStream();

  printf("1 beat = %ld ticks\n", beat_to_tick(1));

  mix->add_source(room_tone->stream());
  mix->add_source(plucks2->setup());
  mix->add_source(plucks3->setup());
  mix->add_source(metronome->stream());

  printf("mix:\n%s\n", mix->inspect().c_str());

  transport.set_player(&player_rt);
  transport.set_source(mix);

  transport.run();
  wait_for_input();
  transport.stop();
  
  return 0;
}
