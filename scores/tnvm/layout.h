#ifndef TNVM_LAYOUT_H
#define TNVM_LAYOUT_H

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
