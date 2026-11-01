# IoT-Project-PulseOxymeter

Overview:

This project is aimed to build a compact pulse oximeter using max30102 sensor and xiao seeed c3, also integrating  a rechargeable battery. This optical sensor based on obtained IR and red light values calculate heart beat and oxygen level and displays the result by transmitting via BLE to a compatible mobile application based on swift UI.

Required Hardware Components:

- Xiao Seeed C3
  Main esp-32 based microcontroller
- Max30102
  Main sensor measuring heart rate and oxygen saturation.
- LiPo rechargeable battery module
  To keep the device running while disconnected from power.
- Breadboard and jumper wires
  For device prototyping
- Protoboard
  For circuit assembling
- Soldering Components
  To solder and combining electrical components
- Connecting Wires
  To establish connection between components

Features:

- Heart rate measurement (BPM).
- Blood oxygen saturation (SpO₂) estimation.
- Optical sensing using the MAX30102.
- Wireless data transmission using BLE.
- Real-time sensor data monitoring through the Serial Monitor and transmission of results to custom ios application.

Working Principle
The MAX30102 uses red and infrared LEDs along with a photodetector to measure changes in light absorption caused by blood flow.
The sensor provides two sets of readings:
RED: Red light intensity measurements.
IR: Infrared light intensity measurements.
These readings are collected by the ESP32-C3 through the I²C communication interface.
The collected samples are processed using the Maxim heart rate and SpO₂ algorithm to estimate heart rate and blood oxygen saturation.


Communication:

The device transmits data by transferring sensor values as data packets via Bluetooth Low Energy(BLE), using custom GATT service and characteristic. The device identifies itself as “test_weather_xiao” that allows the nearby devices to connect to it. The data transmission occurs approximately within a radius of 7 metres.

Software and Libraries used:

- Arduino Framework
- Wire.h
- MAX30105 Library
- Maxim SpO₂ Algorithm
- ESP32 BLE Library
- Wire (I²C) Library

Current Limitations:

- Sensor calibration requires further validation
- Current transmission heavily relies on BLE transmission compromising in range.
- Heavy fluctuation of sensor readings due to environment noise.
- The device hardware box needs design fixes such that the finger used in calculation is held tightly giving little room to       noise related errors.

Future Improvements:

- Improve sensor calibration and measurement reliability.
- Include calculation of heart rate variability and related medical values using currently measured components

Project Status:
Prototype






