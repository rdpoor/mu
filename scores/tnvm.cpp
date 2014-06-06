/*
 * Together Now, Very Minor
 */
#include "mu.h"

#include "NRev.h"
#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "linseg_stream.h"
#include "multiply_stream.h"
#include "null_stream.h"
#include "reverb_stream.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

// TIMING
#define FRAMES_PER_SECOND 44100
#define SECONDS_PER_MINUTE 60.0

#define BEATS_PER_MINUTE 90.0
#define BEATS_PER_MEASURE 4

#define BEAT_TO_SECOND(__b__) (__b__ * SECONDS_PER_MINUTE / BEATS_PER_MINUTE)
#define BEAT_TO_FRAME(__b__) (BEAT_TO_SECOND(__b__) * FRAMES_PER_SECOND)
#define M_TO_BEAT(__m__) (__m__ * BEATS_PER_MEASURE)
#define M_TO_FRAME(__m__) (BEAT_TO_FRAME(M_TO_BEAT(__m__)))

#define M_INTRO 0
#define M_VERS1 (M_INTRO + 8)
#define M_CHOR1 (M_VERS1 + 8)
#define M_VERS2 (M_CHOR1 + 12)
#define M_CHOR2 (M_VERS2 + 8)
#define M_BRIDG (M_CHOR2 + 12)
#define M_OUTRO (M_BRIDG + 12)
#define M_END (M_OUTRO + 12)



int main() {

  mu::ReverbStream reverb_stream;
  mu::AddStream main_mix;
  mu::FileWriteStream file_write_stream;
  mu::RtPlayer player;
  mu::DelayStream 
    intro_delay, 
    vers1_delay, 
    chor1_delay, 
    vers2_delay, 
    chor2_delay, 
    bridg_delay, 
    outro_delay;

  fprintf(stderr,"%d, %f, %f, %f\n", 
          M_TO_BEAT(1),
          M_TO_FRAME(1),
          BEAT_TO_FRAME(4),
          BEAT_TO_SECOND(1));
  main_mix.
    addSource(&(intro_delay.setDelay(M_TO_FRAME(M_INTRO)))).
    addSource(&(vers1_delay.setDelay(M_TO_FRAME(M_VERS1)))).
    addSource(&(chor1_delay.setDelay(M_TO_FRAME(M_CHOR1)))).
    addSource(&(vers2_delay.setDelay(M_TO_FRAME(M_VERS2)))).
    addSource(&(chor2_delay.setDelay(M_TO_FRAME(M_CHOR2)))).
    addSource(&(bridg_delay.setDelay(M_TO_FRAME(M_BRIDG)))).
    addSource(&(outro_delay.setDelay(M_TO_FRAME(M_OUTRO))));
  
  return 0;
}
