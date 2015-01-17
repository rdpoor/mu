#include "gtest/gtest.h"
#include "sine_stream.h"
#include "buffer_fixture.h"
#include <cmath>

class SineStream : public BufferFixture {
protected:
  void render(mu::MuTick start_tick, double freq, double ampl, double phas) {
    sine_stream_.set_frequency(freq);
    sine_stream_.set_amplitude(ampl);
    sine_stream_.set_phase(phas);
    sine_stream_.render(buffer_, start_tick);
  }

  void verify(mu::MuTick start_tick, double freq, double ampl, double phas) {
    double omega = (freq * 2.0 * M_PI) / buffer_.dataRate();
    int n_channels = buffer_.channels();

    for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
      mu::MuFloat expected = ampl * sin((tick + start_tick) * omega + phas);
      for (int channel = n_channels-1; channel >= 0; channel--) {
        mu::MuFloat actual = buffer_(tick, channel);
        ASSERT_DOUBLE_EQ(expected, actual);
      }
    }
  }

  mu::SineStream sine_stream_;
};

TEST_F(SineStream, Basic) {
  mu::MuTick start_tick = 0;
  double freq = 440.0;
  double ampl = 1.0;
  double phas = 0.0;

  render(start_tick, freq, ampl, phas);
  verify(start_tick, freq, ampl, phas);
}

TEST_F(SineStream, NonZeroStart) {
  mu::MuTick start_tick = 123;
  double freq = 440.0;
  double ampl = 1.0;
  double phas = 0.0;

  render(start_tick, freq, ampl, phas);
  verify(start_tick, freq, ampl, phas);
}

