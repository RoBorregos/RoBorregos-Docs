# Control

## Ximena Patricia García

The control architecture of the LARC Open 2026 robot was designed to provide
stable, modular, and reliable autonomous behavior during competition runs.

This section documents the main control systems used in the robot, including the
state machine, movement control, odometry integration, PID controllers, and the
different subsystems responsible for autonomous navigation and manipulation.

A major improvement compared to the LARC Open 2025 robot was the migration from
a mecanum-wheel base to an omnidirectional X-drive configuration using four omni
wheels placed at 45-degree angles. This improved movement stability, lateral
control, and obstacle avoidance performance during autonomous operation.

The robot combines encoder-based odometry, orientation stabilization, line
following, and sensor feedback to maintain controlled movement throughout the
routine. Encoder feedback was especially important due to the weight of the
robot, since small weight imbalances could affect wheel behavior and movement
uniformity. The use of encoders significantly improved movement consistency and
overall control stability.

The control system was organized into independent modules to simplify debugging,
testing, and future improvements.

This section includes documentation for:

- Sensors
- State Machine
- Subsystems