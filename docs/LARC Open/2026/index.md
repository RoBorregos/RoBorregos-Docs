# LARC Open 2026 — Autonomous Coffee Harvesting Robot

This page summarizes the LARC Open 2026 proposal developed by RoBorregos for the
autonomous coffee harvesting challenge. The project focuses on an omni-directional
robot capable of navigating the arena, detecting coffee fruits, avoiding obstacles,
following lines, and depositing collected objects autonomously with minimal human
intervention.

The robot was designed with a strong focus on reliability, modularity, and recovery
strategies to maintain stable behavior during long autonomous runs under changing
arena conditions.

**Objectives**
- Design an autonomous robot capable of locating and collecting coffee fruits/items
  distributed throughout the arena.
- Navigate safely through the field while avoiding collisions and maintaining
  accurate positioning.
- Implement reliable motion control using PID controllers, odometry, and orientation
  stabilization.
- Develop a modular state machine capable of handling autonomous navigation,
  collection, alignment, and recovery behaviors.
- Maximize reliability through sensor fusion and fallback systems to prevent the
  robot from getting stuck during operation.

This page contains a general overview of the project and links to the technical
documentation of each subsystem.

---

## More about this challenge

LARC Open 2026 simulates an autonomous harvesting scenario in which the robot must
collect and transport coffee fruits/items within a limited amount of time.

The challenge requires the integration of several robotics areas, including:

- Autonomous navigation
- Real-time control systems
- Sensor fusion
- Embedded systems
- Obstacle avoidance
- Line following
- Object detection
- Autonomous manipulation
- State-machine-based behaviors

The robot combines omniwheel kinematics, encoder-based odometry, IMU orientation
feedback, ToF distance sensors, IR sensors, QTR line sensors, and computer vision
to achieve reliable autonomous operation.

---

## System Overview

The software architecture is divided into independent modules to simplify testing,
debugging, and scalability.

### Control
- PID controllers
- Motion control
- Odometry
- Orientation stabilization
- Omniwheel kinematics

### Sensors
- BNO055 IMU
- VL53L0X ToF sensors
- QTR line sensors
- IR sensors
- 74HC4067 multiplexers

### Vision
- YOLO-based object detection
- NVIDIA Jetson Xavier integration
- Serial communication with Teensy

### Autonomous Logic
- State machine architecture
- Recovery strategies
- Sensor fallback systems

Each subsystem is documented individually in the following pages.

---

## State Machine Overview

The robot behavior is controlled through a modular state machine designed to
handle navigation, obstacle avoidance, line detection, collection, and recovery
behaviors.

Main states include:

- `START`
  - Elevator initialization and startup sequence.

- `POOL`
  - Main navigation state.
  - The robot moves forward while avoiding obstacles using distance sensors.

- `LOOKFORLINE`
  - Searches for the line used to align with the collection area.
  - Uses QTR sensors for lateral correction.

- `LOOKFORCORNER`
  - Detects the corner/target alignment zone before collection.

- `BEANS`
  - Main collection behavior.
  - Controls intake systems and alignment.

- `BEANSGOBACK`
  - Recovery and repositioning behavior after collection.

- `STOP`
  - Safe stop state.

The modular design allows transitions between behaviors while maintaining stable
orientation and controlled movement.

---

## Main Features

- Omni-directional movement using four omni wheels
- Yaw stabilization using BNO055 + PID control
- Encoder-based odometry for translational tracking
- Line following with QTR lateral correction
- Multi-sensor obstacle detection
- Sensor debouncing and hysteresis filtering
- Fault-tolerant fallback strategies
- Modular state machine architecture
- Real-time communication between embedded systems
- Autonomous collection and deposit routines

---

## Navigation and Control

To maintain stable movement during autonomous operation, the robot combines
multiple control systems simultaneously.

A PID controller is used to maintain the robot orientation using feedback from
the BNO055 IMU. This allows the robot to compensate for wheel slip, uneven
friction, and external disturbances during movement.

Encoder-based odometry is used to estimate translational displacement, while
distance sensors provide additional environmental feedback during navigation.

The robot also implements fallback systems for sensor failures. If a primary
distance sensor becomes invalid, the system switches to alternative sensors
or encoder estimation to maintain operation reliability.

---

## Rules & Scoring (summary)

Official rules and complete scoring details are available in the event documentation.

Summary of important constraints:

- Runs are fully autonomous and time-limited.
- No external human control is allowed during operation.
- Points are awarded for successfully collected and deposited items.
- Collisions, unstable behavior, or leaving the allowed area may generate penalties.
- Reliability and consistency are prioritized over aggressive movement.

For precise scoring, penalties, and technical regulations, consult the official
competition documentation.