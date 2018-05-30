from threading import Thread

import math
import serial


class HapticHandler(object):
    port = ''
    baudrate = 9600
    _active_serial_connection = None

    def __init__(self, port, baudrate=9600, timeout=1):
        """Initilize HapticHandler object for communicating with the sensor.
        Parameters
        ----------
        port : str
            Serial port name to which sensor is connected
        baudrate : int, optional
            Baudrate for serial connection (the default is 115200)
        timeout : float, optional
            Serial port connection timeout in seconds (the default is 1)
        """

        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.connect()

    def connect(self):
        """Connects to the serial port with the name `self.port`. If it was
        connected to another serial port disconnects from it first."""
        if self._active_serial_connection is not None:
            self.disconnect()
        self._active_serial_connection = serial.Serial(self.port, self.baudrate, timeout=self.timeout)

    def disconnect(self):
        """Disconnects from the serial port"""
        if self._active_serial_connection is None:
            return
        self._active_serial_connection.close()

    def activate(self, degrees, distance):

        Thread(target=vibrate, args=(self._active_serial_connection, degrees, distance)).start()  # create thread


def vibrate(serial: serial.Serial, degrees, distance):
    print('vibrate', degrees, distance)

    if serial is None:
        return

    serial.write("<{},{}>\n".format(round(degrees), round(distance)).encode('ascii'))


