ffmpeg -r 15 -s 320x240 -f video4linux2 -i /dev/video0 -f mjpeg - | nc 192.168.10.15 1234
