# PhytoPi-ESP32
PhytoPi is an IoT-based controlled environment system for supporting a plant throughout its entire growth cycle with minimal human intervention. This repository tracks our version of this project for Software Engineering 2, with the addition of an ESP32-based microcontroller.

# Features

Collects soil moisture, temperature, and humidity data using ESP32-attached sensors

Transmits sensor data to a Raspberry Pi via Bluetooth Low Energy (BLE)

Raspberry Pi aggregates, processes, and stores plant data

Data is visualized in a web-based dashboard

Modular structure allows adding additional sensors in the future

# Project Structure

    /PhytoPiESP32 - Main ESP-IDF project directory, stores all development files.
    /docs - Stores all documentation for the project, such as the Project Charter.

# Version Control and Workflow

Development uses Git and GitHub for version control

Feature branches are used for new functionality, bug fixes, and experimental changes

Pull requests are created for merging changes into main after review

Releases are tagged using Semantic Versioning (MAJOR.MINOR.PATCH)

# System Sensors

Capacitive Soil Moisture Sensor: Determines soil water content and triggers watering decisions

Temperature and Humidity Sensor: Monitors ambient conditions affecting plant growth

ESP32 Data Collection: ESP32 reads all sensors and transmits data via BLE to the Raspberry Pi
