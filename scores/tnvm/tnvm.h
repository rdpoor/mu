#ifndef TNVM_TNVM_H
#define TNVM_TNVM_H

// Lazy and pragatic: get all the mu headers
// TODO: This should be part of the mu distribution
#include "mu.h"

#include "add_sp.h"
#include "channelizer_sp.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "fade_sp.h"
#include "file_read_sp.h"
#include "file_write_sp.h"
#include "identity_sp.h"
#include "linseg_sp.h"
#include "loop_sp.h"
#include "map_sp.h"
// #include "multi_source_sp.h"
#include "multiply_sp.h"
#include "nrt_player.h"
#include "null_sp.h"
#include "pitch_utilities.h"
// #include "player.h"
#include "probability_sp.h"
#include "psi_sp.h"
#include "psi_waveform.h"
#include "random_select_sp.h"
#include "resample_sp.h"
#include "reverb_sp.h"
#include "rt_player.h"
#include "sample_processor.h"
#include "sequence_sp.h"
#include "sine_sp.h"
// #include "single_source_sp.h"
#include "splice_sp.h"

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
