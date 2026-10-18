# 🌱 Smart Soil Fertility Monitoring System

An ESP32-based IoT system for monitoring soil nutrients and environmental conditions using NPK, soil moisture, temperature, and humidity sensors. The system integrates RS485/Modbus communication and Blynk IoT for remote monitoring and data-driven agricultural decisions.

![Platform](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif)
![Language](https://img.shields.io/badge/Language-Embedded%20C%2FC%2B%2B-orange?logo=cplusplus)
![Communication](https://img.shields.io/badge/Communication-RS485%20%7C%20Modbus-green)
![IoT](https://img.shields.io/badge/IoT-Blynk-23C48E)
![Status](https://img.shields.io/badge/Status-Working%20Prototype-success)

## 📌 Table of Contents

- [Overview](#-overview)
- [Problem Statement](#-problem-statement)
- [Objectives](#-objectives)
- [Features](#-features)
- [System Architecture](#-system-architecture)
- [Hardware Components](#-hardware-components)
- [Software and Technologies](#-software-and-technologies)
- [Hardware Connections](#-hardware-connections)
- [Working Principle](#-working-principle)
- [LCD Display](#-lcd-display)
- [Blynk IoT Integration](#-blynk-iot-integration)
- [Soil Health Score](#-soil-health-score)
- [Installation and Setup](#-installation-and-setup)
- [Testing and Results](#-testing-and-results)
- [Applications](#-applications)
- [Limitations](#-limitations)
- [Future Scope](#-future-scope)
- [Author](#-author)

## 🔍 Overview

Soil health plays an important role in agricultural productivity. Monitoring soil nutrients and moisture can help farmers better understand field conditions and make informed decisions about irrigation and soil management.

This project uses an ESP32 microcontroller to collect readings from an RS485-based NPK sensor, a capacitive soil moisture sensor, and a DHT11 temperature and humidity sensor.

The collected measurements are processed by the ESP32, displayed on a 16×2 I2C LCD, and transmitted to the Blynk IoT platform when a Wi-Fi connection and Blynk connection are available.

## ❗ Problem Statement

Traditional soil monitoring often relies on manual inspection or laboratory testing, which may not provide continuous information about changing field conditions.

An IoT-based monitoring system can provide convenient access to sensor readings and help users observe soil conditions more frequently.

## 🎯 Objectives

- Monitor soil nitrogen (N), phosphorus (P), and potassium (K) readings.
- Measure soil moisture levels.
- Monitor ambient temperature and relative humidity.
- Implement RS485 communication using Modbus RTU.
- Integrate the ESP32 with Blynk IoT for remote monitoring.
- Display sensor readings and a calculated soil health score on an LCD.
- Explore the use of sensor data to support agricultural decision-making.

## ✨ Features

- **NPK monitoring:** Reads nitrogen, phosphorus, and potassium registers from an RS485-based soil sensor.
- **Soil moisture monitoring:** Converts the analog sensor reading into a calibrated percentage.
- **Temperature and humidity monitoring:** Uses the DHT11 sensor.
- **Modbus communication:** Communicates with the NPK sensor using a UART interface and CRC-16.
- **LCD interface:** Cycles through environmental readings, NPK readings, and a calculated health score.
- **Cloud monitoring:** Sends available measurements to Blynk virtual pins V0–V6.
- **Connection-aware updates:** Sends Blynk data only when Wi-Fi and Blynk are connected.
- **Periodic operation:** Uses BlynkTimer to schedule sensor updates.

## 🏗️ System Architecture

```text
  NPK Soil Sensor
        |
     RS485 A/B
        |
     MAX485
        |
        | UART2
        v
  +----------------------+
  |     ESP32 Board      |
  |                      |
  | Sensor Data Reading  |
  | Modbus Processing    |
  | Moisture Calibration |
  | Health Score         |
  +----------------------+
       |             |
       | I2C         | Wi-Fi
       v             v
   16x2 LCD       Blynk IoT
                      |
                      v
                Remote Dashboard

  Capacitive Moisture Sensor ---> ESP32 ADC
  DHT11 Sensor -----------------> ESP32 GPIO
```

## 🔩 Hardware Components

| Component | Purpose |
|---|---|
| ESP32 development board | Main controller and Wi-Fi connectivity |
| RS485 NPK soil sensor | Measures NPK-related soil parameters |
| MAX485 transceiver | Interfaces the ESP32 UART with RS485 |
| Capacitive soil moisture sensor | Measures soil moisture through an analog output |
| DHT11 sensor | Measures temperature and relative humidity |
| 16×2 LCD with I2C interface | Displays readings and health score |
| Power supply and jumper wires | Power and interconnections |

## 💻 Software and Technologies

- Embedded C/C++
- Arduino IDE
- ESP32 Arduino Core
- RS485 serial communication
- Modbus RTU
- Blynk IoT
- Wi-Fi
- I2C communication

### Main Libraries

- `Wire.h`
- `LiquidCrystal_I2C.h`
- `DHT.h`
- `WiFi.h`
- `BlynkSimpleEsp32.h`
- `HardwareSerial.h`

Install the required libraries using the Arduino IDE Library Manager where applicable.

## 🔌 Hardware Connections

The following pin assignments are taken from the firmware.

| Component / Signal | ESP32 connection |
|---|---|
| MAX485 RO → ESP32 RX2 | GPIO 16 |
| ESP32 TX2 → MAX485 DI | GPIO 17 |
| MAX485 RE and DE control | GPIO 4 |
| Capacitive moisture sensor AO | GPIO 32 |
| DHT11 data output | GPIO 27 |
| Soil pH sensor analog output, if installed | GPIO 35 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

**NPK sensor connections:** Connect the NPK sensor's RS485 A and B terminals to the corresponding MAX485 A and B terminals. The MAX485 interfaces with the ESP32 UART through RO, DI, and the direction-control pins.

**Important:** The supplied firmware does not read the soil pH sensor. GPIO 35 is therefore listed only as a possible connection shown in the circuit diagram, not as an implemented firmware feature.

### Communication Configuration

| Parameter | Configuration |
|---|---|
| ESP32 UART | HardwareSerial 2 |
| UART RX | GPIO 16 |
| UART TX | GPIO 17 |
| Baud rate | 4800 |
| Serial format | 8 data bits, no parity, 1 stop bit |
| Modbus function | 0x03, Read Holding Registers |
| Modbus slave address | 0x01 |
| LCD I2C address | 0x27 |
| LCD dimensions | 16 columns × 2 rows |

Verify the sensor's manual for its supported baud rate, slave address, register map, and required power supply.

### Power and Electrical Safety

- Use the power supply specified by each sensor's datasheet.
- Connect grounds appropriately for the circuit design.
- Do not apply 5 V directly to an ESP32 GPIO or ADC input.
- Check the MAX485 module's logic-level compatibility with the ESP32.
- Do not power the NPK sensor from the ESP32's 3.3 V pin unless its specifications explicitly permit it.

## ⚙️ Working Principle

1. **Initialization:** The ESP32 initializes the LCD, DHT11, UART2, Wi-Fi, and Blynk configuration.
2. **NPK acquisition:** The firmware sends Modbus RTU requests to read the configured NPK registers.
3. **Moisture acquisition:** The ESP32 reads the analog moisture sensor and maps the calibrated ADC range to a percentage.
4. **Environmental sensing:** The DHT11 provides temperature and relative humidity readings.
5. **Health score calculation:** The firmware evaluates N, P, K, and moisture against configured ranges.
6. **LCD update:** The display cycles through environmental measurements, NPK readings, and the calculated health score.
7. **Cloud transmission:** When connected, the ESP32 sends measurements to the configured Blynk virtual pins.

Sensor updates are scheduled every five seconds by the firmware's timer.

## 🖥️ LCD Display

The 16×2 LCD cycles through three screens.

**Screen 1: Environmental readings**

Displays temperature, humidity, and soil moisture.

**Screen 2: NPK readings**

Displays the measured N, P, and K values.

**Screen 3: Calculated health score**

Displays the score and a status label:

- `Good` for scores of 75 or above.
- `OK` for scores from 40 to 74.
- `Bad` for scores below 40.

These labels reflect the firmware's configured scoring rules, not a scientifically validated diagnosis of overall soil health.

## ☁️ Blynk IoT Integration

The system uses Blynk IoT to display sensor readings remotely.

| Virtual Pin | Parameter |
|---|---|
| V0 | Temperature |
| V1 | Relative humidity |
| V2 | Soil moisture percentage |
| V3 | Calculated health score |
| V4 | Nitrogen (N) reading |
| V5 | Phosphorus (P) reading |
| V6 | Potassium (K) reading |

Configure matching datastreams in the Blynk template and add suitable widgets to the dashboard.

### Credentials and Security

The firmware requires Wi-Fi credentials and a Blynk authentication token.

**Never commit real credentials to a public repository.**

Use placeholders in the published source code, or move credentials into a private local header excluded through `.gitignore`. Provide a sanitized example configuration so other users can configure their own devices.

## 📊 Soil Health Score

The firmware calculates a score out of 100 based on four checks, each contributing 25 points.

| Parameter | Configured range | Points |
|---|---|---:|
| Nitrogen | 30–80 | 25 |
| Phosphorus | 20–60 | 25 |
| Potassium | 10–50 | 25 |
| Soil moisture | 30–70% | 25 |

The final score is the sum of the points awarded for the four conditions.

**Interpretation:** This is a simple rule-based indicator implemented in the firmware. The ranges are configurable example thresholds and must not be treated as universal agronomic recommendations. Appropriate NPK ranges depend on sensor calibration, units, soil type, crop, growth stage, and local agricultural guidance.

## 🚀 Installation and Setup

### Prerequisites

- Arduino IDE
- ESP32 board support package
- ESP32 development board
- Required sensors and interface modules
- Wi-Fi access
- Blynk IoT account and configured dashboard

### Steps

1. Clone or download this repository.
2. Open the firmware `.ino` file in Arduino IDE.
3. Install the required libraries.
4. Select the correct ESP32 board and serial port.
5. Configure your local Wi-Fi credentials and Blynk token.
6. Verify the GPIO assignments and sensor power requirements.
7. Upload the firmware.
8. Open the Blynk dashboard and verify incoming readings.
9. Compare sensor readings with appropriate reference measurements.

### Troubleshooting

| Issue | Checks |
|---|---|
| NPK values return zero | Check A/B wiring, UART pins, baud rate, slave ID, register addresses, and response timing |
| LCD displays no text | Check I2C address, SDA/SCL connections, contrast, and power |
| DHT11 returns invalid values | Check data wiring, sensor type, power, and library configuration |
| Moisture percentage is inaccurate | Recalibrate wet and dry ADC values for the actual sensor and soil conditions |
| Blynk receives no data | Check Wi-Fi connectivity, authentication, template settings, and virtual-pin datastreams |


Adjust the names and paths to match the files actually present in your repository.

## 🧪 Testing and Results

Document the results obtained from the assembled system.

Recommended evidence:

- Photograph of the complete hardware setup.
- Screenshot of the LCD displaying readings.
- Screenshot of the Blynk dashboard.
- Sample NPK, moisture, temperature, and humidity readings.
- A short test describing sensor calibration and communication reliability.

Add only measured results from your actual tests. Do not insert estimated accuracy or performance figures without experimental evidence.

## 🌾 Applications

- Soil condition monitoring
- Agricultural IoT prototypes
- Educational embedded-systems projects
- Remote observation of soil moisture and environmental conditions
- Research and experimentation in smart agriculture

## ⚠️ Limitations

- NPK accuracy depends on the sensor, calibration, soil conditions, and register interpretation.
- The health score uses fixed thresholds and is not a validated soil fertility assessment.
- The DHT11 provides environmental temperature and humidity, not soil temperature.
- The current firmware does not implement pH sensing, automated irrigation, or fertilizer recommendations.
- The Blynk dashboard requires proper configuration and network connectivity.

## 🔮 Future Scope

- Integrate and calibrate a soil pH sensor.
- Add data logging and historical trend analysis.
- Implement automated irrigation with appropriate hardware and safety controls.
- Develop crop-specific soil-condition recommendations using validated agronomic references.
- Improve sensor calibration, error handling, and communication diagnostics.
- Evaluate the system using reference instruments and field trials.

##👨‍💻Author

Bhagwat Shinde

Electronics and Telecommunication Engineering | Embedded Systems | IoT

GitHub: [BhagwatShinde01](https://github.com/BhagwatShinde01)

---

If you find this project useful for learning or experimentation, feel free to explore the repository and adapt the implementation to your hardware.

