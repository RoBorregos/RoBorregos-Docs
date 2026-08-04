# VSSS Robot Communication System 

# Documentation Index

- [ESP32 Arduino Communication Class Documentation](arduino.md)
- [ROS2 Communication Node Documentation](ros2.md)

## Overview

This project implements a real-time wireless communication architecture for a VSSS robot using:

- ROS2
- ESP32
- WiFi
- UDP communication

The system is divided into two main software components:

1. ROS2 Communication Node
2. ESP32 Arduino Communication Layer

---

# Table of Contents

1. [Introduction](#introduction)
2. [Why UDP Was Chosen](#why-udp-was-chosen)
3. [How UDP Communication Works](#how-udp-communication-works)
4. [System Architecture](#system-architecture)
5. [Documentation Files](#documentation-files)
6. [Communication Flow](#communication-flow)
7. [Real-Time Design Considerations](#real-time-design-considerations)

---

# Introduction

The robot communication system was designed for:

- Low latency
- High update frequency
- Real-time robot control
- Continuous telemetry streaming

Communication between the ROS2 computer and the ESP32 robot controller is performed using UDP over WiFi.

---

# Why UDP Was Chosen

UDP (User Datagram Protocol) was selected instead of TCP because robotics systems prioritize:

```text
Fresh data over guaranteed delivery
```

---

# TCP vs UDP

| Feature | TCP | UDP |
|---|---|---|
| Guaranteed delivery | Yes | No |
| Retransmission | Yes | No |
| Latency | Higher | Lower |
| Real-time suitability | Medium | Excellent |

---

# Why UDP Is Better for Robot Control

In robotics:

- Commands are sent continuously
- New commands quickly replace old ones
- Delays are more dangerous than packet loss

Example:

If one velocity packet is lost:

```text
The next packet arrives milliseconds later
```

This is acceptable.

However, TCP retransmissions introduce delays that can destabilize robot control loops.

---

# Advantages of UDP in This System

## 1. Low Latency

UDP minimizes communication overhead.

---

## 2. High Update Rates

Supports:

```text
50 Hz
100 Hz
200 Hz
```

or more without waiting for acknowledgements.

---

## 3. Simplicity

UDP is stateless.

No persistent connection management is required.

---

## 4. Better Real-Time Performance

The system always prioritizes the newest command.

---

# How UDP Communication Works

The ESP32 opens a UDP port and waits for packets.

The ROS2 node sends binary packets containing:

- Wheel RPMs
- Kicker commands
- Stop signals

The ESP32 responds with telemetry packets containing:

- Robot orientation
- Wheel velocities

---

# Binary Communication

Instead of using strings or JSON, the system uses packed binary structures.

Advantages:

- Smaller packets
- Faster transmission
- Lower CPU usage
- Deterministic packet sizes

---

# Packed Structures

All packets use:

```cpp
__attribute__((packed))
```

This prevents compiler padding and ensures deterministic binary layouts.

---

# System Architecture

```text
┌────────────────────┐
│ ROS2 Control Node  │
│ AI / Path Planning │
└─────────┬──────────┘
          │
          │ UDP over WiFi
          ▼
┌────────────────────┐
│ ESP32 Robot        │
│ Motor Control      │
│ Sensors            │
└────────────────────┘
```

---

# Documentation Files

## 1. ROS2 Node Documentation

[Ros2 Node](ros2.md)

Contains:

- ROS2 node architecture
- Topic subscriptions
- UDP transmission logic
- RPM calculations
- Real-time behavior

---

## 2. ESP32 Communication Documentation

[Arduino class](arduino.md)

Contains:

- WiFi initialization
- UDP communication layer
- Handshake process
- Packet structures
- Real-time packet handling

---

# Communication Flow

## Step 1 — WiFi Connection

ESP32 connects to the WiFi network.

---

## Step 2 — UDP Initialization

ESP32 opens a UDP listening port.

---

## Step 3 — Handshake

ROS2 node sends initialization packet.

ESP32 replies with confirmation packet.

---

## Step 4 — Command Streaming

ROS2 continuously sends:

```text
robotCmdPacket
```

---

## Step 5 — Robot Control

ESP32:

- Receives commands
- Controls motors
- Updates actuators

---

## Step 6 — Telemetry Feedback

ESP32 sends:

```text
robotStatePacket
```

back to the ROS2 node.

---

# Real-Time Design Considerations

## WiFi Sleep Disabled

```cpp
WiFi.setSleep(false);
```

Prevents:

- Latency spikes
- Delayed packets
- Sleep wakeup overhead

---

## Latest Packet Priority

The system processes the newest available packet to minimize latency accumulation.

---

## Binary Packet Transmission

Avoids expensive serialization and parsing.

---

## Stateless UDP Communication

Reduces overhead and improves timing consistency.

