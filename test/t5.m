## sxf: Synchronous Cross-Fade time scale modification algorithm.
##
## sxf is a time-scale modification algorithm that can independently
## modify the duration and pitch of a quasi-periodic audio waveform.
##
## SXF takes as inputs a waveform and two time-varying signals: Tau(t)
## and Phi(t). Tau(t) determines the "position" in the waveform and
## varies from 0 to n-1, where n is the number of samples in the
## waveform. Phi(t) determines the instantaneous phase expressed in
## samples, so the slope of Phi(t) determines the pitch relative to
## the original: a slope of 2.0 will double the pitch.
##
## At its core, sxf defines a function:
##     S(t) = f(Tau(t), Phi(t))
## that defines the sample generated at time t.  f(tau, phi) works as
## follows:
##
## tau defines a sample index within the original waveform.  sfx finds a
## sample with the desired phase phi on either side of tau.  In the
## ascii graphic below, t1 is the sample strictly greater than tau that
## has a phase of phi, t0 is the sample less than or equal to tau that
## has a phase of phi.
##
##     ^        ^                ^
##    t0       tau              t1
##
## To produce the desired sample, sfx interpolates between t0 and t1,
## with weights determined by the relative position of t:
##
##    s = S(t0) + (S(t1) - S(t0)) * (t - t0) / (t1 - t0)
##
## Finding t0 and t1
##
## At each sample of the original waveform, we compute the period of the
## waveform, measured in samples and save that in P, an array that
## parallels the waveform: P(t) is the period measured at sample S(t).
## We use cross- correlation to find where the waveform is maximally
## similar to itself when shifted by +/- 1 period.

pkg load signal;

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
function p = find_period(signal, tau, estimated_period)
  # window = bartlett(2 * estimated_period - 1);
  s0 = extract_subsignal(signal, tau - estimated_period, estimated_period);
  s1 = extract_subsignal(signal, tau + estimated_period, estimated_period);
  [R, lag] = xcorr(s0, s1);
  # plot(lag, R);
  [i, ix] = max(R);
  p = estimated_period - lag(ix)/2;

  ## "fix" period if it is out of bounds
  if ((p < estimated_period * 0.80) || (p > estimated_period * 1.2))
    p = estimated_period;
  endif
  ## printf("find_period(tau=%f) => %f\n", tau, p);
endfunction

## As written, this function is sensitive to changes in period length:
## if the period is 104.4 for long stretch, and then the period changes
## to 103.9, t1 and t0 "jitter" to another part of the waveform and
## cause discontinuities.
## 
## t            period      t0           tau          t1           alpha     s(t0)     s(t1)     s
## 6302.000000, 104.400000, 1509.100000, 1575.500000, 1613.500000, 0.636015, 0.552945, 0.563293, 0.559527
## 6303.000000, 104.400000, 1510.350000, 1575.750000, 1614.750000, 0.626437, 0.561900, 0.566689, 0.564900
## 6304.000000, 103.900000, 1542.100000, 1576.000000, 1646.000000, 0.326275, -0.455804, -0.381653, -0.431611
## 6305.000000, 104.400000, 1512.850000, 1576.250000, 1617.250000, 0.607280, 0.514798, 0.522980, 0.519767
## 6306.000000, 104.400000, 1514.100000, 1576.500000, 1618.500000, 0.597701, 0.459796, 0.457809, 0.458609
## 6307.000000, 104.400000, 1515.350000, 1576.750000, 1619.750000, 0.588123, 0.393597, 0.380989, 0.386182
## 6308.000000, 103.900000, 1547.100000, 1577.000000, 1651.000000, 0.287777, -0.561005, -0.475342, -0.536353
## 6309.000000, 104.400000, 1517.850000, 1577.250000, 1622.250000, 0.568966, 0.268332, 0.275002, 0.272127
## 6310.000000, 104.400000, 1519.100000, 1577.500000, 1623.500000, 0.559387, 0.224274, 0.251633, 0.239578

function s = generate_sample(signal, tau, phi, estimated_period, t)
  update_view = mod(t, 1000) == 0;
  period = 104.4;  # find_period(signal, tau, estimated_period);
  t1 = tau + mod(phi, period);
  t0 = t1 - period;
  alpha = (tau - t0) / (t1 - t0);
  s1 = fetch(signal, t1);
  s0 = fetch(signal, t0);
  s = s0 + (s1 - s0) * alpha;
  # fprintf(stderr,"%f, %f, %f, %f, %f, %f, %f, %f, %f\n", t, period, t0, tau, t1, alpha, s0, s1, s);
  if (update_view)
    printf("t=%f\n", t); fflush(stdout);
    snip = extract_subsignal(signal, tau, estimated_period+2);
    center = floor(length(snip)/2);
    miny = min(snip);
    maxy = max(snip);
    x0 = floor(tau-t0+center);
    x = floor(tau-tau+center);
    x1 = floor(tau-t1+center);
    h = plot(1:length(snip), snip,
             [x0;x0],[miny;maxy],
             [x;x], [miny;maxy],
             [x1;x1],[miny;maxy]);
    sleep(0.1);
  endif

endfunction

# ================================================================================
# Fetch an interpolated sample from signal.  If phi is outside the bounds of 
# signal, return zero
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

function test_t5(dtau, dphi, estimated_period, filename)
  S = load_signal();
  P = zeros(length(S)/dtau, 1);
  for t=1:length(P)
    P(t) = generate_sample(S, t*dtau, t*dphi, estimated_period, t);
  endfor
  wavwrite(P, 44100, 16, filename);
endfunction

## Local Variables:
## mode: octave
## End:
