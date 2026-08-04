# ROS2 Communication Node

# Overview

The ROS2 communication node acts as the interface between:

- High-level robot control
- AI and strategy systems
- ESP32 robot hardware

The node communicates with the robot using UDP over WiFi.

---

# Main Responsibilities

The node performs:

- Velocity command processing
- Wheel RPM calculations
- UDP packet transmission
- Telemetry reception
- ROS2 topic integration

---

# Architecture

```text
ROS2 Topics
     │
     ▼
ROS2 Communication Node
     │
     ▼
UDP WiFi Communication
     │
     ▼
ESP32 Robot
```

---

# ROS2 Interfaces

# Subscribers

## `/cmd_vel`

Receives robot velocity commands.

Message type:

```text
geometry_msgs/msg/Twist
```

---

# Published Information

The node can publish:

- Robot orientation
- Wheel velocities
- Communication status

---

# Velocity Processing

The node converts:

```text
Linear velocity
Angular velocity
```

into wheel RPM commands.

---

# Wheel Kinematics

For a three-wheel omnidirectional robot:

- Each wheel velocity is computed independently
- Robot motion is decomposed into wheel vectors

---

# RPM Command Packet

The node builds:

```cpp
robotCmdPacket
```

containing:

- Left wheel RPM
- Right wheel RPM
- Back wheel RPM
- Kicker command
- Stop command

---

# UDP Communication

# Transmission Flow

## 1. Build Binary Packet

The ROS2 node serializes:

```cpp
robotCmdPacket
```

---

## 2. Send UDP Packet

Packet is sent to:

```text
ESP32 IP + UDP port
```

---

## 3. Receive Telemetry

The node receives:

```cpp
robotStatePacket
```

from the robot.

---

# Handshake Process

Before normal operation:

1. ROS2 node sends initialization packet
2. ESP32 replies with confirmation packet
3. Communication becomes active

---

# Real-Time Design

# Why UDP Was Chosen

UDP provides:

- Lower latency
- Faster updates
- Better real-time behavior

compared to TCP.

---

# Latest Packet Priority

The node continuously sends fresh commands.

Old commands are automatically replaced.

---

# High-Frequency Updates

The system supports very fast update rates:

```text
50 Hz
100 Hz
200 Hz
```

depending on network conditions.

---

# Telemetry Reception

The ROS2 node receives:

```cpp
robotStatePacket
```

containing:

- Robot yaw
- Measured wheel RPMs

This enables:

- Closed-loop control
- Odometry
- Monitoring

---

# Error Handling

The node should detect:

- Packet loss
- Communication timeout
- Invalid packets

to improve robustness.

---

# Possible Improvements

## Reliability Layer

Optional acknowledgements for critical messages.

---

## Packet Sequence Numbers

Detect lost or delayed packets.

---

## CRC Validation

Detect corrupted packets.

---

## Time Synchronization

Measure network latency.

