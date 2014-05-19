#!/bin/sh
#
# Generate a series of sine tones ranging in pitch from C-1 (8.175 Hz)
# to G9 (12543.85395 Hz).  All stereo, all 44100 Hz sampling rate.
# File names are xxx.wav, where xxx is the MIDI pitch number for that
# frequency.
#
# Thank you, sox
#
for (( i = 0 ; i < 127 ; i++ ))
do
    echo sox -c 2 -r 44100 -n `printf "%03d.wav" $i` synth 1 sin %`expr $i - 69` gain -10
    sox -c 2 -r 44100 -n `printf "%03d.wav" $i` synth 1 sin %`expr $i - 69` gain -10
done
