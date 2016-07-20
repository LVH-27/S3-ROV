ffmpeg -r 15 -s 176x144 -f video4linux2 -i /dev/video0 -f mjpeg - | nc 10.1.2.178 1234
