# ESP32 Rover Control System

This project implements a control system for an ESP32-based rover that can be controlled via a web interface. The system utilizes ultrasonic sensors for obstacle detection and motor control to move the rover in different directions.

## Requirements

- ESP32 development board
- Arduino IDE with ESP32 board support

## Arduino IDE Setup

1. **Install Arduino IDE:**
   - Ensure you have the Arduino IDE installed on your computer. If not, download and install it from the official Arduino website: [Arduino IDE Download](https://www.arduino.cc/en/software)

2. **Install ESP32 Board Support:**
   - Open the Arduino IDE.
   - Go to "File" > "Preferences."
   - In the "Additional Boards Manager URLs" field, add the following URL:
     ```
     https://github.com/espressif/arduino-esp32/releases/download/2.0.0-beta1/package_esp32_index.json
     ```
   - Click "OK" to save the changes.

3. **Install ESP32 Board Package:**
   - Go to "Tools" > "Board" > "Boards Manager."
   - Search for "esp32" in the Boards Manager search bar.
   - Select "esp32" and click the "Install" button.
   - Wait for the installation to complete.

4. **Select ESP32 Board:**
   - Go to "Tools" > "Board" and select "ESP32 Dev Module" under the ESP32 section.

5. **Select Partition Scheme:**
   - Go to "Tools" > "Partition Scheme" and select "Default 4MB with SPIFFS."

## Setup

1. **Hardware Connections:**
   - Connect the ultrasonic sensor to the appropriate pins on the ESP32 board (e.g., `triggerPin` and `echoPin`).
   - Connect the DC motors and motor driver to the ESP32 board (e.g., `motor1Pin1`, `motor1Pin2`, `enable1Pin`, `motor2Pin1`, `motor2Pin2`, and `enable2Pin`).

2. **Upload HTML Interface to SPIFFS:**
   - Create a folder named "data" in the root of your Arduino sketch folder.
   - Place the "Interface.html" file inside the "data" folder.
   - In the Arduino IDE, select the ESP32 board and the correct "Partition Scheme."
   - Upload the sketch to the ESP32 board by clicking "Sketch" > "Upload."

3. **Upload the Sketch:**
   - Open the main sketch file (e.g., `rover_control_system.ino`) in the Arduino IDE.
   - Make sure the correct ESP32 board is selected under "Tools" > "Board."
   - Make sure the correct "Partition Scheme" is selected under "Tools" > "Partition Scheme."
   - Click "Sketch" > "Upload" to upload the sketch to the board.

4. **Connect to the Rover:**
   - Once the sketch is uploaded, the ESP32 will create a WiFi Access Point (AP) with the SSID "ROVER-ESP32".
   - Connect your device (e.g., smartphone, laptop) to this WiFi network.

5. **Control the Rover:**
   - Open a web browser on your connected device and enter `http://192.168.1.1` (or the IP address assigned to the AP) in the address bar.
   - The web interface will display control buttons for moving the rover forward, backward, left, right, and to stop.
   - The ultrasonic sensor will detect obstacles, and the status will be updated on the web interface dynamically.

