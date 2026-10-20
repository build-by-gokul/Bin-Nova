# BinNova — Smart IoT Waste Management

BinNova is an IoT-based waste monitoring prototype designed to monitor separate biodegradable and non-biodegradable waste compartments.

## Features

- Dual-bin fill-level monitoring using ultrasonic sensors
- ESP32-based embedded control
- Live local web dashboard
- TFT display for on-device status
- LED and buzzer status indicators
- Firebase Realtime Database integration
- Configurable bin height and fill thresholds

## Technology Stack

- **Hardware:** ESP32, ultrasonic sensors, ST7735 TFT display, LEDs and buzzer
- **Embedded software:** Arduino framework and C++
- **Dashboard:** HTML, CSS and JavaScript hosted by the ESP32
- **Cloud:** Firebase Realtime Database

## Repository Structure

- `firmware/` — Embedded sketches and sensor tests
- `docs/` — System architecture and technical documentation
- `hardware/` — Hardware references and wiring information
- `assets/` — Project photographs and diagrams

## Getting Started

1. Install the Arduino IDE and ESP32 board support.
2. Install the required Adafruit GFX and ST7735 libraries.
3. Configure your private settings locally using `secrets.h`.
4. Set the bin height to match the actual hardware.
5. Compile and upload the sketch to the correct ESP32 board.
6. Check Serial Monitor at 115200 baud for device and dashboard information.
7. Configure Firebase access rules securely before connecting the device.

Never commit real Wi-Fi passwords, SMS API tokens, or other private credentials.

## Project Status

**Prototype / Expo firmware.** Hardware calibration, cloud security and end-to-end testing should be verified before deployment.

## Maintainer

[build-by-gokul](https://github.com/build-by-gokul)

**Repository:** [Bin-Nova](https://github.com/build-by-gokul/Bin-Nova)
