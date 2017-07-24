import serial
import numpy as np
import struct
import time
import glob
import sys

STOP_BYTE = np.uint8(0b01001100)

# example: #############################
#COMM_PORT = "/dev/ttyUSB3"
#SER_SPEED = 9600
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

    if len(result) == 0:
        print("No serial connection available!")
        quit(-1)
    elif len(result) > 1:
        while True:
            print("Several serial connections available!\nChoose one by writing the list index:")
            try:
                for i in range(len(result)):
                    print(i, result[i])
                port_index = int(input())
                COMM_PORT = result[port_index]
                break
            except IndexError as ie:
                print("Index out of range!\n")
            except ValueError as ve:
                print("Invalid input value!\n")

    else:
        COMM_PORT = result[0]
    return COMM_PORT




def send_rov_message(arduino: serial.Serial, motor1: np.int16,
                     motor2: np.int16, motor3: np.int16, command_flags: np.uint16):

    print("motor1:", motor1)
    print("motor2:", motor2)
    print("motor3:", motor3)
    print("command:", command_flags)

    bin_rep = ""
    bin_rep += np.binary_repr(motor1, 16)
    bin_rep += np.binary_repr(motor2, 16)
    bin_rep += np.binary_repr(motor3, 16)
    bin_rep += np.binary_repr(command_flags)

    #print(bin_rep)
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

#example: #############################
#COMM_PORT = "/dev/ttyUSB1"
#SER_SPEED = 9600
#SER_PARITY = serial.PARITY_NONE
#SER_XON_XOFF = False
#SER_RTSCTS = False
#with serial.Serial(baudrate=SER_SPEED, xonxoff=SER_XON_XOFF, rtscts=SER_RTSCTS, parity=SER_PARITY, port=COMM_PORT) \
#       as arduino:
#   send_rov_message(arduino,
#                    np.int16(123),
#                    np.int16(456),
#                    np.int16(-789),
#                    np.uint16(101112))
