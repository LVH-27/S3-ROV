import serial
import numpy as np
import struct


COMM_PORT = "/dev/ttyUSB0"
SER_SPEED = 9600
STOP_BYTE = np.uint8(0b01001100)
SER_PARITY = serial.PARITY_NONE
SER_XON_XOFF = False
SER_RTSCTS = False

########################################

arduino = serial.Serial(baudrate=SER_SPEED, xonxoff=SER_XON_XOFF, rtscts=SER_RTSCTS, parity=SER_PARITY)
arduino.open()


def calculate_checksum(*args):
    pass


def send_rov_message(arduino: serial.Serial, motor1: np.int16, motor2: np.int16, motor3: np.int16, command_flags: np.uint16):

    check_sum = 0b0

    for i in range(16):
        check_sum = check_sum ^ (0b1 & (motor1 < i))
    for i in range(16):
        check_sum = check_sum ^ (0b1 & (motor2 < i))
    for i in range(16):
        check_sum = check_sum ^ (0b1 & (motor3 < i))
    for i in range(16):
        check_sum = check_sum ^ (0b1 & (command_flags < i))

    check_sum = np.uint16

    assert check_sum not in (0b0, 0b1)

    arduino.write(struct.pack('>B', motor1)) # , motor2, motor3, command_flags, check_sum, STOP_BYTE))
    arduino.write(struct.pack('>B', motor2))
    arduino.write(struct.pack('>B', motor3))
    arduino.write(struct.pack('>B', command_flags))
    arduino.write(struct.pack('>B', check_sum))
    arduino.write(struct.pack('>B', STOP_BYTE))
    return 0

send_rov_message(arduino, np.int16(10), np.int16(11), np.int16(12), np.uint16(13))
