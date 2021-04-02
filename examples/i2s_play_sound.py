import time
import wave
from machine import I2S


# Tested on PCM5102 MODULE (purple with two 3.3V LDO) :
# * 5V <-> VIN if LDO in your module
# * GND <-> GND
# * LRCK <-> GP11
# * DIN <-> GP12
# * BCK <-> GP10

i2s = I2S()


def play_wav(filename):
    sound = wave.open(filename, 'rb')
    # Note: Data seems output like this for signed 16bit stereo:
    # - canal 0 : LSB(0-7) MSB (8-15)
    # - canal 1 : LSB(0-7) MSB (8-15)
    # - canal 0 ...

    if sound.getnchannels() != 2:
        raise ValueError('Not a stereo wav')
    elif sound.getsampwidth() != 2:
        raise ValueError('Not 16bit')

    i2s.set_freq(sound.getframerate())
    sample_count = int(4000/(sound.getnchannels() * sound.getsampwidth()))
    samples = sound.readframes(sample_count)
    count = 0
    delayed = 0

    while True:
        try:
            i2s.write(samples)
        except RuntimeError as e:
            if 'no buffer avail' in str(e):
                delayed += 1
                continue
            else:
                raise e
        count += 1
        samples = sound.readframes(sample_count)
        if len(samples) == 0:
            break
        #time.sleep(0.001)

    print(delayed, count)


play_wav('./external/ressources/sound/joefpres_dark_loops_cc0_20s.wav')
print('finished')
# Copy a wav file (44.1 or 48khz, 16 bits, stereo) on this destination (external/ressources/sound/music.wav).
# If not a wav, you can use Audaicty to convert it.
#play_wav('./external/ressources/sound/music.wav')
