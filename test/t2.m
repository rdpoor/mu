# Find splice points in a quasi-period signal
#
# Given:
#  S(t): a quasi-periodic signal
#  period: the estimated period of S
# Find:
#  R(t), the one-period lags where S(t) is maximally self-similar.
#
# In essense, R(t) answers the question: for a given value of t,
# how many samples should I skip ahead to get to a similar point
# in the next period of S(t)?
#
# Algorithm:
#
# Given S (the signal) and t (the sample index at which we want
# to start a splice), find t', which is the optimal sample index
# for ending the splice.  In this case, "optimal" means that S(t)
# and S(t') have maximum cross correlation.  
#
# To find t', we apply a Bartlett window around S(t) to form s1
# and another around S(t+period) to form s2.  We then take the
# cross-correlation of s1 and s2 and find the lag at which the
# cross-correlation is at a maxiumum.  period + lag(maximum) is
# the delta between t and t'.
#
# Edge cases:
#
# At the start of S(t), i.e. t < window_size/2, we zero pad s1.
# Ad the end of S(t), i.e. t > length(S) - period - window_size/2,
# we zero pad s2.  And if t > length(S) - period, t' is set to t,
# indicating that the best splice point is t itself (otherwise
# we run out of signal).

pkg load signal;

# estimated_period may be negative to indicate x_prime should be in the period before t.
function lag = find_lag(signal, t, estimated_period, window_size)
  if ((t < 1) || (t > length(signal) - estimated_period))
    # quick exit if beyond the end of the signal
    lag = 0;
    return;
  endif

  # define window function
  window = bartlett(window_size);

  window = bartlett(window_size);
  s1 = extract_subsignal(signal, t, window_size) .* window;
  s2 = extract_subsignal(signal, t + estimated_period, window_size) .* window;

  [r, lags] = xcorr(s2, s1);
  [x, ix] = max(r);
  lag = estimated_period + lags(ix);
endfunction

function s = extract_subsignal(signal, t, window_size)
  s_start = floor(t - window_size/2);
  s_end = s_start + window_size - 1; # inclusive
  if (s_start < 1)
    s = [zeros(1 - s_start, 1); signal(1:s_end, 1)];
  elseif (s_end > length(signal))
    s = [signal(s_start:length(signal)); zeros(s_end - length(signal), 1)];
  else
    s = signal(s_start:s_end);
  endif
end

function signal = load_signal(filename = "/Users/r/Projects/mu/SoundSets/tmp/69_resampled.wav")
  signal = wavread(filename)(:, 1);
endfunction

function testlag(signal, t, window_size, estimated_period = 1000, n_periods = 4000)
  lag = find_lag(signal, t, estimated_period, window_size)
  s1 = extract_subsignal(signal, t, n_periods);
  s2 = extract_subsignal(signal, t+lag, n_periods);
  figure(1)
  plot(1:length(s1), s1, 1:length(s2), s2);
  figure(2)
  plot(s1, s2);
endfunction
  
function lags = test(signal, t0, t1, window_size, estimated_period = 1000)
  lags = [];
  for t = t0:t1
    # test hack: set estimated_period to previous lag.  This may improve
    # correlation from period to period.
    # lag = find_lag(signal, t, max(lag, estimated_period/2), window_size);
    # Nope.  It doesn't help.  And it gets confused at startup.  So keep it
    # simple:
    lag = find_lag(signal, t, estimated_period, window_size);
    lags = [lags; lag];

    if (mod(t, 1000) == 0)
      printf("t = %d, lag = %d\n", t, lag);
      fflush(stdout);
    endif
  endfor
endfunction

# Local Variables:
# mode: octave
# End:
