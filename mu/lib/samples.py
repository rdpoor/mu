import numpy as np

class Samples(object):
    """
    Container for audio samples processed by Mu
    """

    DEFAULT_FRAME_COUNT = 1024
    DEFAULT_CHANNEL_COUNT = 2

    def __init__(self, channel_count=DEFAULT_CHANNEL_COUNT, frame_count=DEFAULT_FRAME_COUNT):
        self._data = np.zeros([channel_count, frame_count])

    def get_frame_count(self):
        """
        Get the frame count for the buffer.
        """
        return self._data.shape[1]

    def get_channel_count(self):
        """
        Get the channel count for the buffer.
        """
        return self._data.shape[0]

