# SYNOPSIS:
#   ./resample_sources.rb [csv_file = "source_periods.csv"]
#
# Genereate a script that, when executed, will resample each sound
# file mentioned in source_periods.csv to create pitches from A0 (midi
# 21) to A5 (midi 81).  
#
# Each sound file is reduced to single channel (monophonic), normalized
# and trimmed prior to resampling.
#
# The generated file names are 0nn-{p|m}-cccc.wav, where nn is the midi
# note number, p or m sigifies plus or minus, and cccc is the number
# of cents adjustement needed to create that pitch.

require 'csv'

DEFAULT_CSV_FILE_NAME = "/Users/r/Projects/Mu/utilities/source_periods.csv"
DEST_DIRECTORY = "/Users/r/Projects/Mu/SoundSets/B"
SAMPLING_RATE = 44100
TEMP_WAV_FILE = "/tmp/tmp.wav"
SOX = "/sw/bin/sox"

def process_filespecs(midi_pitches, csv_file_name = DEFAULT_CSV_FILE_NAME)
  csv = CSV.read(csv_file_name, :headers => true, :header_converters => :symbol)
  csv.each {|filespec| process_filespec(midi_pitches, filespec)}
end

def process_filespec(midi_pitches, filespec)
  midi_pitches.each {|midi_pitch| process_pitch(midi_pitch, filespec) }
end

def process_pitch(midi_pitch, filespec)
  filename = filespec[0]
  measured_period = filespec[1].to_f
  delta_cents = compute_delta_cents(midi_pitch, measured_period)
  emit_script_command(midi_pitch, filename, delta_cents)
end

def compute_delta_cents(midi_pitch, measured_period)
  measured_frequency = period_to_frequency(measured_period)
  measured_pitch = frequency_to_midi_pitch(measured_frequency)
  ((midi_pitch - measured_pitch) * 100).round.to_i
end

def period_to_frequency(period)
  SAMPLING_RATE / period
end

def frequency_to_midi_pitch(frequency)
  69.0 + 12.0 * Math.log2(frequency/440.0)
end

def emit_script_command(midi_pitch, src_filename, delta_cents)
  dst_filename = make_dst_file_name(midi_pitch, delta_cents)
  if (delta_cents <= 0)
    # shifting down: just use `speed` command
    # convert to mono, shift pitch, normalized to -3dB
    printf("%s %s %s -S remix - speed %04dc gain -n -3\n", SOX, src_filename, dst_filename, delta_cents)
  else
    # shifting up in pitch: use 'pitch' shifting algo
    printf("%s %s %s -S remix - pitch %04d gain -n -3\n", SOX, src_filename, dst_filename, delta_cents)
  end
end
 
# 0nn-{p|m}-cccc.wav
def make_dst_file_name(midi_pitch, delta_cents)
  sign = (delta_cents >= 0) ? "p" : "n"
  sprintf("%s/%03d-%s-%04d.wav", DEST_DIRECTORY, midi_pitch, sign, delta_cents.abs)
end

def midi_pitches
  (21..81).to_a
end
