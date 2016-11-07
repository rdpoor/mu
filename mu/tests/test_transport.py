import unittest
import transport
from test import test_support
from unittest.mock import patch

class TestTransport(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_defaults(self):
        t = transport.Transport()
        self.assertEqual(t.get_tick(), 0)
        self.assertEqual(t.get_frame_count(), t.DEFAULT_FRAME_COUNT)
        self.assertEqual(t.get_channel_count(), t.DEFAULT_CHANNEL_COUNT)

    def test_thread_started(self):

def test_main():
    test_support.run_unittest(TestTransport)

if __name__ == '__main__':
    test_main()
