cvlc v4l2:///dev/video0 :v4l2-width=480 :v4l2-height=360 :v4l2-fps=15 :caching=0 --sout '#transcode{vcodec=mp4v,vb=2000,venc=ffmpeg{keyint=10,hurry-up,vt=500}}:rtp{dst=224.0.0.100,port=5000,sdp=rtsp://10.1.2.135:8000/stream.sdp}'

