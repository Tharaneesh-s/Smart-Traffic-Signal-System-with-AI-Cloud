
AI-Powered Smart Traffic Signal System
This project implements an AI-powered smart traffic management system that dynamically adjusts traffic signals based on real-time traffic density and emergency vehicle detection. The system integrates computer vision (YOLOv8), GPS tracking, cloud connectivity (Google Cloud), and IoT (ESP32/ESP8266, MQTT) to optimize traffic flow and reduce congestion.

Table of Contents
Introduction
Features
Hardware Components
Software
Circuit Diagram
License
Introduction
Traffic congestion and emergency response delays are major urban challenges. This project aims to develop an intelligent AI-powered traffic signal system that adapts to real-time traffic conditions and prioritizes emergency vehicles using computer vision and IoT.

Features
✅ AI-Based Traffic Detection: Uses YOLOv8 and OpenCV to detect and count vehicles.
✅ Adaptive Signal Control: Adjusts signal timing dynamically based on traffic density.
✅ Emergency Vehicle Priority: Uses GPS tracking to detect and clear routes for ambulances/fire trucks.
✅ Cloud Integration: Stores traffic data on Google Cloud for real-time monitoring and analytics.
✅ IoT Connectivity: ESP32/ESP8266 controls signals via MQTT protocol.
✅ Web Dashboard: Displays real-time traffic conditions and alerts.

Hardware Components
ESP32/ESP8266 – IoT controller for traffic light control.
Raspberry Pi / Jetson Nano – Runs AI model (YOLOv8) for object detection.
IP Camera – Captures real-time video for traffic analysis.
GPS Module – Tracks emergency vehicles.
LED Traffic Lights – Simulated or real-world signal control.
Power Supply – 5V/12V DC power sources.
Software
Python (YOLOv8 + OpenCV) – AI-based traffic detection and classification.
Google Cloud Platform – Stores and processes traffic data.
MQTT Protocol – Real-time communication between ESP32 and cloud.
Arduino IDE – Programming ESP32/ESP8266 for traffic light control.
Flask/Django Web App – Dashboard for monitoring and control.
