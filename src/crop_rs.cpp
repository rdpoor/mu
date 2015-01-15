#include "crop_rs.h"
#include <stdio.h>

namespace mu {

  CropRS::CropRS() : crop_start_( 0 ), crop_end_( 0 ) { 
    source_ = NULL;
  }

  CropRS::~CropRS() {
  }

  bool CropRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if ((source_ == NULL) || (crop_end_ <= start_tick) || (crop_start_ >= end_tick)) {
      // no source or nor overlap
      return false;
    }

    MuTick cropped_start = std::max(start_tick, crop_start_);
    MuTick cropped_end = std::min(end_tick, crop_end_);
    // printf("base=%ld, start=%ld, end=%ld, cstart=%ld, cend=%ld, cdstart=%ld, cdend=%ld\n",
    // base_tick, start_tick, end_tick, crop_start_, crop_end_, cropped_start, cropped_end);
    return (source_->render(frames, base_tick, cropped_start, cropped_end));
  }


}
