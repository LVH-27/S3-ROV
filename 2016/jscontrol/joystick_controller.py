import pygame, struct, socket, time, sys, os
from pygame.locals import *

networking = True
if networking:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('192.168.10.10', 1234))

pygame.init()
pygame.joystick.init()
stick = pygame.joystick.Joystick(0)
stick.init()
pygame.key.set_repeat(1, 1);
pygame.display.set_mode((20, 10))
pygame.display.set_caption('RovController')

print('{0} axes, {1} buttons'.format(stick.get_numaxes(), stick.get_numbuttons()))

while True:
    pygame.event.pump()
    kb_keys = pygame.key.get_pressed()
    mask = 0
    for i in range(stick.get_numbuttons()):
        mask|=stick.get_button(i)<<i
    axes = [round(float(stick.get_axis(i)), 2) for i in range(stick.get_numaxes())]

    if kb_keys[27]:
        mask|=1<<8
    if kb_keys[K_o]:
        axes[2] = -1
    if kb_keys[K_l]:
        axes[2] = 1
    if kb_keys[K_w]:
        axes[1] = -1
    if kb_keys[K_s]:
        axes[1] = 1
    if kb_keys[K_a]:
        axes[0] = -1
    if kb_keys[K_d]:
        axes[0] = 1
    if kb_keys[K_q]:
        mask|=1<<6
    if kb_keys[K_e]:
        mask|=1<<7
    if kb_keys[K_u]:
        mask|=1<<3
    
    
    packed = struct.pack('<ffffI', axes[0], axes[1], axes[3], axes[2], mask)
    #print(axes)
    if networking:
        sock.sendall(packed)
    if mask&(1<<8):
        pygame.quit()
        sys.exit(0)
    time.sleep(0.05)
    
    
    if mask&1<<3 != 0:
		time.sleep(0.1)
