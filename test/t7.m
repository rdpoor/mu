## sxf: Synchronous Cross-Fade time scale modification algorithm.
##
## sxf is a time-scale modification algorithm that can independently
## modify the duration and pitch of a quasi-periodic audio waveform.
##
## SXF takes as inputs a waveform and two time-varying signals: Tau(t)
## and Phi(t). Tau(t) determines the "position" in the waveform and
## varies from 0 to n-1, where n is the number of samples in the
## waveform. Phi(t) determines the instantaneous phase expressed in
## samples, so the slope of Phi(t) determines the pitch relative to the
## original: a slope of 2.0 will double the pitch.
##
## At its core, sxf defines a function: S(t) = f(Tau(t), Phi(t)) that
## defines the sample generated at time t.
##
## THIS VERSION t7:
## - works well
## - is "stateful" -- it maintains (and modifies) phi from one step
##   to the next.
## - uses xcorr() to determine the period.

pkg load signal;

## ================================================================================
## main
##
function [Signal, Period] = test_t7(dtau, dphi, estimated_period, filename)
  printf("setting up..."); fflush(stdout);
  Signal = load_signal();
  Period = computePeriod(Signal, estimated_period);
  printf("\n"); 

  P = zeros(ceil(length(Signal)/dtau), 1);
  tau = 0.0;
  phi = 0.0;
  for t=1:length(P)
    [s, phi] = generate_sample(Signal, Period, tau, phi, t);
    P(t) = s;
    tau += dtau;
    phi += dphi;
  endfor
  wavwrite(P, 44100, 16, filename);
endfunction

## ================================================================
## ================================================================
## inner loop methods

## compute one sample for a given tau and phi.
function [s, phi] = generate_sample(Signal, Period, tau, phi, t)
  period = getPeriod(Period, tau);

  # adjust phi if it is more than one period away
  # from tau
  while (phi > tau)
    phi -= period;
  endwhile
  while (phi < tau - period)
    phi += period;
  endwhile
 
  ## phi <= tau < phi + period

  if (phi == tau)
    s = fetch(Signal, tau);
    return;
  end

  ## phi < tau < phi + period
    
  alpha = (tau - phi) / period;
  s0 = fetch(Signal, phi);
  s1 = fetch(Signal, phi + period);
  s = s0 + (s1 - s0) * alpha;

  show_progress = mod(t, 1000) == 0;
  if (show_progress)
    i = max(1,floor(tau - 1.5*period)):min(length(Signal),floor(tau + 1.5*period));
    itau = max(1,min(length(Signal), floor(tau)));
    it0 = max(1,min(length(Signal), floor(phi)));
    it1 = max(1,min(length(Signal), floor(phi+period)));
    printf("t=%d, tau=%f, phi=%f, phi+period=%f, period=%f\n", t, tau, phi, phi + period, period);
    fflush(stdout);

    plot(i, Signal(i),
         [it1;it1],[-0.5, 0.5],
         [itau;itau],[-0.5, 0.5],
         [it0;it0],[-0.5, 0.5]);
    sleep(0.1);
  endif

endfunction

## Fetch instantaneous period for a specific (possibly fractional)
## Signal(t).
function period = getPeriod(Period, tau)
  if (tau < 1)
    period = Period(1);
  elseif (tau >= length(Period)+1)
    period = Period(length(Period));
  else
    period = Period(floor(tau));
  end
endfunction

## Fetch an interpolated sample from signal.  If phi is outside the
## bounds of signal, return zero
function s = fetch(signal, phi)
  iphi = floor(phi);
  if ((iphi < 1) || (iphi >= length(signal)))
    s = 0;
  elseif (iphi == phi)
    s = signal(iphi);
  else
    alpha = phi - iphi;
    s0 = signal(iphi);
    s1 = signal(iphi+1);
    s = s0 + alpha * (s1 - s0);
  endif
endfunction

## ================================================================
## ================================================================
## Setup / One-time functions below here

function signal = load_signal(filename = "/Users/r/Projects/mu/SoundSets/tmp/69.wav")
  signal = wavread(filename)(:, 1);
endfunction

## Return a sub-signal that is 2 * widow_size - 1 samples wide, centered
## around tau.  The signal is zero padded at the ends.
function s = extract_subsignal(signal, tau, window_size)
  min_i = floor(tau - window_size + 1);
  max_i = floor(tau + window_size - 1);
  z0 = zeros(max(0, 1-min_i), 1);
  s1 = signal(max(min_i, 1):min(max_i, length(signal)), 1);
  z2 = zeros(max(0, max_i-length(signal)), 1);
  s = [z0; s1; z2];
endfunction

## Find the period around sample tau in the signal. We do this by taking
## two snippets of the signal: one at tau+estimated_period and one at
## tau-estimated_period. We cross-correlate the two snippets and use the
## peak to correct the estimated period.
##
## This works well, but fails within 1.5 periods of the beginning and end
## of the signal.  As a heuristic, we simply substitute estimated_period
## if the result is more than 20% off from estimated_period.
##
## This could be improved: it uses the integer sample # as the peak.  But 
## the corellator output is a curve - consider using polyfit or splinefit
## as a way to find the true (interpolated) peak.  For example, see:
## 
##   https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
##
## as a good way to find the fractional peal

function p = find_period(signal, tau, estimated_period)
  # window = bartlett(2 * estimated_period - 1);
  s0 = extract_subsignal(signal, tau - estimated_period, estimated_period);
  s1 = extract_subsignal(signal, tau + estimated_period, estimated_period);
  [R, lag] = xcorr(s0, s1);
  [i, ix] = max(R);
  p = estimated_period - lag(ix)/2;

  ## fix period if it is out of bounds
  if ((p < estimated_period * 0.80) || (p > estimated_period * 1.2))
    p = estimated_period;
  endif
endfunction

## Compute instantaneous frequency (period) of the waveform.  Called once
## at initialization.
function Period = computePeriod(Signal, estimated_period)
  Period = zeros(length(Signal), 1);

  for t=1:length(Signal)
    Period(t) = find_period(Signal, t, estimated_period);
  endfor
endfunction

## Local Variables:
## mode: octave
## End:
