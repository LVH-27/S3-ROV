#!/bin/bash
sshpass -p "raspberry" ssh -o StrictHostKeyChecking=no pi@octopi 'bash -s' < second.sh
cd ~/automation
python ~/S3-ROV/jscontrol/joystick_controller.py
xdg-open ~/Documents/03_web_s/my_stream.html
