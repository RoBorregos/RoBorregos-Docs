# ESP32 Arduino Communication Class

# Overview

The `Communication` class implements wireless communication between an ESP32 robot controller and a ROS2 computer using:

- WiFi
- UDP sockets
- Binary packet transmission

The class is optimized for:

- Low latency
- Real-time robotics
- Continuous command streaming

---

# Main Responsibilities

The class handles:

- WiFi connection
- UDP initialization
- Handshake protocol
- Command reception
- Robot state transmission

---

# Packet Structures

# 1. Initialization Packet

```cpp
struct robotInitPacket {
    int msg;
};
```

Used during the handshake process.

---

# 2. Command Packet

```cpp
struct robotCmdPacket {
    float rpm_left;
    float rpm_right;
    float rpm_back;
    bool kicker;
    bool stop;
};
```

Contains robot control commands.

---

# 3. State Packet

```cpp
struct robotStatePacket {
    float yaw;
    float rpm_left;
    float rpm_right;
    float rpm_back;
};
```

Contains telemetry information.

---

# Packed Structures

All structures use:

```cpp
__attribute__((packed))
```

This guarantees deterministic binary packet layouts.

---

# Constructor

```cpp
Communication(const char *ssid,
              const char *password,
              int local_port)
```

Initializes:

- WiFi credentials
- UDP port

---

# WiFi Initialization

# Method

```cpp
wl_status_t configComms()
```

---

# Responsibilities

## 1. Connect to WiFi

```cpp
WiFi.begin(ssid_, password_);
```

---

## 2. Wait for Connection

```cpp
while (WiFi.status() != WL_CONNECTED)
```

---

## 3. Start UDP Server

```cpp
udp_.begin(local_port_);
```

---

## 4. Disable WiFi Sleep

```cpp
WiFi.setSleep(false);
```

Improves real-time performance.

---

# Handshake System

# Method

```cpp
bool commsInit()
```

---

# Purpose

Establish communication with the ROS2 node.

---

# Handshake Flow

## ROS2 Sends

```text
50056
```

---

## ESP32 Replies

```text
50057
```

---

# Why Multiple Replies?

The ESP32 sends the response multiple times because UDP does not guarantee delivery.

---

# UDP Command Reception

# Method

```cpp
bool udpReceiveCmd(robotCmdPacket *cmd)
```

---

# Responsibilities

- Read incoming UDP packets
- Validate packet size
- Copy binary data into structures
- Discard invalid packets

---

# Latest Packet Strategy

The method processes all queued packets and keeps only the newest one.

This minimizes latency accumulation.

---

# Telemetry Transmission

# Method

```cpp
void udpSendState(robotStatePacket state)
```

---

# Responsibilities

- Build UDP packet
- Serialize telemetry structure
- Send robot state to ROS2 node

---

# Communication Model

```text
ROS2 PC ---- UDP ----> ESP32
       <--- UDP ----
```

---

# Real-Time Design Decisions

## UDP Instead of TCP

Chosen because:

- Lower latency
- No retransmission delays
- Better real-time behavior

---

## Binary Communication

Advantages:

- Smaller packets
- Faster transmission
- Lower CPU usage

---

## Stateless Communication

UDP avoids persistent connection overhead.

---

# Possible Improvements

- CRC validation
- Sequence numbers
- Timeout detection
- Packet timestamps
- Reliability layer


