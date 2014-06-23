#ifndef TNVM_TNVM_H
#define TNVM_TNVM_H

// Lazy and pragatic: get all the mu headers
// TODO: This should be part of the mu distribution
#include "mu.h"

#include "add_stream.h"
#include "channelizer_stream.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "identity_stream.h"
#include "linseg_stream.h"
#include "loop_stream.h"
#include "map_stream.h"
// #include "multi_source_stream.h"
#include "multiply_stream.h"
#include "nrt_player.h"
#include "null_stream.h"
#include "pitch_utilities.h"
// #include "player.h"
#include "probability_stream.h"
#include "psi_stream.h"
#include "psi_waveform.h"
#include "random_select_stream.h"
#include "resample_stream.h"
#include "reverb_stream.h"
#include "rt_player.h"
#include "sequence_stream.h"
#include "sine_stream.h"
// #include "single_source_stream.h"
#include "splice_stream.h"
#include "stream.h"

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

#endif
