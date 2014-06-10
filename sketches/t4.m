# The synthesis part.

pkg load signal;

global tau = 0;
global phi = 0;

function test(dtau, dphi)
  global tau;
  global phi;

  filename = "/Users/r/Projects/mu/SoundSets/tmp/69_resampled.wav";
  signal = wavread(filename)(:, 1);
  M = csvread("/Users/r/Projects/mu/SoundSets/tmp/69_resampled_phi_primes.csv");
  phi_nexts = M(:, 1);
  phi_prevs = M(:, 2);

  estimated_period = 1042;

  tau = 0;
  phi = 0;
  i = 1;
  S = zeros(length(signal), 1);
  while (tau < length(signal))
    S(i) = emit(signal, phi_prevs, phi_nexts, estimated_period);
    tau = tau + dtau;
    phi = phi + dphi;
    i += 1;
    if (mod(i, 10000) == 0)
      printf("tau = %f, phi = %f\n", tau, phi);
      fflush(stdout);
    endif
  endwhile

  wavwrite(S, 44100, 16, "/Users/r/Projects/mu/SoundSets/tmp/69_test.wav");

endfunction

# ================================================================================
# Emit one sample
function s = emit(signal, phi_prevs, phi_nexts, estimated_period)
  global tau;
  global phi;

  if (tau < phi)
    while (true)
      if (phi < 1)
        s = 0;
        return;
      endif
      phi_prime = phi + phi_prev(phi_prevs, phi, estimated_period);
      if (phi_prime <= tau) 
        break;
      endif
      phi = phi_prime;          # tau lags by > 1 period
      printf("-");
    endwhile
    s = intaupolate(signal, tau, phi, phi_prime);
    
  elseif (tau > phi)
    while (true)
      if (phi > length(signal))
        s = 0;
        return;
      endif
      phi_prime = phi + phi_next(phi_nexts, phi, estimated_period);
      if (phi_prime >= tau) 
        break;
      endif
      phi = phi_prime;          # tau lags by > 1 period
      printf("+");
    endwhile
    s = intaupolate(signal, tau, phi, phi_prime);
    
  else  # tau == phi
    s = fetch(signal, phi);
  endif
endfunction


 # ================================================================================
 # Return the index of a sample exactly one period prior to phi
 function phi_prime = phi_prev(phi_primes, phi, estimated_period)
   if ((phi < estimated_period) || (phi > length(phi_primes)))
     phi_prime = -estimated_period;
   else
     phi_prime = phi_primes(floor(phi));
   endif
 endfunction

 # ================================================================================
 # Return the index of a sample exactly one period following phi
 function phi_prime = phi_next(phi_primes, phi, estimated_period)
   if ((phi < 1) || (phi > length(phi_primes) - estimated_period))
    phi_prime = estimated_period;
  else
    phi_prime = phi_primes(floor(phi));
  endif
endfunction

# ================================================================================
# Blend a sample between signal(phi) and signal(phi_prime), depending on the
# relative value of tau.
function s = intaupolate(signal, tau, phi, phi_prime)
  s0 = fetch(signal, phi_prime);
  s1 = fetch(signal, phi);
  alpha = (tau - phi_prime) / (phi - phi_prime);
  s = s0 + alpha * (s1 - s0);
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



# Local Variables:
# mode: octave
# End:
