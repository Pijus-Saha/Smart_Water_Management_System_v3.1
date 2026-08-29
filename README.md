# Smart Water Management System (v3.1)

An IoT-based water monitoring and pump control system built using **ESP32**, **HC-SR04 Ultrasonic Sensor**, and **Blynk IoT Platform**.

## Features
- Real-time water level monitoring
- Automatic pump ON/OFF control
- Manual and Force OFF control via Blynk
- Blynk event notifications for tank full and low level
- Fully compatible with ESP32 microcontroller
- Uses HC-SR04 ultrasonic sensor for distance measurement

## Hardware Components
- ESP32 DevKit
- HC-SR04 Ultrasonic Sensor
- 3V Submersible DC Water Pump (controlled via Relay)
- Relay Module (5V trigger)
- Jumper Wires
- Power Supply

## Pin Configuration
| Component | Pin | Description |
|------------|-----|-------------|
| Ultrasonic Sensor (TRIG) | GPIO 23 | Trigger Pin |
| Ultrasonic Sensor (ECHO) | GPIO 22 | Echo Pin |
| Relay / Pump Control | GPIO 21 | Pump ON/OFF |
| Power | 3.3V | For HC-SR04 |
| GND | GND | Common Ground |

## Blynk Setup
1. Create a new **Blynk Template**:
   - Template ID: `TMPL6agbyZrQX`
   - Template Name: `Smart Water Management System`
2. Add Virtual Pins:
   - **V0** → Water Level Gauge  
   - **V1** → Manual Pump Control (Switch)  
   - **V2** → Force OFF Control (Switch)
3. Copy your **Auth Token** into the code.

## Code Overview
The system uses ultrasonic readings to calculate water level percentage:
```cpp
float height = TANK_HEIGHT - distance;
float percent = (height / TANK_HEIGHT) * 100.0;
