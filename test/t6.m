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
## today we want to generate a series Theta(t) which defines the
## instantaneous phase of Waveform(t).  Theta(t) ramps from zero to 1.0
## at each period.
##
## NOTE: At present, the only way I can see to write the sfx algorithm
## is to make it "stateful", where phi is updated modulo the period at
## each step.

pkg load signal;

## ================================================================================
## main
##
function [Signal, Theta, Omega] = test_t6(dtau, dphi, estimated_period, filename)
  printf("setting up..."); fflush(stdout);
  Signal = load_signal();
  [Theta, Omega] = computeThetaOmega(Signal, estimated_period);
  printf("\n"); fflush(stdout);

  P = zeros(ceil(length(Signal)/dtau), 1);
  for t=1:length(P)
    P(t) = generate_sample(Signal, Theta, Omega, t*dtau, t*dphi, t);
  endfor
  wavwrite(P, 44100, 16, filename);
endfunction

## ================================================================
## ================================================================
## inner loop methods

## compute one sample for a given tau and phi.
function s = generate_sample(Signal, Theta, Omega, tau, phi, t)
  [theta, omega] = getThetaOmega(Theta, Omega, tau);
  period = 1/omega;
  phi = mod(phi, period) / period;
  if (phi == theta)
    s = fetch(Signal, tau);
  elseif (phi < theta)
    t0 = tau - (theta - phi) * omega;
    t1 = t0 + period;
  else                          # phi > theta
    t1 = tau + (phi - theta) * omega;
    t0 = t1 - period;
  endif
  alpha = (tau - t0) / (t1 - t0);
  s1 = fetch(Signal, t1);
  s0 = fetch(Signal, t0);
  s = s0 + (s1 - s0) * alpha;

  show_progress = mod(t, 1000) == 0;
  if (show_progress)
    i = max(1,floor(tau - 1.5*period)):min(length(Signal),floor(tau + 1.5*period));
    itau = max(1,min(length(Signal), floor(tau)));
    it0 = max(1,min(length(Signal), floor(t0)));
    it1 = max(1,min(length(Signal), floor(t1)));
    printf("t=%d, tau=%f, phi=%f, theta=%f, omega=%f, period=%f t0=%f, t1=%f\n", 
           t, tau, phi, theta, omega, period, t0, t1);
    fflush(stdout);

    plot(i, Signal(i),
         i, Theta(i) - 0.5,
         i, Omega(i),
         [it1;it1],[-0.5, 0.5],
         [itau;itau],[-0.5, 0.5],
         [it0;it0],[-0.5, 0.5]);
    sleep(0.1);
  endif

endfunction

## Fetch instantaneous phase (theta) and frequency (omega) for a
## specific (possibly fractional) Signal(t).
function [theta, omega] = getThetaOmega(Theta, Omega, tau)
  itau = max(1, min(length(Theta), floor(tau)));
  alpha = tau - itau;
  theta = Theta(itau);
  omega = Omega(itau) + alpha * theta;
endfunction

## Fetch an interpolated sample from signal.  If phi is outside the
## bounds of signal, return zero
function s = fetch(signal, phi)
  if ((phi < 1) || (phi > length(signal)))
    s = 0;
  elseif ((iphi = floor(phi)) == phi)
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
## if the result is more than 50% off from estimated_period.
##
## This could be improved: it uses the integer sample # as the peak.  But 
## the corellator output is a curve - consider using polyfit or splinefit
## as a way to find the true (interpolated) peak.
#
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

## Compute instantaneous phase (theta) and frequency (omega) of the
## waveform.  Called once at initialization.
function [Theta, Omega] = computeThetaOmega(Signal, estimated_period)
  Theta = zeros(length(Signal), 1);
  Omega = zeros(length(Signal), 1);
  
  theta = 0;
  for t=1:length(Signal)
    period = find_period(Signal, t, estimated_period);
    omega = 1/period;
    Omega(t) = omega;
    Theta(t) = theta = mod(theta + omega, 1.0);
  endfor
endfunction

## Local Variables:
## mode: octave
## End:
