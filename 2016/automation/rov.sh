#!/bin/bash
sshpass -p "raspberry" ssh -o StrictHostKeyChecking=no pi@octopi 'bash -s' < second.sh
xdg-open ~/Documents/03_web_s/my_stream.html&
cd ~/S3-ROV/automation
echo Starting joystick_controller.py
python ~/S3-ROV/jscontrol/joystick_controller.py