import serial
import numpy as np
import struct
import time

# example: #############################
#COMM_PORT = "/dev/ttyUSB3"
#SER_SPEED = 9600
#STOP_BYTE = np.uint8(0b01001100)
#SER_PARITY = serial.PARITY_NONE
#SER_XON_XOFF = False
#SER_RTSCTS = False
#with serial.Serial(baudrate=SER_SPEED, xonxoff=SER_XON_XOFF, rtscts=SER_RTSCTS, parity=SER_PARITY, port=COMM_PORT) \
#        as arduino:
#    send_rov_message(arduino,
#                     np.int16(123),
#                     np.int16(456),
#                     np.int16(-789),
#                     np.uint16(101112)

########################################


def send_rov_message(arduino: serial.Serial, motor1: np.int16,
                     motor2: np.int16, motor3: np.int16, command_flags: np.uint16):

    bin_rep = ""
    bin_rep += np.binary_repr(motor1, 16)
    bin_rep += np.binary_repr(motor2, 16)
    bin_rep += np.binary_repr(motor3, 16)
    bin_rep += np.binary_repr(command_flags)

    print(bin_rep)
    check_sum = np.uint8(bin_rep.count('1'))

    print("chksm:", check_sum)

    assert type(check_sum) is np.uint8
    assert type(motor1) is np.int16 and motor1 in np.array(range(-2**15, 2**15, 1), dtype=np.int16)
    assert type(motor2) is np.int16 and motor2 in np.array(range(-2**15, 2**15, 1), dtype=np.int16)
    assert type(motor3) is np.int16 and motor3 in np.array(range(-2**15, 2**15, 1), dtype=np.int16)
    assert type(command_flags) is np.uint16 and command_flags in np.array(range(0, 2**16), dtype=np.uint16)
    assert type(arduino) is serial.Serial and arduino.is_open

    arduino.write(struct.pack('>h', motor1))                # 16bit: 2B
    arduino.write(struct.pack('>h', motor2))                # 16bit: 2B
    arduino.write(struct.pack('>h', motor3))                # 16bit: 2B
    arduino.write(struct.pack('>H', command_flags))         # 16bit: 2B
    arduino.write(struct.pack('>B', check_sum))             # 8bit: 1B
    arduino.write(struct.pack('>B', STOP_BYTE))             # 8bit: 1B

    return True



                     )
