# BinNova — Smart IoT Waste Management System

**Smarter waste monitoring. Cleaner communities.**

BinNova is an IoT-based waste management prototype designed to monitor bin fill levels, support waste segregation, and improve waste collection workflows.

## Key Features

- Ultrasonic sensor-based bin fill-level monitoring
- ESP32-based sensor integration
- Wet/dry waste segregation prototype
- LED and buzzer status indicators
- Servo-based smart bin lid
- LCD/TFT display support
- Connected monitoring and alert workflow
- QR-based citizen reporting concept

## Hardware

- ESP32 development board
- HC-SR04 ultrasonic sensor
- IR sensor for segregation experiments
- Servo motor
- LEDs and buzzer
- LCD/TFT display
- Power supply and connecting wires

Actual components and features may vary by prototype version.

## Repository Structure

- `firmware/01_tft_test/` — TFT display testing
- `firmware/02_ultrasonic_test/` — Ultrasonic sensor testing
- `firmware/03_fill_test/` — Fill-level testing
- `firmware/04_LED + buzzer working/` — LED and buzzer testing
- `firmware/Calibration/` — Sensor calibration
- `firmware/esp32/BinNova_Expo_Step2_Corrected/` — Main prototype firmware
- `docs/` — Project documentation
- `hardware/` — Hardware resources
- `assets/` — Images and project media

## Getting Started

1. Install the Arduino IDE.
2. Install ESP32 board support.
3. Install the libraries required by the selected sketch.
4. Open the relevant `.ino` file.
5. Copy `secrets.example.h` to `secrets.h` in the same firmware folder.
6. Add your private credentials to the local `secrets.h` file.
7. Select the correct ESP32 board and port, then compile and test.

## Security

- Never commit `secrets.h`, Wi-Fi passwords, API tokens, or private phone numbers.
- Keep `secrets.example.h` limited to placeholders.
- Rotate credentials that were previously exposed.
- Enable TLS certificate verification in production deployments.

## Project Status

This repository contains prototype firmware and sensor experiments. Test individual modules before integrating them into a complete deployment.

## Maintainer

**Gokul D.**

- GitHub: [build-by-gokul](https://github.com/build-by-gokul)
- Repository: [Bin-Nova](https://github.com/build-by-gokul/Bin-Nova)
