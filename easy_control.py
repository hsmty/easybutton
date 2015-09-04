import atexit
import time
import signal
import sys

import Adafruit_BluefruitLE
from Adafruit_BluefruitLE.services import UART

ble = Adafruit_BluefruitLE.get_provider()

def main():
    ble.clear_cached_data()
    global device
    adapter = ble.get_default_adapter()
    adapter.power_on()
    print('Using adapter: {0}'.format(adapter.name))
    print('Searching for UART device...')
    try:
        adapter.start_scan()
        device = UART.find_device()
        if device is None:
            raise RuntimeError('Failed to find UART device!')
    finally:
        adapter.stop_scan()

    print('Connecting to device...')
    device.connect()

    print('Discovering services...')    
    UART.discover(device)
    uart = UART(device)

    line = ''
    timer = time.time()
    
    while True:
        received = uart.read(timeout_sec=1)
        if received is not None:
            timer = time.time()
            line = ''.join([line, received])
        else:
            if (time.time() - timer) > 1 and line != '':
                print('{0} {1}'.format('line is: ', line))
                line = ''


def stop_easy_control(signal = None, frame = None):
    global device
    device.disconnect()
    sys.exit(0)

atexit.register(stop_easy_control)
signal.signal(signal.SIGINT, stop_easy_control)

ble.initialize()
ble.run_mainloop_with(main)