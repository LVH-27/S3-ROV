cvlc v4l2:///dev/video0 --sout '#transcode{vcodec=mp4v,vb=5000,venc=ffmpeg{keyint=10,hurry-up,vt=2000}}:rtp{dst=224.0.0.100,port=5000,sdp=rtsp://192.168.11.106:8000/stream.sdp}'
