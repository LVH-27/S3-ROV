import serial
import serial.tools
import serial.tools.list_ports
import struct

import numpy as np

COMM_PORT = '/dev/ttyUSB4'
print(COMM_PORT)
SER_SPEED = 9600
STOP_BYTE = np.uint8(0b01001100)
SER_PARITY = serial.PARITY_NONE
SER_XON_XOFF = False
SER_RTSCTS = False


arduino = serial.Serial(port=COMM_PORT, baudrate=SER_SPEED, xonxoff=SER_XON_XOFF, rtscts=SER_RTSCTS, parity=SER_PARITY)
if not arduino.is_open:
	arduino.open()

rec_string = ""
while True:
	print(str(arduino.read_until(b'\n').decode('ASCII')))
	

arduino.close()