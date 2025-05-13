#!/usr/bin/env python3
# line_follow.py
# Raspberry Pi 4 + Picamera2 + OpenCV
# Auto-detects Arduino, captures bottom 40% of frame,
# decides F/L/R/S and sends over Serial, prints Arduino debug.

import cv2
import numpy as np
import time
import serial
import glob
import sys
from picamera2 import Picamera2

def find_arduino_port():
    ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')
    if not ports:
        print("ERROR: No Arduino port found! Check connection.")
        sys.exit(1)
    return ports[0]

def detect_and_drive():
    # Open serial to Arduino
    port = find_arduino_port()
    arduino = serial.Serial(port, 9600, timeout=0.5)
    time.sleep(2)  # wait for Arduino reset
    print(f"[Pi] Connected to Arduino on {port}")

    # Initialize Picamera2
    picam2 = Picamera2()
    cfg = picam2.create_preview_configuration(main={"size": (640, 480)})
    picam2.configure(cfg)
    picam2.start()
    time.sleep(2)
    print("[Pi] Line follower started.")

    try:
        while True:
            frame = picam2.capture_array()
            gray  = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            blur  = cv2.GaussianBlur(gray, (5,5), 0)
            _, thresh = cv2.threshold(blur, 60, 255, cv2.THRESH_BINARY_INV)

            h, w = thresh.shape
            roi   = thresh[int(h*0.6):, :]
            left  = roi[:, :w//2]
            right = roi[:, w//2:]

            left_sum  = np.sum(left)
            right_sum = np.sum(right)

            # Decide command
            if   left_sum >  100 and right_sum >  100: cmd = 'F'
            elif left_sum >  100:                      cmd = 'L'
            elif right_sum > 100:                      cmd = 'R'
            else:                                      cmd = 'S'

            # Send and print
            arduino.write(cmd.encode())
            print(f"[Pi] Sent: {cmd}   L={left_sum:.0f}   R={right_sum:.0f}")

            # Read Arduino debug lines
            while True:
                line = arduino.readline().decode(errors='ignore').strip()
                if not line:
                    break
                print(f"[Arduino] {line}")

            time.sleep(0.05)

    except KeyboardInterrupt:
        print("\n[Pi] Interrupted by user.")
    finally:
        arduino.write(b'S')
        picam2.stop()
        arduino.close()
        print("[Pi] Shutdown complete.")

if __name__ == "__main__":
    detect_and_drive()
