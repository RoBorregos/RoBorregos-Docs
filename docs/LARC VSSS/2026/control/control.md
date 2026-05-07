# VSSS Robot Control Loop

## Overview

This document describes the control loop implementation of a VSSS (Very Small Size Soccer) robot using Arduino-compatible hardware. The system controls three omnidirectional motors, receives commands over UDP, and sends robot state feedback to an external controller.

The control architecture is divided into three main subsystems:

- Communication subsystem
- Motor control subsystem
- Debug and monitoring subsystem

The robot uses periodic non-blocking loops based on `millis()` timing.

---

# Main Components

## Communication

The communication layer is implemented using the `Communication` class.

```cpp
Communication udpClientServer(
    WIFI_SSID,
    WIFI_PASSWORD,
    LOCAL_PORT
);
```

### Responsibilities

- Connect to WiFi
- Initialize UDP communication
- Receive wheel speed commands
- Send robot state feedback

---

## Motors

The robot uses three motors:

- Left motor
- Right motor
- Back motor

Each motor is represented by a `Motor` object.

```cpp
Motor motorLeft(
    0,
    inA1,
    inA2,
    PWMA,
    ENCA
);

Motor motorBack(
    1,
    inB1,
    inB2,
    PWMB,
    ENCB
);

Motor motorRight(
    2,
    inC1,
    inC2,
    PWMC,
    ENCC
);
```

Each motor contains:

- PWM control
- Direction control
- Encoder reading
- Closed-loop speed controller

---

# System Initialization

The initialization is performed inside `setup()`.

```cpp
void setup()
```

## Serial Communication

```cpp
Serial.begin(9600);
```

Used for debugging and telemetry.

---

## Status LED

```cpp
pinMode(15, OUTPUT);
digitalWrite(15, HIGH);
```

GPIO 15 is configured as a status LED.

---

## Communication Initialization

```cpp
udpClientServer.configComms();
udpClientServer.commsInit();
```

This initializes:

- WiFi connection
- UDP socket

---

## Motor Initialization

```cpp
motorLeft.begin();
motorRight.begin();
motorBack.begin();
```

This initializes:

- PWM outputs
- Direction pins
- Encoder interrupts
- Internal controller variables

---

## PID Gain Configuration

```cpp
motorLeft.attach_kterms(10, 360, 0);
motorRight.attach_kterms(10, 250, 0);
motorBack.attach_kterms(10, 210, 0);
```

Each motor has independent controller gains.

### Parameters

```cpp
attach_kterms(Kp, Ki, Kd)
```

### Current Gains

| Motor | Kp | Ki | Kd |
|------|------|------|------|
| Left | 10 | 360 | 0 |
| Right | 10 | 250 | 0 |
| Back | 10 | 210 | 0 |

The controller currently behaves as a PI controller because `Kd = 0`.

---

# Main Control Loop

The robot executes all tasks inside:

```cpp
void loop()
```

The architecture is cooperative and non-blocking.

---

# Encoder Update Loop

At the beginning of every iteration:

```cpp
motorLeft.tick();
motorRight.tick();
motorBack.tick();
```

## Purpose

- Update encoder measurements
- Compute wheel speed
- Update internal controller timing

This executes as fast as possible.

---

# LED Task

Executed every 1000 ms.

```cpp
if ((millis() - previous_led_toggle) >= 1000)
```

## Purpose

- Toggle status LED
- Indicate system activity

## Behavior

- LED changes state every second

---

# UDP Communication Task

Executed every 25 ms.

```cpp
if ((millis() - previous_udp_update) >= 25)
```

### Frequency

```text
40 Hz
```

---

## Receiving Commands

```cpp
udpClientServer.udpReceiveCmd(&cmd);
```

The robot receives a `robotCmdPacket`.

### Command Fields

| Variable | Description |
|------|------|
| `rpm_left` | Desired left wheel RPM |
| `rpm_right` | Desired right wheel RPM |
| `rpm_back` | Desired back wheel RPM |
| `stop` | Emergency stop flag |

---

## RPM to RPS Conversion

```cpp
left_rps = cmd.rpm_left / 60;
right_rps = cmd.rpm_right / 60;
back_rps = cmd.rpm_back / 60;
```

The motor controller internally uses:

```text
RPS = Revolutions Per Second
```

---

## Sending Robot State

The robot sends feedback through:

```cpp
udpClientServer.udpSendState(state);
```

### Feedback Fields

| Variable | Description |
|------|------|
| `rpm_left` | Measured left wheel RPM |
| `rpm_right` | Measured right wheel RPM |
| `rpm_back` | Measured back wheel RPM |
| `yaw` | Robot orientation |

Current implementation:

```cpp
state.yaw = 0;
```

Yaw estimation has not yet been implemented.

---

# Motor Control Task

Executed every 25 ms.

```cpp
if ((millis() - previous_motor_update) >= 25)
```

### Frequency

```text
40 Hz
```

---

## Emergency Stop

```cpp
if (stop)
```

When enabled:

```cpp
motorLeft.brake();
motorRight.brake();
motorBack.brake();
```

This immediately stops all motors.

---

## Speed Control

If the robot is not stopped:

```cpp
motorLeft.move(left_rps);
motorRight.move(right_rps);
motorBack.move(back_rps);
```

The `move()` function performs closed-loop wheel speed regulation.

### Input

```text
Desired wheel speed in RPS
```

The controller uses encoder feedback to regulate motor velocity.

---

# Debug Task

Executed every 100 ms.

```cpp
if ((millis() - previous_debug_update) >= 100)
```

### Frequency

```text
10 Hz
```

## Purpose

- Print controller telemetry
- Monitor motor behavior
- Tune PID gains

Functions used:

```cpp
motorLeft.print_debug();
motorRight.print_debug();
motorBack.print_debug();
```

---

# Timing Architecture

| Task | Period | Frequency |
|------|------|------|
| Encoder update | Continuous | Maximum possible |
| UDP communication | 25 ms | 40 Hz |
| Motor control | 25 ms | 40 Hz |
| Debug printing | 100 ms | 10 Hz |
| LED toggle | 1000 ms | 1 Hz |

---

# Control Flow Summary

```text
Receive UDP command
        ↓
Convert RPM to RPS
        ↓
Execute wheel speed controllers
        ↓
Read encoder feedback
        ↓
Send robot state through UDP
```

---

# Current Features

- Closed-loop wheel speed control
- UDP communication
- Encoder feedback
- Independent motor PI controllers
- Emergency stop support
- Non-blocking timing architecture

---

# Future Improvements

Possible future improvements include:

- IMU integration for yaw estimation
- Full robot kinematics
- Cascaded control loops
- Odometry estimation
- ROS integration
- FreeRTOS task separation
- Feedforward wheel control
- Motion profiling
- Trajectory tracking

---
````
