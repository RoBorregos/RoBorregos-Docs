# PID Controller Documentation

## Overview

This document describes the implementation of the `PID` class used in the VSSS robot control system.

The controller is implemented as a static utility class and provides:

- Proportional control
- Integral control
- Derivative control
- Output clamping
- Anti-windup protection
- Direction-limited outputs
- Automatic reset functionality

The implementation is designed for embedded systems using Arduino-compatible hardware.

---

# File Structure

The implementation is divided into two files:

| File | Description |
|---|---|
| `pid.h` | Class declaration and parameter structure |
| `pid.cpp` | PID controller implementation |

---

# Header File

```cpp
#ifndef PICO_LIB_PID_H
#define PICO_LIB_PID_H
```

The include guards prevent multiple inclusions of the header file.

---

# Arduino Dependency

```cpp
#include <Arduino.h>
```

The PID controller depends on Arduino timing functions such as:

```cpp
millis()
```

---

# Direction Macros

```cpp
#define PID_ACCEPT_POSITIVES_ONLY true
#define PID_ACCEPT_NEGATIVES_ONLY false
```

These macros are used when the controller is configured to allow motion only in one direction.

---

# PID Class

```cpp
class PID
```

The class contains:

- PID parameter structure
- Reset function
- Compute function

All methods are static.

---

# PidParameters Structure

```cpp
struct PidParameters
```

This structure stores all controller variables, gains, states, and configuration parameters.

---

# PID Gains

## Proportional Gain

```cpp
float kp{};
```

Controls the proportional response.

The proportional term is:

:contentReference[oaicite:0]{index=0}

---

## Integral Gain

```cpp
float ki{};
```

Controls accumulated error compensation.

The integral term is:

:contentReference[oaicite:1]{index=1}

---

## Derivative Gain

```cpp
float kd{};
```

Controls damping and prediction of error changes.

The derivative term is:

:contentReference[oaicite:2]{index=2}

---

# Output Limits

## Maximum Output

```cpp
double max_output = 0.0;
```

Limits the maximum absolute controller output.

If:

```text
max_output > 0
```

the controller output is clamped between:

:contentReference[oaicite:3]{index=3}

---

## Minimum Output

```cpp
double min_output = 0.0;
```

Defines a deadband region.

If the absolute output is below this threshold, the output becomes zero.

Used to avoid:

- Motor buzzing
- Weak PWM signals
- Small ineffective commands

---

# Error Threshold

```cpp
float error_threshold = 0.0;
```

Defines the minimum error magnitude considered significant.

Used for:

- Integral activation control
- Optional automatic reset

---

# Internal PID State Variables

## Integral Error

```cpp
double integral_error = 0.0;
```

Stores accumulated error over time.

---

## Previous Error

```cpp
double previous_error = 0.0;
```

Used to calculate the derivative term.

---

## Last Iteration Time

```cpp
unsigned long long last_iteration_ms = 0;
```

Stores the timestamp of the previous computation.

---

# First Run Flag

```cpp
bool first_run = true;
```

Used to avoid invalid derivative calculations during initialization.

---

# Runtime Variables

## Target

```cpp
double target = 0.0;
```

Desired setpoint value.

---

## Error

```cpp
double error = 0.0;
```

Current control error.

Typically:

:contentReference[oaicite:4]{index=4}

---

## Output

```cpp
double output = 0.0;
```

Final PID controller output.

---

# One Direction Only Mode

```cpp
bool one_direction_only = false;
```

Restricts controller output to one direction.

Useful for systems such as:

- Fans
- Pumps
- Unidirectional motors

---

## Accepted Direction

```cpp
bool accept_type = PID_ACCEPT_POSITIVES_ONLY;
```

Possible modes:

| Mode | Description |
|---|---|
| `PID_ACCEPT_POSITIVES_ONLY` | Negative outputs are forced to zero |
| `PID_ACCEPT_NEGATIVES_ONLY` | Positive outputs are forced to zero |

---

# Automatic Reset Mode

```cpp
bool reset_within_threshold = false;
```

If enabled:

```text
When |error| < error_threshold
```

the controller automatically resets.

Useful to:

- Remove residual integral accumulation
- Stabilize systems near equilibrium

---

# Reset Function

