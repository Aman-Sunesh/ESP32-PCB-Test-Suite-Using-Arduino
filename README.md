# ESP32 PCB Test Suite

## Overview
The **ESP32 PCB Test Suite** is a versatile application designed to validate critical functionalities of an ESP32-based PCB. It can **monitor voltage**, **read temperature and humidity** via a DHT sensor, and **connect to Wi-Fi**, displaying real-time data over a **web interface**. This test suite helps in quickly diagnosing hardware issues such as **low voltage**, **out-of-range temperature/humidity**, and **Wi-Fi connectivity problems**.

You can **run** this project:
1. **On a physical ESP32 board** using the Arduino IDE and the appropriate libraries.
2. **Within a web-based simulator** like [Wokwi](https://wokwi.com/), which allows you to test the circuit logic without needing physical hardware.

A working simulator model is available here:  
[**Wokwi Simulation**](https://wokwi.com/projects/387077585204271105)

And the repository containing the full code and documentation can be found at:  
[**GitHub: Aman-Sunesh/ESP32-PCB-Test-Suite-Using-Arduino**](https://github.com/Aman-Sunesh/ESP32-PCB-Test-Suite-Using-Arduino/tree/main)

---

## Prerequisites

### Hardware (Physical Board)
- An **ESP32** or **ESP32-C3** development board
- A DHT sensor (e.g., **DHT22**) wired to the appropriate digital pin
- A **potentiometer** or voltage source for simulating **voltage changes** (if testing low voltage detection)
- **USB cable** for programming and power

### Software
1. **Arduino IDE** (version 1.8.13 or higher) or **PlatformIO**.
2. **ESP32 Board Package** installed via the Arduino Boards Manager:
   1. In **File > Preferences**, add this URL to **Additional Board Manager URLs**:  
      ```
      https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
      ```
   2. In **Tools > Board > Board Manager**, search for "**ESP32**" and install the package.
3. **DHT Sensor Library** (e.g., Adafruit DHT or a compatible library)
4. **ESPAsyncWebServer** library for hosting the real-time data web page
5. **WiFi** library (bundled with Arduino ESP32 support)
6. **(Optional)** [Wokwi](https://wokwi.com/) simulator account if you intend to run and test the code online without physical hardware.

---

## Installation & Setup

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/Aman-Sunesh/ESP32-PCB-Test-Suite-Using-Arduino.git
   cd ESP32-PCB-Test-Suite-Using-Arduino
   ```
2. **Open Arduino IDE** and load the .ino file (e.g., system_integration_test.ino).
3. **Select ESP32 Board**: In Tools > Board, choose ESP32 Dev Module or your specific ESP32 variant.
4. **Adjust Wi-Fi Credentials**: In the code, update ssid and password with your Wi-Fi network credentials.
5. **Compile and Upload**: Click Verify to compile and Upload to flash the code onto your board.
6. **Monitor Output:**: Open the Serial Monitor (baud rate 115200) to view diagnostic messages, including Wi-Fi status, sensor readings, and any alert messages.

## Usage

### Physical ESP32
1. **Power on** your board via USB or external supply.
2. **Open Serial Monitor** at `115200` baud.
3. The code automatically **scans for Wi-Fi networks** up to 10 times, attempting to connect to the specified `ssid`.
4. If successful, the board **hosts a web server**. The Serial Monitor prints an IP address.
5. **Visit the IP address** in a browser on the same network to see real-time sensor data (voltage, temperature, humidity) and any generated alerts.

### Wokwi Simulator
1. **Open** the project link on Wokwi.
2. Click **“Start Simulation”** to run the virtual ESP32.
3. The built-in **Serial Monitor** inside Wokwi displays the diagnostic messages as if it were real hardware.
4. You can **interact** with simulated components (like a DHT sensor or a potentiometer) to trigger alerts or change displayed values.

---

## Features

### Voltage Monitoring
- Reads analog input and checks for **low voltage** (< 3.0V).
- Triggers an alert message both on Serial and via the web interface, then puts the MCU into deep sleep if voltage is too low.

### DHT Sensor Readings
- Reads **temperature** in Celsius, Fahrenheit, Kelvin, and **humidity** in percent.
- Allows for **alert thresholds**:
  - Temperature below -10 °C or above 45 °C
  - Humidity above 75%
- Triggers an alert and deep sleep to protect the hardware.

### Wi-Fi Connectivity
- Scans networks up to 10 attempts.
- Connects to the target SSID.
- Prints network status, IP address, and RSSI.
- Fails gracefully if connection not possible.

### Web Server
- Displays real-time sensor data on a local IP page.
- Uses **Server-Sent Events (SSE)** for dynamic updates without page refresh.
- Alerts instantly displayed in the browser (e.g., “Low Voltage”, “High Temperature”, etc.).

### Deep Sleep on Error
- After printing alerts, the microcontroller enters a **deep sleep** state to save power and prevent further damage.

---

## Important Notes

- **Pin Assignments**: Double-check your wiring. The default code uses `voltageSensorPin = 0` and `DHTPin = 3` for sensors, and `LED = 9`. Adapt to your actual PCB layout as needed.
- **Simulator Limitations**: While Wokwi simulates logic and sensor behavior, actual hardware performance (timing, analog noise, Wi-Fi range) may differ in real life.
- **Data Accuracy**: The formula for converting ADC to voltage is set for a **12-bit ADC** with a certain reference voltage. Adjust it if your PCB design or microcontroller differs.

---

## Troubleshooting

### No Wi-Fi Connection
- **Issue**: After 10 attempts, the Serial Monitor prints “Failed to connect.”
- **Solution**: Check credentials, router signal, or if MAC filtering is enabled.

### DHT Sensor Shows “nan”
- **Issue**: Temperature/humidity reads `nan`.
- **Solution**: Ensure DHT sensor library is correct and the pin used matches the code. Verify 5V vs. 3.3V logic levels.

### Voltage Reading Always Zero or Too High
- **Issue**: The `voltageValue` does not match expected.
- **Solution**: Confirm your resistor divider or sensor wiring. Adjust the multiplication factor `(3.3 * 2)` for your design.

### Simulation Doesn’t Reflect Real Behavior
- **Issue**: Works on Wokwi but not physically, or vice versa.
- **Solution**: Ensure actual hardware wiring matches the simulator schematic. Some libraries behave slightly differently under simulation.

---

## Contributing
Contributions and improvements are welcome! Feel free to:
- **Fork this repo** and create pull requests
- **Add new sensors** or improved web interface designs
- **Optimize** memory use or performance

---

## Acknowledgments
- **Wokwi Online Simulator** for providing a convenient virtual environment.
- [**ESP32 Arduino Core Documentation**](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [**DHT Sensor Library**](https://github.com/adafruit/DHT-sensor-library)
- [**ESPAsyncWebServer Library**](https://github.com/me-no-dev/ESPAsyncWebServer)

**Enjoy testing your ESP32 PCB, whether physically or through the power of simulation!**
