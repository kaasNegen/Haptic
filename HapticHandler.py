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

        self.port = serial_ports()[2]
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
        if(self._active_serial_connection.readline() == "A\\r\\n"):
            self._active_serial_connection.write("<{},{}>".format(degrees, distance))
        else:
            pass

def serial_ports():
    """ Lists serial port names
        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result
