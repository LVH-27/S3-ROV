import os
import picamera
from time import sleep, strftime

camera = picamera.PiCamera()
time_interval = 0

while time_interval <= 0:
	time_interval = float(raw_input("Specify the length of the recording in seconds: "))

if not os.path.isdir("recordings"):
    os.makedirs("recordings")

name = strftime("%Y-%m-%d_%H:%M:%S" + ".h264")

print "Recording..."
camera.start_recording('recordings/' + name)
sleep(time_interval)
camera.stop_recording()
print "Done"
print "Saved as " + name
