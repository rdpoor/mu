# TODO: switch from threading to mutliprocessing, then add
# `immediately` option to Transport#stop()

import threading
import samples
import time

class Transport(object):
    """
    Transport is a generic class to control run / stop / rewind functions.

    The Transport class creates a separate thread in which all of the rendering
    takes place.  When running, it calls render() on whatever stream is set as
    the transport's source.
    """

    DEFAULT_FRAME_COUNT = 1024
    DEFAULT_CHANNEL_COUNT = 2

    def __init__(self):
        print("=== init() a")
        self._source = None
        self._tick = 0
        self._stoppable = threading.Event()
        self._synchronization = threading.Lock()
        self._samples = None
        self._frame_count = self.DEFAULT_FRAME_COUNT
        self._channel_count = self.DEFAULT_CHANNEL_COUNT

    def source(self):
        return self._source

    def set_source(self, source):
        self._source = source
        return self

    def start(self):
        # Tell the processing thread to start rendering sampless.
        self._rendering_thread = threading.Thread(target=self.run)
        self._rendering_thread.start()
        return self

    def stop(self):
        # Tell the processing thread to stop rendering sampless.  Any
        # partially rendered samples will complete.
        self._rendering_thread = None
        return self

    def stop_and_wait(self):
        # Tell the processing thread to stop rendering sampless, then
        # block until processing finishes.
        self.stop()
        self._stoppable.wait()
        return self

    def get_tick(self):
        """
        Return the tick (frame index) about to be or currently being processed.
        """
        return self._tick

    def set_tick(self, tick):
        with self._synchronization:
            self._tick = tick
        return self

    def rewind(self):
        return self.set_tick(0)

    def jog(self, ticks):
        with self._synchronization:
            self._tick += ticks
        return self

    def get_frame_count(self):
        return self._frame_count

    def set_frame_count(self, frame_count):
        if frame_count != self._frame_count:
            self._frame_count = frame_count
            self._samples = None

    def get_channel_count(self):
        return self._channel_count

    def set_channel_count(self, channel_count):
        if channel_count != self._channel_count:
            self._channel_count = channel_count
            self._samples = None

    def get_samples(self):
        if self._samples == None:
            self._samples = samples.Samples(frame_count=self.get_frame_count(), channel_count=self.get_channel_count())
        return self._samples

    # This is the processing thread
    def run(self):
        my_thread = threading.current_thread()
        self._stoppable.clear()
        try:
            while (self._rendering_thread == my_thread):
                self.render()
        finally:
                self._stoppable.set()

    # Called from processing thread.
    def render(self):
        if self.source() != None:
            self.source().render(self.get_samples(), self.get_tick())
            self.jog(self._samples.get_frame_count())


if __name__ == '__main__':
    import time

    class Bingbong(object):
        def render(self, samples, tick):
            print("bing " + str(tick))
            time.sleep(0.5)

    t = Transport()
    t.set_source(Bingbong())
    t.start()
    time.sleep(2.6)
    t.stop_and_wait()
    # t.stop()