```cpp
static void reset(PidParameters& pid);
```

Resets the controller internal state.

---

## Reseted Variables

The following variables are cleared:

```cpp
pid.integral_error = 0;
pid.previous_error = 0;
pid.output = 0;
pid.error = 0;
pid.target = 0;
pid.last_iteration_ms = 0;
pid.first_run = true;
```

---

# Compute Function

```cpp
static void compute(PidParameters& pid);
```

This function performs a complete PID update.

---

# Timing Calculation

The controller uses:

```cpp
millis()
```

to calculate elapsed time.

## Delta Time

:contentReference[oaicite:5]{index=5}

Converted to seconds:

```cpp
const double delta_time_s =
    static_cast<double>(delta_time_ms) / 1000.0;
```

---

# First Run Behavior

During the first execution:

- Derivative calculation is skipped
- Previous timestamps are initialized
- Initial proportional output is generated

This prevents division-by-zero and unstable startup behavior.

---

# Proportional Term

Calculated using:

:contentReference[oaicite:6]{index=6}

Implementation:

```cpp
const double pTerm = pid.kp * pid.error;
```

---

# Integral Term

Accumulated using:

:contentReference[oaicite:7]{index=7}

Implementation:

```cpp
pid.integral_error += pid.error * delta_time_s;
```

---

# Anti-Windup Protection

The controller includes anti-windup logic.

Integration stops if:

- Output is saturated
- Error pushes further into saturation

Condition:

```cpp
if (
    ((pid.output >= pid.max_output) && (pid.error > 0))
    || ((pid.output <= -pid.max_output) && (pid.error < 0))
)
```

This prevents excessive integral accumulation.

---

# Derivative Term

Calculated using:

:contentReference[oaicite:8]{index=8}

Implementation:

```cpp
const double error_derivate =
    (pid.error - pid.previous_error) / delta_time_s;
```

---

# Final PID Equation

The final output is:

:contentReference[oaicite:9]{index=9}

Implementation:

```cpp
double calculate_output =
    pTerm + iTerm + dTerm;
```

---

# Output Clamping

The output is clamped using:

```cpp
clamp(calculate_output,
      -pid.max_output,
       pid.max_output);
```

This prevents unsafe actuator commands.

---

# Deadband Logic

If:

```cpp
fabs(calculate_output) < pid.min_output
```

then:

```cpp
calculate_output = 0.0;
```

This creates a deadband region.

---

# Direction Filtering

If one-direction mode is enabled:

```cpp
if (pid.one_direction_only)
```

the controller filters invalid output directions.

Example:

```cpp
if (pid.output < 0)
    pid.output = 0;
```

---

# Automatic Reset Logic

If enabled:

```cpp
if (pid.reset_within_threshold &&
    (fabs(pid.error) < pid.error_threshold))
```

the controller calls:

```cpp
reset(pid);
```

---

# Control Flow Summary

```text
Read current error
        ↓
Compute delta time
        ↓
Calculate proportional term
        ↓
Apply anti-windup logic
        ↓
Calculate integral term
        ↓
Calculate derivative term
        ↓
Compute final output
        ↓
Clamp output
        ↓
Apply direction filtering
        ↓
Store current state
```

---

# Features Summary

## Implemented Features

- Full PID controller
- Variable timestep calculation
- Anti-windup protection
- Output saturation
- Deadband region
- Direction-limited outputs
- Automatic reset support
- Embedded-system friendly implementation

---

# Typical Embedded Applications

This controller can be used for:

- DC motor velocity control
- Position control
- Line followers
- Temperature control
- Balancing robots
- Drone stabilization
- Wheel speed regulation

---

# Example Usage

```cpp
PID::PidParameters motor_pid;

motor_pid.kp = 10;
motor_pid.ki = 250;
motor_pid.kd = 0;

motor_pid.max_output = 255;

motor_pid.target = desired_speed;
motor_pid.error = desired_speed - measured_speed;

PID::compute(motor_pid);

analogWrite(PWM_PIN, motor_pid.output);
```

---

# Future Improvements

Possible improvements include:

- Derivative filtering
- Feedforward control
- Adaptive PID gains
- Integral clamping
- Fixed timestep mode
- Auto-tuning methods
- Low-pass derivative filter
- Floating-point optimization

---
````
