avconv -f video4linux2 -i /dev/video0 -r 24 -s 176x144 -c:v:1 copy -f mjpeg - | nc -l 10.1.2.145 1234

# /dev/video0 je input fajl - feed s uređaja
# framerate 15, reza 176x144
# codec:v:1: uzmi prvi ili drugi (nisam siguran) video stream i samo ga kopiraj, bez konverzije
#http://askubuntu.com/questions/365163/how-to-minimize-usage-of-cpu-memory-by-ffmpeg-when-recording-video
