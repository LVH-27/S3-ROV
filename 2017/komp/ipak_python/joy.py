import pygame, struct, serial, time, sys, os, glob
from pygame.locals import *
import numpy as np, math

from ROVserial import *


MOTOR_MAP_CONST = 2**15-1
WAIT_TIME = 0.01 # wait period before sending each package (in seconds)

COMM_PORT = serial_ports()
SER_SPEED = 115200
STOP_BYTE = np.uint8(0b01001100)
SER_PARITY = serial.PARITY_NONE
SER_XON_XOFF = False
SER_RTSCTS = False

###########################################


class ROVmsg:
	'ROV message class, sent to Arduino'
	def __init__(self, serialCon):
		self.thr_left = np.int16(0)
		self.thr_right = np.int16(0)
		self.thr_center = np.int16(0)
		self.cmd_mask = np.uint16(0)
		self.stop = np.uint8(0b01001100) # stop-byte, unlikely to occur in previous bytes
		self.checksum = None
		self.serialCon = serialCon

	def send(self):
		send_rov_message(self.serialCon, self.thr_left, self.thr_right, self.thr_center, self.cmd_mask)

	def __repr__(self):
		return "MSG! L: {} R: {} C: {}".format(self.thr_left, self.thr_right, self.thr_center)



def handle_input(axes, mask, msg):
	"""The angle and the radius calculated below adhere to the 
	left analog stick as represented by the image below
	     --- PI/2 ---
	    /            \ r=1
	   /              \
	+-PI      o r=0    0
	   \              /
	    \            /
	     -- -PI/2 ---"""

	angle = math.atan2(-axes[1], axes[0])
	r = math.hypot(axes[0], -axes[1])
	if r > 1:
		r = 1


	#print("r:", r)
	#print("angle:", angle)
	if r < 0.05: # dead zone
		msg.thr_left = np.int16(MOTOR_MAP_CONST*0)
		msg.thr_right = np.int16(MOTOR_MAP_CONST*0)
	else:
		# MOVING FORWARD
		if (0 <= angle and angle < math.pi/2.) :
			# axis0 > 0
			msg.thr_left = np.int16(MOTOR_MAP_CONST*r*(1+axes[0])/2.) # map [-1.0, 1.0] to [-MOTOR_MAP_CONST, MOTOR_MAP_CONST] range, for actual motor use
			print("msg.tl:", msg.thr_left)
			msg.thr_right = np.int16(MOTOR_MAP_CONST*r*(1-axes[0])/2.)
		
		elif ((math.pi/2. <= angle and angle < math.pi) or angle == (-math.pi)):
			# axis0 < 0
			msg.thr_right = np.int16(MOTOR_MAP_CONST*r*(1-axes[0])/2.)
			msg.thrleft = np.int16(MOTOR_MAP_CONST*r*(1+axes[0])/2.)
		
		# MOVING BACKWARD
		# Intervals <-PI_8, 0> and <-PI, -7/8 PI> are taken as dead zones.
		# Therefore, the axis0 is normalized to cos(M_PI_8) so as to preserve the throttle range.
		elif (-math.pi/2. <= angle and angle < -math.pi/8.):
			axes[0] /= math.cos(math.pi/8.)
			# axis0 > 0
			msg.thr_right = np.int16(MOTOR_MAP_CONST*-r*(1+axes[0])/2.)
			msg.thr_left = np.int16(MOTOR_MAP_CONST*-r*(1-axes[0])/2.)
		elif (-7*math.pi/8. < angle and angle < -math.pi/2.):
			axes[0] /= math.cos(math.pi/8.);
			# axis0 < 0
			msg.thr_left = np.int16(MOTOR_MAP_CONST*-r*(1-axes[0])/2.)
			msg.thr_right = np.int16(MOTOR_MAP_CONST*-r*(1+axes[0])/2.)
	
	# CENTER MOTOR
	if abs(axes[2]) < 0.05: # dead zone
			msg.thr_center = np.int16(MOTOR_MAP_CONST*0);
	else:
			msg.thr_center = np.int16(MOTOR_MAP_CONST*axes[2]);
	
	msg.cmd_mask = np.uint16(mask)


pygame.init()
pygame.joystick.init()
stick = pygame.joystick.Joystick(0)
stick.init()

pygame.key.set_repeat(1, 1);
pygame.display.set_mode((20, 10))
pygame.display.set_caption('RovController')

print('{0} axes, {1} buttons'.format(stick.get_numaxes(), stick.get_numbuttons()))

with serial.Serial(baudrate=SER_SPEED, xonxoff=SER_XON_XOFF, rtscts=SER_RTSCTS, parity=SER_PARITY, port=COMM_PORT) \
	as serialCon:

	while True:
		pygame.event.pump()
		kb_keys = pygame.key.get_pressed()
		mask = 0
		for i in range(stick.get_numbuttons()):
			mask|=stick.get_button(i)<<i
		axes = [float(stick.get_axis(i)) for i in range(stick.get_numaxes())]
		###axes = [axes[0], axes[1], axes[3], axes[2]] # hack - osi su bile tim redoslijedom prosle godine i ne da mi se razmisljat zasto - radilo je!

		#packed = struct.pack('<ffffI', axes[0], axes[1], axes[3], axes[2], mask)
		#print([float(stick.get_axis(i)) for i in range(stick.get_numaxes())])

		####### pripazi na axes[0132]!

		msg = ROVmsg(serialCon)
		handle_input(axes, mask, msg)
		print(msg)
		msg.send()

		print([round(axis, 3) for axis in axes],end="    |    ")
		print(mask)

		if mask&(1<<8):
			pygame.quit()
			sys.exit(0)

		#time.sleep(1)
		#time.sleep(WAIT_TIME)


		#if mask&1<<3 != 0:
			#time.sleep(0.1)
