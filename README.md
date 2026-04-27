# PhytoPi-ESP32

PhytoPi is an IoT-based controlled environment system for supporting a plant throughout its entire growth cycle with minimal human intervention. This repository tracks the SWE-410 Milestone 5 implementation, integrating an ESP32-S3 as the sensor and telemetry node publishing live data to a Raspberry Pi over MQTT.

---

## Features

- Reads soil moisture, ambient temperature, and light level from three analog sensors via ESP32-S3 ADC
- Publishes live JSON telemetry to a Raspberry Pi Mosquitto MQTT broker over Wi-Fi every 5 seconds
- Raspberry Pi aggregates and visualizes data on a live Grafana dashboard
- Modular component structure — each sensor has its own wrapper with a consistent Init/Read API
- Event-driven Wi-Fi reconnect with automatic MQTT re-connection on drop

---

## Project Structure

```
PhytoPi-ESP32/
├── esp32-multisensor-mqtt-rpi-node/
│   ├── CMakeLists.txt
│   ├── main/
│   │   ├── CMakeLists.txt
│   │   └── main.c
│   └── components/
│       ├── wifi_manager/
│       │   ├── CMakeLists.txt
│       │   ├── wifi_manager.h
│       │   └── wifi_manager.c
│       ├── mqtt_manager/
│       │   ├── CMakeLists.txt
│       │   ├── mqtt_manager.h
│       │   └── mqtt_manager.c
│       ├── thermistor_wrapper/
│       │   ├── CMakeLists.txt
│       │   ├── thermistor_wrapper.h
│       │   └── thermistor_wrapper.c
│       ├── light_sensor_wrapper/
│       │   ├── CMakeLists.txt
│       │   ├── light_sensor_wrapper.h
│       │   └── light_sensor_wrapper.c
│       └── soil_sensor_wrapper/
│           ├── CMakeLists.txt
│           ├── soil_sensor_wrapper.h
│           └── soil_sensor_wrapper.c
└── docs/
    ├── milestone5_report.pdf
    └── screenshots/
```

---

## Sensors

| Sensor | GPIO | ADC Channel | Output |
|---|---|---|---|
| NTC Thermistor (10kΩ, B=3950) | GPIO5 | ADC1 CH4 | °C |
| Photoresistor (light sensor) | GPIO2 | ADC1 CH1 | 0–100% |
| Capacitive soil moisture | GPIO1 | ADC1 CH0 | 0–100% |

All three sensors share a single `adc_oneshot_unit_handle_t` created in `app_main` and passed to each wrapper on init.

---

## MQTT

**Broker:** Mosquitto on Raspberry Pi, port 1883, authenticated

**Topic:** `gcu/lab/env/telemetry`

**Payload (every 5 seconds):**
```json
{"device":"esp32-01","temperature_c":24.3,"light_pct":61.2,"soil_moisture_pct":45.0}
```

**Subscribe (on Pi):**
```bash
mosquitto_sub -h 127.0.0.1 -t 'gcu/lab/env/telemetry' -u nolan -P rootroot -v
```

---

## Build / Flash / Run

**Prerequisites:** ESP-IDF v5.2.6, target ESP32-S3

```bash
cd esp32-multisensor-mqtt-rpi-node
idf.py set-target esp32s3
idf.py build
idf.py -p COM4 flash monitor        # Windows
idf.py -p /dev/ttyUSB0 flash monitor  # Linux
```

Update credentials in `main/main.c` before flashing:
```c
#define WIFI_SSID       "your_ssid"
#define WIFI_PASSWORD   "your_password"
#define MQTT_BROKER_URI "mqtt://<pi-ip>:1883"
#define MQTT_USERNAME   "nolan"
#define MQTT_TOPIC      "gcu/lab/env/telemetry"
```

---

## Raspberry Pi Setup

```bash
# Start broker and dashboard
sudo systemctl start mosquitto
sudo systemctl start grafana-server
```

Grafana dashboard accessible at `http://<pi-ip>:3000` (default login: admin / admin).

---
