# Ultrasonic Distance Sensor – Smart Home Distributed System

This project was developed as part of a university course on *Distributed Systems in Automotive Engineering*.  
It is a **collaborative project**, implemented in cooperation with other student teams and professors, integrating multiple subsystems into a distributed Smart Home system using CAN communication.

---

## 📌 Project Overview

The system measures distances using an **ultrasonic sensor** and visualizes them via an **8-LED bar**.  
Relevant data is exchanged with other subsystems through a **CAN bus**, including:

- Sending measured distance values  
- Sending an alarm flag when a threshold is undershot  
- Receiving a configurable distance threshold  

The project includes both hardware and software development as well as CAN-based interaction with other nodes in the system.

---

## 🎯 Objectives

- Develop an autonomous distance-measurement subsystem  
- Implement CAN message exchange within a distributed architecture  
- Display measured distances using 8 LEDs  
- Trigger an alarm message when a threshold is reached  
- Design and build a custom PCB  
- Implement embedded C firmware for measurement, LED control, and CAN communication  
- **Integrate seamlessly with other student-developed modules as part of a collaborative system**

---

## 🧠 Software Architecture

The firmware was implemented in **C** and runs on an **ATmega microcontroller**.  
The key components include:

### 🔹 Main Loop
- System initialization (CAN, SPI, GPIO, external interrupt, timer)  
- Receive threshold value via CAN  
- Trigger measurement every 1 second  
- Convert timer values to distance  
- Update LED output  
- Send measurement and alarm messages over CAN  

### 🔹 External Interrupt + Timer1
Used for precise measurement of the **ultrasonic sensor echo pulse**:

1. Rising edge detected → start Timer1  
2. Falling edge detected → stop Timer1  
3. Store timer value  
4. Convert timer value to distance (based on ultrasonic travel time)  

This method ensures high-resolution measurement and avoids blocking delays.

### 🔹 LED Control
The measured distance (0–300 cm) is mapped to **8 LEDs**:
Step size = (300 − threshold) / 8
LED count = (distance − threshold) / step size


LED behavior:
- All LEDs OFF → distance outside sensor range  
- Gradual lighting based on measured distance  
- All LEDs ON + alarm sent → distance below threshold  

A `switch-case` structure is used for LED excitation patterns.

---

## 🔗 CAN Communication

The module exchanges data exclusively via CAN.  
Message structure was defined during the course.

### CAN Message (TX) — Distance + Alarm  
**Identifier:** `0x300`  
**Data:**

| Byte | Content                       |
|------|-------------------------------|
| 0    | Distance (MSB)                |
| 1    | Distance (LSB)                |
| 2    | Alarm flag (`0x55`/on, `0x00`/off) |

### CAN Message (RX) — Threshold  
**Identifier:** `0x301`  
**Data:**

| Byte | Content         |
|------|-----------------|
| 0    | Threshold (MSB) |
| 1    | Threshold (LSB) |

---

## 🚀 Getting Started

1. Assemble and solder the PCB  
2. Flash the microcontroller firmware  
3. Connect the module to the CAN bus (ensure 120 Ω termination as required)  
4. Power on the system  
5. The device automatically starts distance measurement and CAN communication  

---

## 🤝 Collaboration

This project was **developed in cooperation with other teams**, with each team contributing separate modules.  
The distance sensor system was integrated as part of a larger distributed Smart Home environment, demonstrating the interaction of multiple subsystems via CAN.

---


