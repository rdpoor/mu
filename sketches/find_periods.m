# use cross-correlation to find periodicity within a sound file

pkg load signal;

function signal = load_signal(filename = "/Users/r/Projects/mu/SoundSets/tmp/69_resampled.wav")
  # return just one channel of data
  signal = wavread(filename)(:, 1);
endfunction

function [r, lags] = single_sided_xcorr(signal, offset = 1, window = 2048)
  [r, lags] = xcorr(signal(offset:offset + window), 'coeff');
  len = length(r);
  r = r(ceil(len/2):len);
  lags = lags(ceil(len/2):len);
  # plot(lags, r)
endfunction

function period = find_period(r, lags, est=1000, tolerance=0.1)
  minp = floor(est * (1 - tolerance));
  maxp = floor(est * (1 + tolerance));
  selector = ((lags > minp) & (lags < maxp));
  [x, ix] = max(r(selector));
  period = lags(selector)(ix);
endfunction

function periods = doit(signal, window = 2048, est_period = 1000, tolerance = 0.2)
  offset = 1
  periods = []
  while (offset + window < length(signal))
    [r, lags] = single_sided_xcorr(signal, offset, window);
    p = find_period(r, lags, est_period, tolerance);
    if (mod(offset, 100) == 0)
      printf("offset=%d, period=%d\n", offset, p);
      fflush(stdout);
    endif
    periods = [periods, p];
    offset = offset + 1;
  endwhile
endfunction
  
function periods = blooper(signal, window = 2048)
  start = 1
  periods = []
  while (start + window < length(signal))
    printf("start = %d\n", start);
    [r, lags] = xcorr(signal(start+1:start+window, 1), signal(start+1:start+window, 1));

    subplot(1,2,1);
    plot(signal(start+1:start+window, 1));
    subplot(1,2,2);
    plot(lags, r); 

    sleep(1)

    [peaks, locs] = findpeaks(max(r, 1)); 
    if (length(locs) > 1)
      period = locs(2)
      start = start + period;
      periods = [periods, period];
    else
      printf("punting at start = %d\n", start);
      return
    endif
  endwhile
endfunction

  

# Local Variables:
# mode: octave
# End:
