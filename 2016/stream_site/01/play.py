import os
import picamera
from time import strftime

camera = picamera.PiCamera()

if not os.path.isdir("recordings"):
    os.makedirs("recordings")

name = strftime("%Y-%m-%d_%H:%M:%S" + ".h264")

camera.start_recording('recordings/' + name)
