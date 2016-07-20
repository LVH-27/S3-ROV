ffserver -f /etc/ffserver.conf & ffmpeg -r 15 -s 176x144 -f video4linux2 -i /dev/video0 http://localhost/webcam.ffm
