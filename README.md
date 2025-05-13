# Line Follower Robot

This project implements a line-follower robot using an Arduino Uno (with HW-95/L298N motor driver) and a Raspberry Pi 4 (32-bit OS) with Picamera2 and OpenCV.

## Table of Contents

* [Features](#features)
* [Hardware Requirements](#hardware-requirements)
* [Connections & Setup](#connections--setup)

  * [HW-95 (L298N) Motor Driver](#hw-95-l298n-motor-driver)
  * [Removing ENA/ENB Jumpers](#removing-enaenb-jumpers)
* [Software Setup](#software-setup)

  * [Arduino Uno](#arduino-uno)
  * [Headless Raspberry Pi 4 OS (32-bit)](#headless-raspberry-pi-4-os-32-bit)
  * [Dependencies (Pi side)](#dependencies-pi-side)
* [Deployment](#deployment)

  * [Clone the Repository](#clone-the-repository)
  * [Install Arduino Sketch](#install-arduino-sketch)
  * [Pi Code & Service](#pi-code--service)
* [Usage & Debugging](#usage--debugging)
* [License](#license)

## Features

* Real-time black-line detection via Picamera2 + OpenCV
* Motion commands (`F`, `L`, `R`, `S`) sent over serial to Arduino
* Debug output on both Pi and Arduino (direction, pin states, PWM)
* Auto-start on boot using a systemd service

## Hardware Requirements

* Arduino Uno
* Raspberry Pi 4 (32-bit OS)
* HW-95/L298N motor driver module
* Two DC gear motors & power supply
* Picamera2-compatible camera module
* Pushbutton (for emergency KILL)
* Common ground wiring between Arduino, Pi, and driver

## Connections & Setup

### HW-95 (L298N) Motor Driver

| Motor A     | Arduino Pin |
| ----------- | ----------- |
| ENA         | D10 (PWM)   |
| IN1         | D9          |
| IN2         | D8          |
| Motor A +/− | OUT1/OUT2   |

| Motor B     | Arduino Pin |
| ----------- | ----------- |
| ENB         | D5 (PWM)    |
| IN3         | D7          |
| IN4         | D6          |
| Motor B +/− | OUT3/OUT4   |

* **VMot** → motor battery voltage (e.g. 6 V or 12 V)
* **5 V** → Arduino 5 V (only if VMot ≤ 12 V and 5 V jumper removed when VMot > 12 V)
* **GND** → common ground to Arduino & Pi

### Removing ENA/ENB Jumpers

By default, ENA/ENB are tied HIGH via jumper caps. Pull both small plastic caps off the two-pin headers labeled ENA and ENB so that your Arduino’s `analogWrite(..., PWM)` will actually drive the enable pins.

## Software Setup

### Arduino Uno

1. Open `line_follower.ino` in the Arduino IDE.
2. Verify the pin definitions match your wiring.
3. Upload to the Uno.
4. Open Serial Monitor at 9600 baud to view startup banner and debug.

### Headless Raspberry Pi 4 OS (32-bit)

1. **Download** Raspberry Pi OS Lite (32-bit) image.
2. **Flash** to your microSD card (e.g. using Raspberry Pi Imager).
3. On the `boot` partition:

   * Create an empty file named `ssh` to enable SSH.
   * Create `wpa_supplicant.conf` with your Wi-Fi credentials:

     ```ini
     country=US
     ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
     update_config=1

     network={
       ssid="YourSSID"
       psk="YourPassword"
     }
     ```
4. **Insert** the card, power the Pi, and SSH into `raspberrypi.local` or its IP.

### Dependencies (Pi side)

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install -y python3-pip libatlas-base-dev libjpeg-dev
pip3 install picamera2 opencv-python-headless pyserial numpy
```

## Deployment

### Clone the Repository

```bash
cd ~
git clone https://github.com/yourusername/line-follower-robot.git
cd line-follower-robot
```

### Install Arduino Sketch

Open the `arduino/` folder in the Arduino IDE, select your Uno board/port, and upload `line_follower.ino`.

### Pi Code & Service

1. Copy `pi/line_follow.py` to `/home/pi/line_follow.py`:

   ```bash
   cp pi/line_follow.py ~/
   chmod +x ~/line_follow.py
   ```
2. Create systemd service `/etc/systemd/system/linefollower.service`:

   ```ini
   [Unit]
   Description=Line Follower Service
   After=multi-user.target

   [Service]
   User=pi
   ExecStart=/usr/bin/python3 /home/pi/line_follow.py
   Restart=always

   [Install]
   WantedBy=multi-user.target
   ```
3. Enable and start:

   ```bash
   sudo systemctl daemon-reload
   sudo systemctl enable linefollower.service
   sudo systemctl start linefollower.service
   ```

## Usage & Debugging

* **Serial Monitor (Arduino)**: view `Arduino Line-Follower Ready` then `>> Dir:` logs.
* **Pi logs**:

  ```bash
  sudo journalctl -u linefollower.service -f
  ```
* **Manual run**:

  ```bash
  python3 ~/line_follow.py
  ```

## License

Released under the MIT License. See [LICENSE](LICENSE).
