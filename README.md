# Multi-Stage Cold Chain Validator (IoT)

This project demonstrates an IoT-based system for monitoring food quality
during cold-chain transportation and storage.

The system measures temperature, humidity, and gas levels using sensors
connected to an ESP32 microcontroller and uploads the data to the cloud
using ThingSpeak.

MATLAB-based analytics are used to compute exposure metrics and a
Spoilage Risk Score (SRS), which helps identify unsafe conditions.
An alert mechanism using LED and buzzer is triggered when the risk exceeds
a predefined threshold.

## Hardware Used
- ESP32
- DS18B20 Temperature Sensor
- DHT22 Humidity Sensor
- MQ-135 Gas Sensor
- LED and Buzzer

## Software Used
- Arduino IDE
- ThingSpeak Cloud Platform
- MATLAB Analytics

## Features
- Real-time sensor data logging
- Cloud-based data analytics
- Stage-wise monitoring using QR inputs
- Automatic visual and audio alerts

## Application
The system can be used in food logistics, cold storage monitoring,
and transportation quality assurance.

## Arduino Firmware
The Arduino sketch (`esp32_cold_chain_validator.ino`) runs on the ESP32
microcontroller. It reads data from temperature, humidity, and gas sensors
and uploads the values to ThingSpeak using HTTP requests.

The firmware also fetches the Spoilage Risk Score (SRS) computed in the
ThingSpeak MATLAB Analysis environment and activates LED and buzzer alerts
when the risk exceeds a defined threshold.

## MATLAB Analytics
The MATLAB script (`spoilage_risk_analysis.m`) implements the data analytics
logic used in the project. It processes sensor data uploaded to ThingSpeak
and computes exposure-based metrics such as Safe Exposure Time (SET),
Humidity Exposure Time (HET), and a Spoilage Risk Score (SRS).

The analytics are executed within the ThingSpeak MATLAB Analysis environment.
The script provided in this repository documents the same logic for reference
and reproducibility.

## Author
Vinay Koli
