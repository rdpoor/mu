#include "gtest/gtest.h"
#include "sine_stream.h"
#include "buffer_fixture.h"
#include <cmath>

class SineStream : public BufferFixture {
protected:
  void render(mu::MuTick buffer_start, double freq, double ampl, double phas) {
    sine_stream_.set_f0(freq);
    sine_stream_.set_a0(ampl);
    sine_stream_.set_p0(phas);
    sine_stream_.render(buffer_start, &buffer_);
  }

  void verify(mu::MuTick buffer_start, double freq, double ampl, double phas) {
    double omega = (freq * 2.0 * M_PI);
    int n_channels = buffer_.channels();

    for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
      double tau = (tick + buffer_start) / buffer_.dataRate();
      mu::MuFloat expected = ampl * sin(tau * omega + phas);
      for (int channel = n_channels-1; channel >= 0; channel--) {
        mu::MuFloat actual = buffer_(tick, channel);
        ASSERT_DOUBLE_EQ(expected, actual);
      }
    }
  }

  mu::SineStream sine_stream_;
};

TEST_F(SineStream, Basic) {
  mu::MuTick buffer_start = 0;
  double freq = 440.0;
  double ampl = 1.0;
  double phas = 0.0;

  render(buffer_start, freq, ampl, phas);
  verify(buffer_start, freq, ampl, phas);
}

TEST_F(SineStream, NonZeroStart) {
  mu::MuTick buffer_start = 123;
  double freq = 440.0;
  double ampl = 1.0;
  double phas = 0.0;

  render(buffer_start, freq, ampl, phas);
  verify(buffer_start, freq, ampl, phas);
}

