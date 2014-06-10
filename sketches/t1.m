# Given:
#  S(t), a quasi-periodic signal
#  period, the estimated period of s
# Find:
#  R(t), the one-period lags where S(t) is maximimally self-similar, i.e. 
#  s(t + r(t)) approximately equals s(t).
# Ideally, R(t) should be "smooth".
#
# Results: Adding a Bartlett windowing function on the signals before
# passing them to xcorr() makes a substantial improvement.  The results
# look promising.  Next step: how well does the forward-looking lag
# correspond to the backward-looking lead?

pkg load signal;

function s = load_signal(filename = "/Users/r/Projects/mu/SoundSets/tmp/69_resampled.wav")
  # return just one channel of data
  s = wavread(filename)(:, 1);
endfunction

function lag = find_lag(signal, src_offset, period)
  # at far end of signal, lag = 0 (last period is
  # identical to itself)
  if (src_offset > length(signal)-period)
    lag = 0;
    return;
  endif

  src_lo = floor(src_offset - period / 2);
  src_hi = src_lo + period;
  dst_lo = src_hi;
  dst_hi = dst_lo + period;
  # truncate analysis window at extreme ends
  if (src_lo < 1)
    d = 1 - src_lo;
    src_lo += d;
    dst_lo += d;
  elseif (dst_hi > length(signal))
    d = dst_hi - length(signal);
    src_hi -= d;
    dst_hi -= d;
  endif

  sig1 = signal(src_lo:src_hi) .* bartlett(src_hi - src_lo + 1);
  sig2 = signal(dst_lo:dst_hi) .* bartlett(src_hi - src_lo + 1);
  [r, lags] = xcorr(sig2, sig1);
  [x, ix] = max(r);
  
  lag = period + lags(ix);

  if (mod(src_offset, 1000) == 0)
    printf("src_offset = %d, lag = %d\n", src_offset, lag);
    fflush(stdout);
    # plot(lags, r);
    # sleep(1);
  endif

endfunction

function lags = find_lags(signal, period)
  lags = [];
  for i = 1:length(signal)/10 # i = floor(length(signal)*0.9):length(signal) # i = 1:length(signal)/10
    lags = [lags, find_lag(signal, i, period)];
  endfor
endfunction

# Local Variables:
# mode: octave
# End:
